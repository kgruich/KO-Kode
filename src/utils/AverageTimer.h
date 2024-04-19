#ifndef AVERAGETIMER_H
#define AVERAGETIMER_H

#include <chrono>

class AverageTimer {
public:
    bool firstRun = true;
    bool isRunning = false;
    std::chrono::time_point<std::chrono::high_resolution_clock> firstTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
    std::chrono::duration<double> totalTime;
    int runs = 0;

    void start() {
        if (firstRun) {
            firstTime = std::chrono::high_resolution_clock::now();
            firstRun = false;
        }
        startTime = std::chrono::high_resolution_clock::now();
        isRunning = true;
    }

    void stop() {
        endTime = std::chrono::high_resolution_clock::now();
        totalTime += endTime - startTime;
        runs++;
        isRunning = false;
    }

    template <typename T>
    [[nodiscard]] auto lapsedCurrent() const {
        return std::chrono::duration_cast<T>(endTime - startTime).count();
    }

    template <typename T>
    [[nodiscard]] auto lapsedTotal() const {
        return std::chrono::duration_cast<T>(endTime - firstTime).count();
    }

    template <typename T>
    [[nodiscard]] auto averageTime() const {
        return std::chrono::duration_cast<T>(totalTime).count() / static_cast<double>(runs);
    }

    friend std::ostream& operator<<(std::ostream& os, const AverageTimer& t);
};

inline std::ostream& operator<<(std::ostream& os, const AverageTimer& t) {
    if (t.isRunning) {
        os << "\t" << "Timer is still running\n";
    }
    else if (t.runs == 0) {
        os << "\t" << "time spent: " << t.lapsedCurrent<std::chrono::milliseconds>() << " ms\n";
    }
    else {
        os << "\t" << "total time spent: " << t.lapsedTotal<std::chrono::milliseconds>() << " ms\n";
        os << "\t" << "total runs: " << t.runs << "\n";
        os << "\t" << "average time: " << t.averageTime<std::chrono::milliseconds>() << " ms\n";
    }
    return os;
}

#endif