#include "timer.h"

using namespace std::chrono;

Timer::Timer()
{
    Last = steady_clock::now();
}

float Timer::Mark()
{
    const auto Old = Last;
    Last = steady_clock::now();
    const duration<float> FrameTime = Last - Old;
    return FrameTime.count();
}

float Timer::Peek() const
{
    return duration<float>(steady_clock::now() - Last).count();
}