/**
 * @author Roberto Vicario (C) 2024
 */

#pragma once

#include <chrono>
#include <iostream>

using namespace std::chrono;
using namespace std;

template <typename Algorithm>
int computeInitTime(string algorithm, uint32_t anchor_set, uint32_t working_set) {
    auto start{clock()};
    Algorithm obj(anchor_set, working_set);
    auto end{clock()};

    auto time{static_cast<double>(end - start) / CLOCKS_PER_SEC};
    cout << "# [LOG] ----- @" << algorithm << "\t>_ init_time      = " << time << " seconds" << endl;

    return 0;
}
