#include "Profiler.hpp"
    
Profiler *Profiler::instance = nullptr;

Profiler::Profiler() {
}

Profiler::~Profiler() {
}

void Profiler::StartFrame(const std::string &name)
{
    Frame frame;

    frame.name = name;
    frame.time = glfwGetTime();

    _startTimes.push(frame);
}

void Profiler::EndFrame()
{
    auto frame = _startTimes.top();

    auto start = frame.time;
    auto end = glfwGetTime();

    _frameTimes[frame.name] = end - start;

    _startTimes.pop();
}

double Profiler::GetFrameTime(const std::string &name)
{
    if (_frameTimes.find(name) == _frameTimes.end())
        return -1.0;
    return _frameTimes[name];
}