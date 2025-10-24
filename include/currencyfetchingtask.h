#pragma once
#include "itask.h"
#include <string>

class IIniWrapper;
class ISQLEngine;
class IDataParser;

class CurrencyFetchingTask : public ITask
{
  public:
    using Currencies = std::vector<std::string>;

    CurrencyFetchingTask(IIniWrapper &ini, ISQLEngine &db, IDataParser &parser, uint64_t interval)
        : ini_(ini), db_(db), parser_(parser), ITask(std::chrono::milliseconds(interval))
    {
    }

    void Run() override;

    void Init() override;

  private:
    Currencies currencies_;
    std::string rateminhistory_;

    IIniWrapper &ini_;
    ISQLEngine &db_;
    IDataParser &parser_;

    void ReadIniFile();
};
