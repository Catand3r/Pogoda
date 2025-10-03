#pragma once
#include <functional>
#include <future>
#include <thread>
#include "Logger.h"

class ITask
{
public:
    ITask(std::chrono::milliseconds interval) : interval_(interval)
    {
        nextRun_ = std::chrono::steady_clock::now() + interval;
    }
    virtual ~ITask() = default;

    virtual void Run() = 0;
    bool ShouldRun()
    {
        auto now = std::chrono::steady_clock::now();
        return now >= nextRun_;
    }

    void MarkFinish()
    {
        nextRun_ = std::chrono::steady_clock::now() + interval_;
    }

private:
    std::chrono::steady_clock::time_point nextRun_;
    std::chrono::milliseconds interval_;

public:
    std::future<void> taskFuture_;
};

class Executor
{
  public:
    static Executor &getInstance()
    {
        static Executor instance;
        return instance;
    }

    void execute(ITask &task)
    {
        if (!task.taskFuture_.valid())
        {
            task.taskFuture_ = std::async(std::launch::async, [&task]() { task.Run(); });
        }

        auto status = task.taskFuture_.wait_for(std::chrono::seconds(0));
        if (status == std::future_status::ready)
        {
            task.MarkFinish();

            try
            {
                task.taskFuture_.get();
            }
            catch (const std::exception &e)
            {
                Logger::getInstance().logError("Task error: " + std::string(e.what()));
            }

            // uruchom nowe zadanie
            task.taskFuture_ = std::async(std::launch::async, [&task]() { task.Run(); });
        }
    }

  private:
    Executor() = default;
    Executor(const Executor &) = delete;
    Executor &operator=(const Executor &) = delete;
};
