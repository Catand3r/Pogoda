#pragma once
#include <string>

class IIniWrapper
{
  public:
    IIniWrapper(const std::string &src) : src_(src)
    {
    }

    virtual std::string getValue(const std::string &section, const std::string &key,
                                 const std::string &defaultValue) const = 0;

  private:
    const std::string src_;
};
