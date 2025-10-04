#pragma once
#include "idataparser.h"
#include <nlohmann/json.hpp>

struct WeatherData
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

class DataParser : public IDataParser
{
  public:
    void parse(const std::string &data) override;
    void getWeatherData(WeatherData &wd) override;

  private:
    nlohmann::json json_;
};
