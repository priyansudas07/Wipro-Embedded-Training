#ifndef VDEVPULSE_LOGGER_HPP
#define VDEVPULSE_LOGGER_HPP

#include <string>
#include <fstream>
#include <mutex>

enum class LogLevel {
    INFO,
    SUCCESS,
    WARNING,
    ERROR,
    DEVICE
};

class Logger {
public:
    static Logger& getInstance();
    void init(const std::string& log_path = "");
    void log(LogLevel level, const std::string& message);

private:
    Logger() = default;
    ~Logger();

    std::ofstream log_file_;
    std::mutex mutex_;
};

#endif // VDEVPULSE_LOGGER_HPP
