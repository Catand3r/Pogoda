#pragma once
#include <sstream>
#include <string>
#include <vector>

class IIniWrapper
{
  public:
    IIniWrapper(const std::string &src) : src_(src)
    {
    }

    virtual std::string getValue(const std::string &section, const std::string &key,
                                 const std::string &defaultValue) const = 0;

    virtual std::vector<std::string> getValues(const std::string &section, const std::string &key,
                                               const std::vector<std::string> &defaultValue) const = 0;

    std::vector<std::string> split(const std::string &s, char delimiter) const
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter))
        {
            tokens.push_back(token);
        }
        return tokens;
    }

    std::string join(const std::vector<std::string> &s, char delimiter) const
    {
        std::string token;
        for (const auto &str : s)
        {
            if (!token.empty())
            {
                token += delimiter;
            }

            token += str;
        }

        return token;
    }

  private:
    const std::string src_;
};
