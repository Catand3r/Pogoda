#pragma once
#include <string>

class IDataParser
{
  public:
    struct Result
    {
    };

  public:
    virtual bool parse(const std::string &data) = 0;
    virtual void getData(Result &) = 0;
};
