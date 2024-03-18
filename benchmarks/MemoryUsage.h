/**
 * @author Roberto Vicario (C) 2024
 */

#ifndef CPP_CONSISTENT_HASHING_MEMORYUSAGE_H
#define CPP_CONSISTENT_HASHING_MEMORYUSAGE_H

#include <iostream>
using namespace std;

class MemoryUsage {
public:
    template <typename Algorithm>
    static int bench(string algorithm, uint32_t anchor_set, uint32_t working_set) {
        Algorithm obj(anchor_set, working_set);
        size_t size = sizeof(obj);
        cout << "# [LOG] ----- " << "@" << algorithm << " >_ @memory_usage = " << size << " bytes" << endl;

        return 0;
    }
};

#endif // CPP_CONSISTENT_HASHING_MEMORYUSAGE_H
