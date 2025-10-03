#include "WeatherFetchingTask.h"
#include "Logger.h"
#include "isqlengine.h"
#include "Http.h"
#include <nlohmann/json.hpp>

WeatherFetchingTask::WeatherFetchingTask(const Cities& cities, ISQLEngine& db,  uint64_t interval) : ITask(std::chrono::milliseconds(interval)), cities_(cities), db_(db)
{

}


void WeatherFetchingTask::Run()
{
    for (const auto &city : cities_)
    {
        try
        {
            Logger::getInstance().logInfo("Fetching weather for city: " + city);
            auto response = HttpClient::getInstance().get("https://wttr.in/" + city + "?format=j1");

             auto json_ = nlohmann::json::parse(response);

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

            if (!db_.exec(query, queryResult))
            {
                Logger::getInstance().logError("Failed to execute query: " + query);
                continue;
            }
        }
        catch (const std::exception &e)
        {
            Logger::getInstance().logError("Error fetching weather data: " + std::string(e.what()));
            return;
        }
    }
}
