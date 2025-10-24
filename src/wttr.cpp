#include "wttr.h"
#include "currencydataparser.h"
#include "currencyfetchingtask.h"
#include "logger.h"
#include "loggingtask.h"
#include "scheduler.h"
#include "weatherdataparser.h"
#include "weatherfetchingtask.h"

Wttr::Wttr(const std::string &iniFileSrc)
{
    ini_ = std::make_unique<IniWrapper>(iniFileSrc);
    db_ = std::make_unique<SQLiteDB>("weather.db");

    if (!createDatabase())
    {
        Logger::getInstance().logError("Database creation failed");
    }
}

Wttr::~Wttr()
{
    Scheduler::getInstance().stop();

    if (outFile_.is_open())
    {
        outFile_.close();
    }
}

void Wttr::run()
{
    Scheduler &scheduler_ = Scheduler::getInstance();

    auto weatherInterval = std::stoull(ini_->getValue("Weather", "ReadPeriod", "5000"));
    weatherparser_ = std::make_unique<WeatherDataParser>();

    auto weatherTask = std::make_unique<WeatherFetchingTask>(*ini_, *db_, *weatherparser_, weatherInterval);
    weatherTask->Init();
    scheduler_.addTask(std::move(weatherTask));

    auto currencyInterval = std::stoull(ini_->getValue("Currency", "ReadPeriod", "5000"));
    currencyparser_ = std::make_unique<CurrencyDataParser>();

    auto currencyTask = std::make_unique<CurrencyFetchingTask>(*ini_, *db_, *currencyparser_, currencyInterval);
    currencyTask->Init();
    scheduler_.addTask(std::move(currencyTask));

    auto loggingTask = std::make_unique<LoggingTask>(5000);
    scheduler_.addTask(std::move(loggingTask));

    scheduler_.run();
}

bool Wttr::openDB()
{
    if (!db_->isOpen())
    {
        return db_->open();
    }
    return true;
}

bool Wttr::createDatabase()
{
    if (!openDB())
    {
        Logger::getInstance().logError("Failed to open database");
        throw std::runtime_error("Database file loading failed");
        return false;
    }
    ISQLEngine::QueryResult result;
    return db_->exec("CREATE TABLE IF NOT EXISTS Pogoda (time TEXT, city TEXT, desc TEXT, temp REAL, humidity"
                     " REAL, wind REAL, UNIQUE(time,city));",
                     result);
}
