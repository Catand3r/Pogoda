#pragma once
#include "Http.h"
#include "Logger.h"
#include "Scheduler.h"
#include "inicpp.h"
#include "isqlengine.h"
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>

class Wttr
{
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

        //todo
        scheduler_.addTask(
            [this]() {
                try
                {
                    if (!outFile_.is_open())
                    {
                        outFile_.open("../pogoda.json", std::ios::trunc);
                    }

                    for (const auto &city : cities_)
                    {
                        Logger::getInstance().logInfo("Fetching weather for city: " + city);
                        auto response = HttpClient::getInstance().get("https://wttr.in/" + city + "?format=j1");

                        if (!outFile_.is_open())
                        {
                            Logger::getInstance().logError("Failed to open output file");
                            return;
                        }

                        // zamiast nlohmann -> IDataParser
                        json_ = nlohmann::json::parse(response);

                        // from_json() // struct WeatherData{ cirt, desc, temp, feels, humidity, wind}
                        std::string city = json_["nearest_area"][0]["areaName"][0]["value"].get<std::string>();
                        std::string desc = json_["current_condition"][0]["weatherDesc"][0]["value"].get<std::string>();
                        std::string temp = json_["current_condition"][0]["tempC"].get<std::string>();
                        std::string feels = json_["current_condition"][0]["FeelsLikeC"].get<std::string>();
                        std::string humidity = json_["current_condition"][0]["humidity"].get<std::string>();
                        std::string wind = json_["current_condition"][0]["windspeedKmph"].get<std::string>();

                        std::ostringstream oss;
                        oss << "INSERT INTO Pogoda(city, description, temperature, humidity, wind) VALUES("
                            << "'" << city << "', "
                            << "'" << desc << "', "
                            << "'" << temp << " (" << feels << ")" << "', "
                            << "'" << humidity << "', "
                            << "'" << wind << "');";

                        std::string query = oss.str();

                        ISQLEngine::QueryResult queryResult;

                        if (!db_->exec(query, queryResult))
                        {
                            Logger::getInstance().logError("Failed to execute query: " + query);
                            continue;
                        }
                    }
                }
                catch (const std::exception &e)
                {
                    Logger::getInstance().logError("Error fetching weather data: " + std::string(e.what()));
                    return;
                }
            },
            readPeriod_);

        scheduler_.addTask([this]() { Logger::getInstance().Write(); },
                           5); // co 5 sekund flush do pliku

        scheduler_.run();
    }

  private:
    std::unique_ptr<ISQLEngine> db_; // ISQLEngine& // std::unique_ptr<ISQLEngine>

    std::ofstream outFile_;

    nlohmann::json json_; // IDataParser& // std::unique_ptr<IDataParser>

    std::vector<std::string> cities_;

    int readPeriod_ = 0;

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

        readPeriod_ = iniFile["Settings"]["ReadPeriod"].as<int>(); // exception
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
