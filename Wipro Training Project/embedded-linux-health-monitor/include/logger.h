#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    RECOVERY
};

class Logger {
public:
    static Logger& getInstance();
    void init(const std::string& filename);
    void log(LogLevel level, const std::string& message);

private:
    Logger() = default;
    ~Logger();
    std::ofstream log_file_;
    std::mutex mutex_;
    std::string getTimestamp();
    std::string levelToString(LogLevel level);
};

#endif // LOGGER_H
