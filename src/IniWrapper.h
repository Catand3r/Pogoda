#pragma once
#include "Logger.h"
#include "inicpp.h"
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

class IniWrapper : public IIniWrapper
{
  public:
    IniWrapper(const std::string &src) : IIniWrapper(src)
    {
        try
        {
            iniFile_.load(src);
            Logger::getInstance().logInfo("INI file loaded successfully: " + src);
        }
        catch (const std::exception &e)
        {
            Logger::getInstance().logError("Error loading INI file: " + std::string(e.what()));
        }
    }

    std::string getValue(const std::string &section, const std::string &key,
                         const std::string &defaultValue) const override
    {
        try
        {
            return iniFile_.at(section).at(key).as<std::string>();
        }
        catch (const std::exception &e)
        {
            Logger::getInstance().logError("Error retrieving INI value [" + section + "][" + key + "]: " + e.what());
            return defaultValue;
        }
    }

  private:
    ini::IniFile iniFile_;
};
