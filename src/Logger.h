#pragma once
#include <chrono>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

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
    Logger()
    {
        DateTime dt = getCurrentDateTime(std::chrono::system_clock::now());

        const std::string filename =
            /*"/log/" + std::to_string(dt.year) + "/" + (dt.month < 10 ? "0" : "") + std::to_string(dt.month) + "/" +
            (dt.day < 10 ? "0" : "") + std::to_string(dt.day) + "/" + */
            /*std::to_string(dt.year) + "-" + (dt.month < 10 ? "0" : "") + std::to_string(dt.month) + "-" +
            (dt.day < 10 ? "0" : "") + std::to_string(dt.day) + "-" + (dt.hour < 10 ? "0" : "") +
            std::to_string(dt.hour) + ":" + (dt.minute < 10 ? "0" : "") + std::to_string(dt.minute) + ":" +
            (dt.second < 10 ? "0" : "") + std::to_string(dt.second) +*/
            "test.log";

        file_.open(filename, std::ios::app);

        if (!file_.is_open())
        {
            throw(std::exception("Log file failed to open"));
        }
    }
    // nie by� by to domy�lny konstruktor -> utworzy� plik o konkretnej �cie�ce
    // /log/2025/08/22/2025-08-22-18:35:02.log
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    std::mutex mtx_;
    std::ofstream file_;
    std::queue<std::string> messageQueue_;

    DateTime getCurrentDateTime(std::chrono::system_clock::time_point tp)
    {
        using namespace std::chrono;

        auto now = system_clock::now();
        year_month_day ymd = year_month_day{floor<std::chrono::days>(now)};

        auto ms_since_epoch = duration_cast<milliseconds>(tp.time_since_epoch());
        auto ms_today = ms_since_epoch % days(1); // czas od p�nocy

        auto hours = duration_cast<std::chrono::hours>(ms_today);
        auto minutes = duration_cast<std::chrono::minutes>(ms_today % std::chrono::hours(1));
        auto seconds = duration_cast<std::chrono::seconds>(ms_today % std::chrono::minutes(1));
        auto millis = duration_cast<std::chrono::milliseconds>(ms_today % std::chrono::seconds(1));

        return {int(ymd.year()),      unsigned(ymd.month()), unsigned(ymd.day()), int(hours.count()),
                int(minutes.count()), int(seconds.count()),  int(millis.count())};
    }

    void log(const std::string &message)
    {
        std::lock_guard<std::mutex> lock(mtx_);

        DateTime dt = getCurrentDateTime(std::chrono::system_clock::now());

        const std::string logEntry =
            "[" + std::to_string(dt.year) + "-" + (dt.month < 10 ? "0" : "") + std::to_string(dt.month) + "-" +
            (dt.day < 10 ? "0" : "") + std::to_string(dt.day) + " " + (dt.hour < 10 ? "0" : "") +
            std::to_string(dt.hour) + ":" + (dt.minute < 10 ? "0" : "") + std::to_string(dt.minute) + ":" +
            (dt.second < 10 ? "0" : "") + std::to_string(dt.second) + "." +
            (dt.millisecond < 100 ? (dt.millisecond < 10 ? "00" : "0") : "") + std::to_string(dt.millisecond) + "] " +
            message + "\n";

        messageQueue_.push(logEntry);
    }

  public:
    static Logger &getInstance()
    {
        static Logger instance;
        return instance;
    }

    void logWarning(const std::string &message)
    {
        log("[w]:" + message);
    }

    void logError(const std::string &message)
    {
        log("[e]:" + message);
    }

    void logInfo(const std::string &message)
    {
        log("[i]:" + message);
    }

    void Write()
    {
        std::lock_guard<std::mutex> lock(mtx_);

        std::string messages;
        while (!messageQueue_.empty())
        {
            messages += messageQueue_.front();
            messageQueue_.pop();
        }

        file_ << messages;
    }
};
