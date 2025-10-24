#include "currencyfetchingtask.h"
#include "currencydataparser.h"
#include "http.h"
#include "iiniwrapper.h"
#include "isqlengine.h"
#include "logger.h"

void CurrencyFetchingTask::Init()
{
    ReadIniFile();
}

void CurrencyFetchingTask::ReadIniFile()
{
    currencies_ = ini_.getValues("Currency", "Currencies", {"USD", "EUR", "GBP"});
    rateminhistory_ = ini_.getValue("Currency", "RateMinHistory", "40");
}

void CurrencyFetchingTask::Run()
{
    for (const auto &currency : currencies_)
    {
        try
        {
            Logger::getInstance().logInfo("Fetching currency data for: " + currency);
            auto response = HttpClient::getInstance().get("https://api.nbp.pl/api/exchangerates/rates/a/" + currency +
                                                          "/last/" + rateminhistory_ + "/?format=json");

            parser_.parse(response);

            CurrencyData cd;
            parser_.getData(cd);
            cd.logCurrencyInfo();

            std::string values;
            for (int i = 0; i < cd.rates.size(); i++)
            {
                const auto &rate = cd.rates[i];
                std::ostringstream qss;
                qss << "('" << rate.date << "', '" << cd.code << "', " << rate.mid << ")";
                if (i == cd.rates.size() - 1)
                {
                    qss << ";";
                }
                else
                {
                    qss << ", ";
                }
                values += qss.str();
            }

            std::ostringstream oss;
            oss << "INSERT OR IGNORE INTO Waluta(time, code, rate) VALUES" << values;

            std::string query = oss.str();

            ISQLEngine::QueryResult queryResult;

            if (!db_.exec(query, queryResult))
            {
                Logger::getInstance().logError("Failed to execute query: " + query);
                continue;
            }
        }
        catch (const std::exception &e)
        {
            Logger::getInstance().logError("Error fetching currency data: " + std::string(e.what()));
        }
    }
}
