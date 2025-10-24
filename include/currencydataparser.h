#pragma once
#include "idataparser.h"
#include <nlohmann/json.hpp>

struct CurrencyData : public IDataParser::Result
{
    std::string date;
    std::string currency;
    double rate;
    void logCurrencyInfo() const;
};

class CurrencyDataParser : public IDataParser
{
  public:
    void parse(const std::string &data) override;
    void getData(Result &result) override;

  private:
    nlohmann::json json_;
};
