/**
 * @author Roberto Vicario (C) 2024
 */

#pragma once

#include <chrono>
#include <iostream>

using namespace chrono;
using namespace std;

class InitTime {
public:
    template <typename Algorithm>
    static int bench(string algorithm, uint32_t anchor_set, uint32_t working_set) {
        auto start = high_resolution_clock::now();
        Algorithm obj(anchor_set, working_set);
        auto end = high_resolution_clock::now();

        auto elapsed = duration_cast<nanoseconds>(end - start);
        long long time = elapsed.count();
        cout << "# [LOG] ----- " << "@" << algorithm << "\t\t>_ init_time      =\t" << time << " nanoseconds" << endl;

        return 0;
    }
};
