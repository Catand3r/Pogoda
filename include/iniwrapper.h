#pragma once
#include "iiniwrapper.h"
#include "inicpp.h"

class IniWrapper : public IIniWrapper
{
  public:
    IniWrapper(const std::string &src);

    std::string getValue(const std::string &section, const std::string &key,
                         const std::string &defaultValue) const override;

  private:
    ini::IniFile iniFile_;
};
