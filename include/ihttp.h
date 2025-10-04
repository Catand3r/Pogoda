#pragma once
#include <string>

class IHttp
{
  public:
    using Package = std::pair<bool, std::string>;
    virtual std::string get(const std::string &url) = 0;
};
