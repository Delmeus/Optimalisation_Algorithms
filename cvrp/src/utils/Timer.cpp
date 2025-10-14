//
// Created by Antek on 14.10.2025.
//

#include "Timer.hpp"

using namespace std::chrono;

void Timer::start() {
    beginning = high_resolution_clock::now();
}

void Timer::stop() {
    end = high_resolution_clock::now();
}

long long Timer::nano() {
    return duration_cast<nanoseconds>(end - beginning).count();
}

long long Timer::micro(){
    return duration_cast<microseconds>(end - beginning).count();
}

long long Timer::mili() {
    return duration_cast<milliseconds>(end - beginning).count();
}