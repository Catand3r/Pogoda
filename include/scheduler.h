#pragma once
#include "ischeduler.h"

class Scheduler : public IScheduler
{
  private:
    Scheduler() = default;
    Scheduler(const Scheduler &) = delete;
    Scheduler &operator=(const Scheduler &) = delete;

    Tasks tasks_;

    bool running_ = true;

  public:
    static Scheduler &getInstance();

    void addTask(std::unique_ptr<ITask> task) override;

    void run() override;

    void stop() override;
};
