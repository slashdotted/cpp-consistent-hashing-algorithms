/**
 * @author Roberto Vicario (C) 2024
 * @copyright Amos Brocco (C) 2024
 *
 * @brief
 */

#ifndef CPP_CONSISTENT_HASHING_MEMORYUSAGE_H
#define CPP_CONSISTENT_HASHING_MEMORYUSAGE_H

#include <iostream>
using namespace std;

class MemoryUsage {
public:
    template <typename Algorithm>
    static int bench(string name) {
        size_t size = sizeof(Algorithm);
        cout << "# [LOG] ----- " << "@" << name << " >_ @memory_usage = " << size << " Bytes" << endl;

        return 0;
    }
};

#endif // CPP_CONSISTENT_HASHING_MEMORYUSAGE_H
