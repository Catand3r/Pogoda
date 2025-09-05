#pragma once
#include "Executor.h"
#include <mutex>
#include <string>
#include <vector>

/*
struct WeatcherFetchingTask : public Task
{
WeatcherFetchingTask( Citites cities) : cities_(citites)
    Run() override
    {
        for (const auto &city : cities_)
        {
            Logger::getInstance().log("Fetching weather for city: " + city);
            auto response = HttpClient::getInstance().get("https://wttr.in/" + city + "?format=j1");

            if (!outFile_.is_open())
            {
                Logger::getInstance().log("Failed to open output file");
                return;
            }

            json_ = nlohmann::json::parse(response);

                    std::string city = json_["nearest_area"][0]["areaName"][0]["value"].get<std::string>();
                    std::string desc = json_["current_condition"][0]["weatherDesc"][0]["value"].get<std::string>();
                    std::string temp = json_["current_condition"][0]["tempC"].get<std::string>();
                    std::string feels = json_["current_condition"][0]["FeelsLikeC"].get<std::string>();
                    std::string humidity = json_["current_condition"][0]["humidity"].get<std::string>();
                    std::string wind = json_["current_condition"][0]["windspeedKmph"].get<std::string>();

                    std::ostringstream oss;
                    oss << "INSERT INTO Pogoda(city, description, temperature, humidity, wind) VALUES("
                        << "'" << city << "', "
                        << "'" << desc << "', "
                        << "'" << temp << " (" << feels << ")" << "', "
                        << "'" << humidity << "', "
                        << "'" << wind << "');";

                    std::string query = oss.str();

                    SQLiteDB::QueryResult queryResult;

                    if (!db_.exec(query, queryResult))
                    {
                        Logger::getInstance().log("Failed to execute query: " + query);
                        continue;
                    }
                }
            }
            catch (const std::exception &e)
            {
                Logger::getInstance().log("Error fetching weather data: " + std::string(e.what()));
                return;
            }
    }

    Citites cities_
}

struct LoggerTask : public Task
{
void Run() override
{
Logger::getInstance().Write();
}
}
*/

class IScheduler
{
  public:
    virtual ~IScheduler() = default;
    virtual void addTask(const std::function<void()> &task, const int &) = 0;
    virtual void run() = 0;
    virtual void stop() = 0;
};

class Scheduler : public IScheduler
{
  private:
    Scheduler() = default;
    Scheduler(const Scheduler &) = delete;
    Scheduler &operator=(const Scheduler &) = delete;

    std::vector<Task> tasks_;

    bool running_ = true;

    std::mutex mtx_;

  public:
    static Scheduler &getInstance()
    {
        static Scheduler instance;
        return instance;
    }

    void addTask(const std::function<void()> &task, const int &readPeriodInt) override
    {
        std::chrono::milliseconds readPeriod(readPeriodInt * 1000);
        std::lock_guard<std::mutex> lock(mtx_);
        tasks_.push_back({task, std::chrono::steady_clock::now() + readPeriod, readPeriod});
    }

    void run() override
    {
        while (running_)
        {
            auto now = std::chrono::steady_clock::now();
            std::lock_guard<std::mutex> lock(mtx_);
            for (auto &task : tasks_)
            {
                if (now >= task.nextRun)
                {
                    Executor::getInstance().execute(task);
                    // tutaj przekazaæ ca³y task
                    task.nextRun = now + task.interval;
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
