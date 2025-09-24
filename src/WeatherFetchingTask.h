#pragma once
#include "Executor.h"

class ISQLEngine;

class WeatherFetchingTask : public ITask
{
public:
    using Cities = std::vector<std::string>;
public:
    WeatherFetchingTask(const Cities& cities, ISQLEngine& db, uint64_t interval);

    void Run() override;

private:
    const Cities& cities_;
    ISQLEngine& db_;
};
