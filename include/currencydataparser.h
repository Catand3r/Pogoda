#pragma once
#include "idataparser.h"
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

struct Rate
{
    std::string date;
    double mid;
};

struct CurrencyData : public IDataParser::Result
{
    std::string code;
    std::vector<Rate> rates;
    void logCurrencyInfo() const;
};

class CurrencyDataParser : public IDataParser
{
  public:
    bool parse(const std::string &data) override;
    void getData(Result &result) override;

  private:
    nlohmann::json json_;
};
