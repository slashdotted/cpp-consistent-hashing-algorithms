/**
 * @author Roberto Vicario (C) 2024
 */

#pragma once

#include <iostream>
using namespace std;

template <typename Algorithm>
int computeMemoryUsage(string algorithm, uint32_t anchor_set, uint32_t working_set) {
    Algorithm obj(anchor_set, working_set);
    size_t size = sizeof(obj);
    cout << "# [LOG] ----- @" << algorithm << "\t>_ memory_usage   = " << size << " bytes" << endl;

    return 0;
}
