#pragma once
#include <functional>
#include <future>
#include <thread>

struct Task
{
    std::function<void()> func;
    // virtual Run() = 0;
    std::chrono::steady_clock::time_point nextRun;
    std::chrono::milliseconds readPeriod;
    std::chrono::milliseconds interval;
    std::future<void> taskFuture;
};

class Executor
{
  public:
    static Executor &getInstance()
    {
        static Executor instance;
        return instance;
    }
    void execute(Task &task)
    {

        if (!task.taskFuture.valid())
        {
            task.taskFuture = std::async(std::launch::async, task.func);
        }

        auto status = task.taskFuture.wait_for(std::chrono::seconds(0));
        if (status == std::future_status::ready)
        {
            // pobierz wynik (lub obs³u¿ wyj¹tek, jeœli task rzuci³)
            try
            {
                task.taskFuture.get();
            }
            catch (const std::exception &e)
            {
                std::cerr << "Task error: " << e.what() << "\n";
            }

            // uruchom nowe zadanie
            task.taskFuture = std::async(std::launch::async, task.func);
        }
    }

  private:
    Executor() = default;
    Executor(const Executor &) = delete;
    Executor &operator=(const Executor &) = delete;
};
