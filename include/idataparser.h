#pragma once
#include <string>

class IDataParser
{
  public:
    struct Result
    {
    };

  public:
    virtual void parse(const std::string &data) = 0;
    virtual void getData(Result &) = 0;
};
