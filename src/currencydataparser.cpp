#include "currencydataparser.h"
#include "logger.h"

void from_json(const json &j, Rate &r)
{
    j.at("effectiveDate").get_to(r.date);
    j.at("mid").get_to(r.mid);
}

void from_json(const json &j, CurrencyData &resp)
{
    j.at("code").get_to(resp.code);
    j.at("rates").get_to(resp.rates);
}

void CurrencyData::logCurrencyInfo() const
{
    std::string logMsg = "History: " + std::to_string(rates.size()) + " code: " + code + ": ";
    for (auto &rate : rates)
    {
        logMsg += "Date: " + rate.date + ", Mid: " + std::to_string(rate.mid) + "; ";
    }
    Logger::getInstance().logInfo(logMsg);
}

void CurrencyDataParser::parse(const std::string &data)
{
    try
    {
        json_ = nlohmann::json::parse(data);
        Logger::getInstance().logInfo("JSON parsed successfully");
    }
    catch (const nlohmann::json::parse_error &e)
    {
        Logger::getInstance().logError("JSON parse error: " + std::string(e.what()));
    }
}

void CurrencyDataParser::getData(IDataParser::Result &result)
{
    auto &cd = static_cast<CurrencyData &>(result);
    cd = json_;
}
