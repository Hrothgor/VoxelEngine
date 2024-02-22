#include "Logger.hpp"
#include <fstream>
    
Logger *Logger::instance = nullptr;

Logger::Logger() {
}

Logger::~Logger() {
}

void Logger::PrintLogToFile(const std::string &filename) {
    std::ofstream file(filename, std::ofstream::out | std::ofstream::trunc);
    for (auto &log : _logs) {
        file << FormatLog(log) << std::endl;
    }
    file.close();
}

void Logger::Log(LogType type, const std::string message, ...) {
    va_list args;
    va_start(args, message);
    char buffer[1024];
    vsnprintf(buffer, 1024, message.c_str(), args);
    va_end(args);

    time_t now = time(nullptr);

    _logs.push_back({type, buffer, 
        localtime(&now)->tm_hour, 
        localtime(&now)->tm_min, 
        localtime(&now)->tm_sec});
}

std::string Logger::FormatLog(const LogStruct &log)
{
    std::string type;
    switch (log.type) {
        case LogType::WARNING:
            type = "WARNING";
            break;
        case LogType::ERROR:
            type = "ERROR";
            break;
        case LogType::INFO:
            type = "INFO";
            break;
        case LogType::SUCCESS:
            type = "SUCCESS";
            break;
    }
    char hour[3], minute[3], second[3];
    sprintf(hour, "%02d", log.hour);
    sprintf(minute, "%02d", log.minute);
    sprintf(second, "%02d", log.second);
    std::string formattedLog = "[" + std::string(hour) + ":" + std::string(minute) + ":" + std::string(second) + "] "
                             + type + ": " + log.message;
    return formattedLog;
}