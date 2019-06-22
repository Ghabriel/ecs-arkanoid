#pragma once

#include <chrono>
#include <functional>

struct TimedEvent {
    std::chrono::system_clock::time_point when;
    std::function<void()> fn;
};

template<typename F>
TimedEvent createEvent(int delayMs, F fn) {
    auto now = std::chrono::system_clock::now();
    auto when = now + std::chrono::milliseconds(delayMs);

    return TimedEvent { when, fn };
}
