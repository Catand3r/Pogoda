#include "executor.h"
#include "logger.h"
#include <functional>
#include <future>
#include <thread>

Executor &Executor::getInstance()
{
    static Executor instance;
    return instance;
}

void Executor::execute(ITask &task)
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
