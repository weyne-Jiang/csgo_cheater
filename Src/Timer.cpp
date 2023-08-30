//
// Created by weyne-Jiang on 2023/8/2.
//
#include "Timer.h"

void Timer::sleep(uint64_t wholeTime, TimeAccuracy timeAccuracy) noexcept {
    Timer clock;
    clock.start();
    clock.blockThread(wholeTime, timeAccuracy);
}

void Timer::start() noexcept
{
    if (_isActive)
        return;
    _isActive = true;
    _lastStartTime = std::chrono::high_resolution_clock::now();
}

void Timer::pause() noexcept
{
    if (!_isActive)
        return;
    _isActive = false;
    _totalNano += std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - _lastStartTime).count();
}

void Timer::blockThread(uint64_t wholeTime, TimeAccuracy timeAccuracy) const noexcept
{
    uint64_t wholeNano = wholeTime * static_cast<uint64_t>(pow(1000, 3 - (int)timeAccuracy));
    auto current = std::chrono::high_resolution_clock::now();
    while (true)
    {
        if (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - current).count() > wholeNano)
            return;
    }
}

void Timer::reset() noexcept
{
    _isActive = false;
    _lastStartTime = std::chrono::high_resolution_clock::now();
    _totalNano = 0;
}

void Timer::restart() noexcept {
    _isActive = true;
    _lastStartTime = std::chrono::high_resolution_clock::now();
    _totalNano = 0;
}

bool Timer::isActive() const noexcept {
    return _isActive;
}

uint64_t Timer::totalTime(TimeAccuracy timeAccuracy) const noexcept
{
    uint32_t accfactor = static_cast<uint64_t>(pow(1000, 3 - (int)timeAccuracy));
    if (_isActive)
        return (_totalNano + std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - _lastStartTime).count()) / accfactor;
    return _totalNano / accfactor;
}
