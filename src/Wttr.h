#pragma once
#include "Http.h"
#include "IDataParser.h"
#include "IniWrapper.h"
#include "Logger.h"
#include "Scheduler.h"
#include "inicpp.h"
#include "isqlengine.h"
#include "WeatherFetchingTask.h"
#include "LoggingTask.h"

#include <fstream>
#include <memory>

class Wttr
{
public:
    using Cities = std::vector<std::string>;
  public:
    Wttr(const std::string &iniFileSrc)
    {
        ini_ = std::make_unique<IniWrapper>(iniFileSrc);
        parser_ = std::make_unique<DataParser>();

        readIniFile();
        /*if (!createDB())
        {
            Logger::getInstance().logError("Failed to create or open database");
            throw std::runtime_error("Database initialization failed");
        }
        else
        {
            Logger::getInstance().logInfo("Database initialized successfully");
        }*/
    }

    ~Wttr()
    {
        Scheduler::getInstance().stop();

        if (outFile_.is_open())
        {
            outFile_.close();
        }
    }

    void run()
    {
        Scheduler &scheduler_ = Scheduler::getInstance();


        auto weatherTask = std::make_unique<WeatherFetchingTask>(cities_, *db_, *parser_, readPeriod_);
        scheduler_.addTask(std::move(weatherTask));

        auto loggingTask = std::make_unique<LoggingTask>(5000);
        scheduler_.addTask(std::move(loggingTask));

        scheduler_.run();
    }

  private:
    std::unique_ptr<ISQLEngine> db_;

    std::unique_ptr<IIniWrapper> ini_;

    std::ofstream outFile_;

    std::unique_ptr<IDataParser> parser_;

    Cities cities_;

    uint64_t readPeriod_ = 0; // miliseconds

    void readIniFile()
    {
        readPeriod_ = std::stoi(ini_->getValue("Settings", "ReadPeriod", "5")) * 1000;

        cities_ = split(ini_->getValue("Settings", "Cities", "Warsaw,Krakow,Gdansk"), ',');
    }

    bool openDB()
    {
        if (!db_->isOpen())
        {
            return db_->open();
        }
        return true;
    }

    bool createDB()
    {
        if (!openDB())
        {
            Logger::getInstance().logError("Failed to open database");
            throw std::runtime_error("Database file loading failed");
            return false;
        }
        ISQLEngine::QueryResult result;
        return db_->exec("CREATE TABLE IF NOT EXISTS Pogoda (time TEXT, city TEXT, desc TEXT, temp REAL, humidity"
                         "REAL, wind REAL);",
                         result);
    }

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
};
