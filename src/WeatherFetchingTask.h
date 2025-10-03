#pragma once
#include "Executor.h"

class ISQLEngine;
class IDataParser;

class WeatherFetchingTask : public ITask
{
public:
    using Cities = std::vector<std::string>;
public:
    WeatherFetchingTask(const Cities& cities, ISQLEngine& db, IDataParser& parser, uint64_t interval);

    void Run() override;

private:
    const Cities& cities_;
    ISQLEngine& db_;
    IDataParser& parser_;
};
