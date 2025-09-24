#pragma once
#include "Logger.h"
#include <nlohmann/json.hpp>
#include <string>

struct WeatherData
{
    std::string time;
    std::string city;
    std::string desc;
    std::string temp;
    std::string feels;
    std::string humidity;
    std::string wind;
    void logWeatherInfo() const
    {
        Logger::getInstance().logInfo("Time: " + time + ", City: " + city + ", Description: " + desc +
                                      ", Temperature: " + temp + " (Feels like: " + feels + "), Humidity: " + humidity +
                                      ", Wind: " + wind);
    }
};

void from_json(const nlohmann::json &j, WeatherData &wd)
{
    j.at("current_condition").at(0).at("localObsDateTime").get_to(wd.time);
    j.at("nearest_area").at(0).at("areaName").at(0).at("value").get_to(wd.city);
    j.at("current_condition").at(0).at("weatherDesc").at(0).at("value").get_to(wd.desc);
    j.at("current_condition").at(0).at("temp_C").get_to(wd.temp);
    j.at("current_condition").at(0).at("FeelsLikeC").get_to(wd.feels);
    j.at("current_condition").at(0).at("humidity").get_to(wd.humidity);
    j.at("current_condition").at(0).at("windspeedKmph").get_to(wd.wind);
}

class IDataParser
{
  public:
    virtual void parse(const std::string &data) = 0;
    virtual void getWeatherData(WeatherData &) = 0;
};

class DataParser : public IDataParser
{
  public:
    void parse(const std::string &data) override
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
    void getWeatherData(WeatherData &wd) override
    {
        from_json(json_, wd);
    }

  private:
    nlohmann::json json_;
    // void from_json(const nlohmann::json &j, WeatherData &wd);
};
