#pragma once
#include "idataparser.h"
#include <nlohmann/json.hpp>

struct WeatherData : public IDataParser::Result
{
    std::string time;
    std::string city;
    std::string desc;
    std::string temp;
    std::string feels;
    std::string humidity;
    std::string wind;
    void logWeatherInfo() const;
};

class WeatherDataParser : public IDataParser
{
  public:
    bool parse(const std::string &data) override;
    void getData(Result &result) override;

  private:
    nlohmann::json json_;
};
