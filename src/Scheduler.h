#pragma once
#include "Executor.h"
#include <mutex>
#include <string>
#include <vector>
#include <memory>

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

class Scheduler : public IScheduler
{
  private:
    Scheduler() = default;
    Scheduler(const Scheduler &) = delete;
    Scheduler &operator=(const Scheduler &) = delete;

    Tasks tasks_;

    bool running_ = true;

  public:
    static Scheduler &getInstance()
    {
        static Scheduler instance;
        return instance;
    }

    void addTask(std::unique_ptr<ITask> task) override
    {
        tasks_.emplace_back(std::move(task));
    }

    void run() override
    {
        while (running_)
        {
            auto now = std::chrono::steady_clock::now();
            for (auto &task : tasks_)
            {
                if (task->ShouldRun())
                {
                    Executor::getInstance().execute(*task);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // odci¹¿enie CPU
        }
    }

    void stop() override
    {
        running_ = false;
    }
};
