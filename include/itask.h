#pragma once
#include <chrono>
#include <future>

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
