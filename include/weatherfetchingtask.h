#pragma once
#include "itask.h"
#include <string>

class IIniWrapper;
class ISQLEngine;
class IDataParser;

class WeatherFetchingTask : public ITask
{
  public:
    using Cities = std::vector<std::string>;

    WeatherFetchingTask(IIniWrapper &ini, ISQLEngine &db, IDataParser &parser, uint64_t interval);

    void Run() override;

    void Init() override;

  private:
    Cities cities_;

    IIniWrapper &ini_;
    ISQLEngine &db_;
    IDataParser &parser_;

    void ReadIniFile();
};
