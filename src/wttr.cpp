#include "wttr.h"
#include "logger.h"
#include "loggingtask.h"
#include "scheduler.h"
#include "weatherfetchingtask.h"

std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

Wttr::Wttr(const std::string &iniFileSrc)
{
    ini_ = std::make_unique<IniWrapper>(iniFileSrc);
    parser_ = std::make_unique<DataParser>();
    db_ = std::make_unique<SQLiteDB>("weather.db");

    readIniFile();

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

    auto weatherTask = std::make_unique<WeatherFetchingTask>(cities_, *db_, *parser_, readPeriod_);
    scheduler_.addTask(std::move(weatherTask));

    auto loggingTask = std::make_unique<LoggingTask>(5000);
    scheduler_.addTask(std::move(loggingTask));

    scheduler_.run();
}

void Wttr::readIniFile()
{
    readPeriod_ = std::stoi(ini_->getValue("Settings", "ReadPeriod", "5")) * 1000;

    cities_ = split(ini_->getValue("Settings", "Cities", "Warsaw,Krakow,Gdansk"), ',');
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
                     " REAL, wind REAL);",
                     result);
}
