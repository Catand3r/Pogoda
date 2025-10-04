#pragma once
#include <chrono>
#include <fstream>
#include <mutex>
#include <queue>

struct DateTime
{
    int year;
    unsigned int month;
    unsigned int day;
    int hour;
    int minute;
    int second;
    int millisecond;
};

class Logger
{
  private:
    Logger();

    ~Logger();
    // nie by³ by to domyœlny konstruktor -> utworzyæ plik o konkretnej œcie¿ce
    // /log/2025/08/22/2025-08-22-18:35:02.log
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    std::mutex mtx_;
    std::ofstream file_;
    std::queue<std::string> messageQueue_;

    DateTime getCurrentDateTime(std::chrono::system_clock::time_point tp);

    void log(const std::string &message);

  public:
    static Logger &getInstance();

    void logWarning(const std::string &message);

    void logError(const std::string &message);

    void logInfo(const std::string &message);

    void Write();
};
