#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <mutex>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    SUCCESS,
    DEBUG
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
    bool is_initialized_ = false;
};

#endif // LOGGER_HPP
