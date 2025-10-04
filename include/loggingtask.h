#pragma once
#include "itask.h"

class LoggingTask : public ITask
{
  public:
    LoggingTask(uint64_t interval) : ITask(std::chrono::milliseconds(interval))
    {
    }

    void Run() override;
};
