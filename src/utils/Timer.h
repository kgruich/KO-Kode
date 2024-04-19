#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer
{
public:
    bool isRunning = false;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;

    void start() {
        startTime = std::chrono::high_resolution_clock::now();
        isRunning = true;
    }

    void stop() {
        endTime = std::chrono::high_resolution_clock::now();
        isRunning = false;
    }

    template <typename T>
    [[nodiscard]] auto lapsed() const {
        return std::chrono::duration_cast<T>(endTime - startTime).count();
    }

    friend std::ostream& operator<<(std::ostream& os, const Timer& t);
};

inline std::ostream& operator<<(std::ostream& os, const Timer& t) {
    if (t.isRunning) {
        os << "\t" << "Timer is still running\n";
    }
    else {
        os << "\t" << "time spent: " << t.lapsed<std::chrono::milliseconds>() << " ms\n";
    }
    return os;
}

#endif