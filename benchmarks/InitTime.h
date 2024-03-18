/**
 * @author Roberto Vicario (C) 2024
 */

#ifndef CPP_CONSISTENT_HASHING_INITTIME_H
#define CPP_CONSISTENT_HASHING_INITTIME_H

#include <chrono>
#include <iostream>
using namespace std;
using namespace chrono;

class InitTime {
public:
    template <typename Algorithm>
    static int bench(string algorithm, uint32_t anchor_set, uint32_t working_set) {
        auto start = high_resolution_clock::now();
        Algorithm obj(anchor_set, working_set);
        auto end = high_resolution_clock::now();

        auto elapsed = duration_cast<nanoseconds>(end - start);
        long long time = elapsed.count() / 1000000;
        cout << "# [LOG] ----- " << "@" << algorithm << " >_ @init_time = " << time << " milliseconds" << endl;

        return 0;
    }
};

#endif // CPP_CONSISTENT_HASHING_INITTIME_H
