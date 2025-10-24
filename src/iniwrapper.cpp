#include "iniwrapper.h"
#include "logger.h"

IniWrapper::IniWrapper(const std::string &src) : IIniWrapper(src)
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

std::string IniWrapper::getValue(const std::string &section, const std::string &key,
                                 const std::string &defaultValue) const
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

std::vector<std::string> IniWrapper::getValues(const std::string &section, const std::string &key,
                                               const std::vector<std::string> &defaultValue) const
{
    return split(getValue(section, key, join(defaultValue, ',')), ',');
}
