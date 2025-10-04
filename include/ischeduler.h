#pragma once
#include "itask.h"
#include <memory>
#include <vector>

class IScheduler
{
  public:
    using Tasks = std::vector<std::unique_ptr<ITask>>;

  public:
    virtual ~IScheduler() = default;
    virtual void addTask(std::unique_ptr<ITask> task) = 0;
    virtual void run() = 0;
    virtual void stop() = 0;
};
