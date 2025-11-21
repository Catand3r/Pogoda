#include "currencyfetchingtask.h"
#include "http.h"
#include "iiniwrapper.h"
#include "logger.h"
#include <algorithm>
#include <format>
#include <utility>

bool IsWeekend(const std::chrono::sys_days &date)
{
    auto weekday = std::chrono::weekday{date};
    return weekday == std::chrono::Saturday || weekday == std::chrono::Sunday;
}

bool DateExists(const std::vector<std::chrono::sys_days> &dates, const std::chrono::sys_days &date)
{
    return std::any_of(dates.begin(), dates.end(), [&date](const auto &d) { return d == date; }) ||
                             IsWeekend(date);
}

std::vector<std::chrono::sys_days> ExtractDatesFromQuery(const ISQLEngine::QueryResult &queryResult)
{
    std::vector<std::chrono::sys_days> dates;
    dates.reserve(queryResult.size());

    for (const auto &row : queryResult)
    {
        for (const auto &col : row)
        {
            if (col.first == "time")
            {
                std::istringstream iss(col.second);
                std::chrono::year_month_day ymd;
                iss >> std::chrono::parse("%F", ymd);
                dates.push_back(std::chrono::sys_days{ymd});
            }
        }
    }
    return dates;
}

std::string GetStringFromDate(const std::chrono::sys_days &date)
{
    return std::format("{:%Y-%m-%d}", date);
}

void CurrencyFetchingTask::Init()
{
    ReadIniFile();
}

void CurrencyFetchingTask::ReadIniFile()
{
    currencies_ = ini_.getValues("Currency", "Currencies", {"USD", "EUR", "GBP"});
    rateminhistory_ = ini_.getValue("Currency", "RateMinHistory", "40");
}

std::vector<CurrencyFetchingTask::Range> CurrencyFetchingTask::setRanges(
    const std::vector<std::chrono::sys_days> &queryRes, const std::chrono::sys_days &currentTime,
    const std::chrono::sys_days &oldestTime)
{
    auto ranges = std::vector<Range>{};
    std::chrono::sys_days currentLeft = oldestTime;
    std::chrono::sys_days currentRight = currentLeft;
    while (currentRight != currentTime && currentLeft != currentTime)
    {
        bool leftDateExists = true;
        while (leftDateExists && currentLeft < currentTime)
        {
            leftDateExists = DateExists( queryRes, currentLeft);

            if (leftDateExists)
                currentLeft += std::chrono::days(1);
        }

        currentRight = currentLeft;

        bool rightDateExists = false;
        while (!rightDateExists && currentRight != currentTime)
        {
            rightDateExists = DateExists(queryRes, currentRight + std::chrono::days(1));

            if (!rightDateExists)
                currentRight += std::chrono::days(1);
        }

        ranges.push_back(std::make_pair(currentLeft, currentRight));

        currentLeft = currentRight + std::chrono::days(1);
    }

    return ranges;
}

void CurrencyFetchingTask::Run()
{
    for (const auto &currency : currencies_)
    {
        try
        {
            RunForCurrency(currency);
        }
        catch (const std::exception &e)
        {
            Logger::getInstance().logError("Error fetching currency data: " + std::string(e.what()));
        }
    }
}

void CurrencyFetchingTask::RunForCurrency(const std::string& currency)
{
    auto date = std::chrono::sys_days(std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())});
    auto rangeDate = date - std::chrono::days(std::stoi(rateminhistory_));
    auto ymdRange = std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(rangeDate)};

    ISQLEngine::QueryResult rangeResult = GetExistingDatesFromDB(currency, ymdRange);
    if( rangeResult.empty())
        return;

    std::vector<std::chrono::sys_days> existingDates = ExtractDatesFromQuery(rangeResult);
    auto ranges = setRanges(existingDates, date, rangeDate);

    std::vector<CurrencyData> currencyDataList; 

    for (auto &range : ranges)
        currencyDataList.push_back(FetchingDataForRange(currency, range));

    SaveDataInDB( currencyDataList);
}

ISQLEngine::QueryResult CurrencyFetchingTask::GetExistingDatesFromDB(const std::string& currency, const std::chrono::year_month_day& ymdRange)
{
    std::ostringstream rangeQuery;
    rangeQuery << "SELECT time FROM Waluta WHERE code = '" << currency << "' AND time >= '" << ymdRange
               << "' ORDER BY time DESC;";

    ISQLEngine::QueryResult rangeResult;

    if (!db_.exec(rangeQuery.str(), rangeResult))
    {
        Logger::getInstance().logError("Failed to execute query: " + rangeQuery.str());
        return {};
    }

    return rangeResult;
}

CurrencyData CurrencyFetchingTask::FetchingDataForRange(const std::string& currency, const CurrencyFetchingTask::Range& range)
{
    const auto startDate = GetStringFromDate(range.first);
    const auto endDate = GetStringFromDate(range.second);

    Logger::getInstance().logInfo("Fetching currency data for: " + currency + "From: " + startDate + " to: " + endDate);
    auto response = HttpClient::getInstance().get("https://api.nbp.pl/api/exchangerates/rates/a/" + currency + "/" +
                                                       startDate + "/" + endDate + "/?format=json");

    CurrencyData cd;

    if (parser_.parse(response))
        parser_.getData(cd);
    else
    {
        Logger::getInstance().logWarning("No data available for currency: " + currency + " from " +
                                              startDate + " to " + endDate);

        cd.code = currency;
        for (auto dt = range.first; dt <= range.second; dt += std::chrono::days(1))
        {
            Rate rate;
            rate.date = GetStringFromDate(dt);
            rate.mid = -1.0;
            cd.rates.push_back(rate);
        }
    }

    return cd;
}

bool CurrencyFetchingTask::SaveDataInDB(const std::vector<CurrencyData>& currencyDataList)
{
    std::string values;

    for (auto &cd : currencyDataList)
    {
        for (int i = 0; i < cd.rates.size(); i++)
        {
            const auto &rate = cd.rates[i];
            std::ostringstream qss;
            qss << "('" << rate.date << "', '" << cd.code << "', ";
            if (rate.mid == -1)
                qss << "NULL";
            else
                qss << rate.mid;
                qss << ")";
            if (i < cd.rates.size() - 1 || &cd != &currencyDataList.back())
            {
                qss << ", ";
            }
            values += qss.str();
        }
    }

    std::ostringstream oss;
    oss << "INSERT OR IGNORE INTO Waluta(time, code, rate) VALUES" << values << ";";

    std::string query = oss.str();

    ISQLEngine::QueryResult queryResult;

    if (!db_.exec(query, queryResult))
    {
        Logger::getInstance().logError("Failed to execute query: " + query);
        return false;
    }

    return true;
}
