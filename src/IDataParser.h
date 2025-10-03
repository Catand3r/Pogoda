#pragma once
#include <string>

struct WeatherData;

class IDataParser
{
  public:
    virtual void parse(const std::string &data) = 0;
    virtual void getWeatherData(WeatherData &) = 0;
};
