#include "weatherfetchingtask.h"
#include "dataparser.h"
#include "http.h"
#include "isqlengine.h"
#include "logger.h"

WeatherFetchingTask::WeatherFetchingTask(const Cities &cities, ISQLEngine &db, IDataParser &parser, uint64_t interval)
    : ITask(std::chrono::milliseconds(interval)), cities_(cities), db_(db), parser_(parser)
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

            parser_.parse(response);

            WeatherData wd;
            parser_.getWeatherData(wd);
            wd.logWeatherInfo();

            std::ostringstream oss;
            oss << "INSERT OR IGNORE INTO Pogoda(time, city, desc, temp, humidity, wind) VALUES("
                << "'" << wd.time << "', "
                << "'" << wd.city << "', "
                << "'" << wd.desc << "', "
                << "'" << wd.temp << " (" << wd.feels << ")" << "', "
                << "'" << wd.humidity << "', "
                << "'" << wd.wind << "');";

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
