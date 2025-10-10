#include "logger.h"
#include <filesystem>
#include <future>

Logger::Logger()
{
    DateTime dt = getCurrentDateTime(std::chrono::system_clock::now());

    std::ostringstream ossfilename;
    ossfilename << std::filesystem::current_path().string() << "/log/" << dt.year << "/" << std::setw(2)
                << std::setfill('0') << dt.month << "/" << std::setw(2) << std::setfill('0') << dt.day << "/";

    if (!std::filesystem::exists(ossfilename.str()))
    {
        if (!std::filesystem::create_directories(ossfilename.str()))
        {
            throw(std::exception("Failed to create log directory"));
        }
    }

    const std::string filename = ossfilename.str() + std::to_string(dt.year) + "-" + (dt.month < 10 ? "0" : "") +
                                 std::to_string(dt.month) + "-" + (dt.day < 10 ? "0" : "") + std::to_string(dt.day) +
                                 "-" + (dt.hour < 10 ? "0" : "") + std::to_string(dt.hour) + "-" +
                                 (dt.minute < 10 ? "0" : "") + std::to_string(dt.minute) + "-" +
                                 (dt.second < 10 ? "0" : "") + std::to_string(dt.second) + ".log";

    file_.open(filename, std::ios::app);

    if (!file_.is_open())
    {
        throw(std::exception("Log file failed to open"));
    }
}

Logger::~Logger()
{
    if (file_.is_open())
    {
        file_.close();
    }
}

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

DateTime Logger::getCurrentDateTime(std::chrono::system_clock::time_point tp)
{
    using namespace std::chrono;

    auto now = system_clock::now();
    year_month_day ymd = year_month_day{floor<std::chrono::days>(now)};

    auto ms_since_epoch = duration_cast<milliseconds>(tp.time_since_epoch());
    auto ms_today = ms_since_epoch % days(1); // czas od pó³nocy

    auto hours = duration_cast<std::chrono::hours>(ms_today);
    auto minutes = duration_cast<std::chrono::minutes>(ms_today % std::chrono::hours(1));
    auto seconds = duration_cast<std::chrono::seconds>(ms_today % std::chrono::minutes(1));
    auto millis = duration_cast<std::chrono::milliseconds>(ms_today % std::chrono::seconds(1));

    return {int(ymd.year()),      unsigned(ymd.month()), unsigned(ymd.day()), int(hours.count()),
            int(minutes.count()), int(seconds.count()),  int(millis.count())};
}

void Logger::log(const std::string &message)
{
    std::lock_guard<std::mutex> lock(mtx_);

    DateTime dt = getCurrentDateTime(std::chrono::system_clock::now());

    std::ostringstream oss;
    oss << "[" << dt.year << "-" << std::setw(2) << std::setfill('0') << dt.month << "-" << std::setw(2)
        << std::setfill('0') << dt.day << " " << std::setw(2) << std::setfill('0') << dt.hour << ":" << std::setw(2)
        << std::setfill('0') << dt.minute << ":" << std::setw(2) << std::setfill('0') << dt.second << "."
        << std::setw(3) << std::setfill('0') << dt.millisecond << "] "
        << "[ts="
        << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()
        << "] " << message << "\n";

    const std::string logEntry = oss.str();

    messageQueue_.push(logEntry);
}

void Logger::logWarning(const std::string &message)
{
    log("[w]:" + message);
}

void Logger::logError(const std::string &message)
{
    log("[e]:" + message);
}

void Logger::logInfo(const std::string &message)
{
    log("[i]:" + message);
}

void Logger::Write()
{
    std::lock_guard<std::mutex> lock(mtx_);

    std::string messages;
    while (!messageQueue_.empty())
    {
        messages += messageQueue_.front();
        messageQueue_.pop();
    }

    file_ << messages;
    file_.flush();
}
