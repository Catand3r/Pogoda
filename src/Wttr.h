#pragma once
#include "Http.h"
#include "Logger.h"
#include "Scheduler.h"
#include "inicpp.h"
#include "isqlengine.h"
#include "WeatherFetchingTask.h"
#include "LoggingTask.h"

#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>

class Wttr
{
public:
    using Cities = std::vector<std::string>;
  public:
    Wttr(const std::string &iniFileSrc)
    {
        readIniFile(iniFileSrc);
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


        auto weatherTask = std::make_unique<WeatherFetchingTask>(cities_, *db_, readPeriod_);
        scheduler_.addTask(std::move(weatherTask));

        auto loggingTask = std::make_unique<LoggingTask>(5000);
        scheduler_.addTask(std::move(loggingTask));

        scheduler_.run();
    }

  private:
    std::unique_ptr<ISQLEngine> db_; // ISQLEngine& // std::unique_ptr<ISQLEngine>

    std::ofstream outFile_;

    nlohmann::json json_; // IDataParser& // std::unique_ptr<IDataParser>

    Cities cities_;

    uint64_t readPeriod_ = 0; // miliseconds

    void readIniFile(const std::string &iniFileSrc)
    {
        auto &logger = Logger::getInstance();

        ini::IniFile iniFile;
        try
        {
            iniFile.load(iniFileSrc);
            logger.logInfo("INI file loaded successfully: " + iniFileSrc);
        }
        catch (const std::exception &e)
        {
            logger.logError("Error loading INI file: " + std::string(e.what()));
            throw;
        }

        readPeriod_ = iniFile["Settings"]["ReadPeriod"].as<int>() * 1000; // exception
        /*
        GetString("Settings", "ReadPeriod", 5);

        GetString(section, key, defaultValue)
        {
        try
        {
        value = iniFile[section][key].as<std::string>());
        }
        catch(...)
        {
        return defaultValue;
        }
        }

        */
        cities_ = split(iniFile["Settings"]["Cities"].as<std::string>(), ',');
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
        return db_->exec("CREATE TABLE IF NOT EXISTS Pogoda (Miasto TEXT, Warunki TEXT, Temperatura REAL, Wilgotnosc "
                         "REAL, Predkosc wiatru REAL);",
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
