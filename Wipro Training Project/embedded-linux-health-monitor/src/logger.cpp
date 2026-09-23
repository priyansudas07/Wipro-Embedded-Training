#include "logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::~Logger() {
    if (log_file_.is_open()) {
        log_file_.close();
    }
}

void Logger::init(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!log_file_.is_open()) {
        log_file_.open(filename, std::ios::out | std::ios::app);
    }
}

std::string Logger::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::RECOVERY: return "RECOVERY";
        default: return "UNKNOWN";
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string timestamp = getTimestamp();
    std::string level_str = levelToString(level);

    std::string formatted = "[" + timestamp + "] [" + level_str + "] " + message;
    
    std::cout << formatted << std::endl;

    if (log_file_.is_open()) {
        log_file_ << formatted << std::endl;
    }
}
