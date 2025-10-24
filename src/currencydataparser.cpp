#include "currencydataparser.h"
#include "logger.h"

void from_json(const nlohmann::json &j, CurrencyData &cd)
{
    j.at("rates").at(0).at("effectiveDate").get_to(cd.date);
    j.at("code").get_to(cd.currency);
    j.at("rates").at(0).at("mid").get_to(cd.rate);
}

void CurrencyData::logCurrencyInfo() const
{
    Logger::getInstance().logInfo("Date: " + date + ", Currency: " + currency + ", Rate: " + std::to_string(rate));
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
