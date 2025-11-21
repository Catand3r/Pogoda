#pragma once
#include "itask.h"
#include <chrono>
#include <string>

class IIniWrapper;
class ISQLEngine;
class IDataParser;

class CurrencyFetchingTask : public ITask
{
  public:
    using Currencies = std::vector<std::string>;
    using Range = std::pair<std::chrono::sys_days, std::chrono::sys_days>;

    CurrencyFetchingTask(IIniWrapper &ini, ISQLEngine &db, IDataParser &parser, uint64_t interval)
        : ini_(ini), db_(db), parser_(parser), ITask(std::chrono::milliseconds(interval))
    {
    }

    void Run() override;

    void Init() override;

  private:
    std::vector<Range> setRanges(const std::vector<std::chrono::sys_days> &result,
                                 const std::chrono::sys_days &currentTime, const std::chrono::sys_days &oldestTime);

    Range range_;
    Currencies currencies_;
    std::string rateminhistory_;

    IIniWrapper &ini_;
    ISQLEngine &db_;
    IDataParser &parser_;

    void ReadIniFile();
};
