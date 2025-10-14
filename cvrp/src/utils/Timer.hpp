//
// Created by Antek on 14.10.2025.
//

#ifndef ALGORYTMY_OPTYMALIZACJI_TIMER_H
#define ALGORYTMY_OPTYMALIZACJI_TIMER_H
#include "chrono"

using namespace std::chrono;

class Timer {
public:
    void start();
    void stop();
    long long  nano();
    long long  micro();
    long long  mili();
private:
    high_resolution_clock::time_point beginning;
    high_resolution_clock::time_point end;
};

#endif //ALGORYTMY_OPTYMALIZACJI_TIMER_H
