#include "scheduler.h"
#include "executor.h"

Scheduler &Scheduler::getInstance()
{
    static Scheduler instance;
    return instance;
}

void Scheduler::addTask(std::unique_ptr<ITask> task)
{
    tasks_.emplace_back(std::move(task));
}

void Scheduler::run()
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

void Scheduler::stop()
{
    running_ = false;
}
