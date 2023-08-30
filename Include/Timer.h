//
// Created by weyne-Jiang on 2023/8/2.
//

#ifndef CSGO_CHEATER_TIMER_H
#define CSGO_CHEATER_TIMER_H

#include "head.hpp"
#define TIMER_START(tag) auto tag##_start = std::chrono::high_resolution_clock::now(),tag##_end = tag##_start;
#define TIMER_END(tag) tag##_end = std::chrono::high_resolution_clock::now();
#define TIMER_COST_s(tag) printf("%s costs %lld s\n",#tag,std::chrono::duration_cast<std::chrono::seconds>(tag##_end - tag##_start).count());
#define TIMER_COST_ms(tag) printf("%s costs %lld ms\n",#tag,std::chrono::duration_cast<std::chrono::milliseconds>(tag##_end - tag##_start).count());
#define TIMER_COST_us(tag) printf("%s costs %lld us\n",#tag,std::chrono::duration_cast<std::chrono::microseconds>(tag##_end - tag##_start).count());
#define TIMER_COST_ns(tag) printf("%s costs %lld ns\n",#tag,std::chrono::duration_cast<std::chrono::nanoseconds>(tag##_end - tag##_start).count());

#define BENCHMARK(func, ...) \
TIMER_START(func)            \
func(__VA_ARGS__);         \
TIMER_END(func)              \
TIMER_COST_ms(func)

enum class TimeAccuracy {
    Second = 0,
    Millisecond = 1,
    Microsecond = 2
};

class Timer
{
public:
    [[maybe_unused]]
    static void sleep(uint64_t wholeTime, TimeAccuracy timeAccuracy) noexcept;

    [[maybe_unused]]
    void start() noexcept;

    [[maybe_unused]]
    void pause() noexcept;

    [[maybe_unused]]
    void blockThread(uint64_t wholeTime, TimeAccuracy timeAccuracy) const noexcept;

    [[maybe_unused]]
    void reset() noexcept;

    [[maybe_unused]]
    void restart() noexcept;

    [[nodiscard]]
    [[maybe_unused]]
    bool isActive() const noexcept;

    [[nodiscard]]
    [[maybe_unused]]
    uint64_t totalTime(TimeAccuracy timeAccuracy = TimeAccuracy::Millisecond) const noexcept;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> _lastStartTime = std::chrono::high_resolution_clock::now();
    uint64_t _totalNano = 0;
    bool _isActive = false;
};

#endif //CSGO_CHEATER_TIMER_H
