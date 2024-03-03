#pragma once

#ifndef PROFILER_HPP_
#define PROFILER_HPP_

#include "Include.hpp"

class Profiler {
    struct Frame {
        std::string name;
        double time;
    };
    public:
        Profiler();
        ~Profiler();

        void StartFrame(const std::string &name);
        void EndFrame();

        double GetFrameTime(const std::string &name); // in seconds

    protected:
    private:
        std::stack<Frame> _startTimes;
        std::unordered_map<std::string, double> _frameTimes;

// SINGLETON
    protected:
        static Profiler *instance;
    public:
        Profiler(Profiler &other) = delete;
        void operator=(const Profiler &) = delete;
        static Profiler *Get() {
            if (instance == nullptr)
                instance = new Profiler();
            return instance;
        };
};

#endif /* !PROFILER_HPP_ */