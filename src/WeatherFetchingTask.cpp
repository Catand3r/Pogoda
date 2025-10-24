#include "weatherfetchingtask.h"
#include "http.h"
#include "iniwrapper.h"
#include "isqlengine.h"
#include "logger.h"
#include "weatherdataparser.h"

WeatherFetchingTask::WeatherFetchingTask(IIniWrapper &ini, ISQLEngine &db, IDataParser &parser, uint64_t interval)
    : ITask(std::chrono::milliseconds(interval)), ini_(ini), db_(db), parser_(parser)
{
}

void WeatherFetchingTask::Init()
{
    ReadIniFile();
}

void WeatherFetchingTask::ReadIniFile()
{
    cities_ = ini_.getValues("Weather", "Cities", {"Warsaw", "Krakow", "Gdansk"});
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
            parser_.getData(wd);
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
