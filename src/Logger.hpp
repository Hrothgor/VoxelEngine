#pragma once

#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include "Include.hpp"

class Logger {
    public:
        enum LogType {
            WARNING,
            ERROR,
            INFO,
            SUCCESS
        };

        struct LogStruct {
            LogType type;
            std::string message;
            int hour, minute, second;
        };

        Logger();
        ~Logger();

        void Log(LogType type, const std::string message, ...);
        
        void PrintLogToFile(const std::string &filename);
        std::string FormatLog(const LogStruct &log);
        std::vector<LogStruct> GetLogs() {
            return _logs;
        }

    protected:
    private:
        std::vector<LogStruct> _logs;

// SINGLETON
    protected:
        static Logger *instance;
    public:
        Logger(Logger &other) = delete;
        void operator=(const Logger &) = delete;
        static Logger *Get() {
            if (instance == nullptr)
                instance = new Logger();
            return instance;
        };
};

#endif /* !LOGGER_HPP_ */