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
}

void CurrencyFetchingTask::Run()
{
    for (const auto &currency : currencies_)
    {
        try
        {
            Logger::getInstance().logInfo("Fetching currency data for: " + currency);
            auto response = HttpClient::getInstance().get("https://api.nbp.pl/api/exchangerates/rates/A/" + currency +
                                                          "/?format=json");

            parser_.parse(response);

            CurrencyData cd;
            parser_.getData(cd);
            cd.logCurrencyInfo();
        }
        catch (const std::exception &e)
        {
            Logger::getInstance().logError("Error fetching currency data: " + std::string(e.what()));
        }
    }
}
