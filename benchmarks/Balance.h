#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <string_view>
#include <vector>
#include <ctime>
#include <fmt/core.h>

using namespace std;

#include "pcg_random.hpp" // Assuming this file exists

// Replace with appropriate includes for unordered_flat_map and cxxopts
// #include <boost/unordered/unordered_flat_map.hpp>
// #include <cxxopts.hpp>


template <typename Algorithm>
int computeBalance(const std::string_view algorithm, const std::string& filename,
                 uint32_t anchor_set, uint32_t working_set,
                 uint32_t num_removals, uint32_t num_keys) {
    std::random_device rd;
    std::mt19937 rng(rd());

    Algorithm engine(anchor_set, working_set);

    std::vector<uint32_t> anchor_absorbed_keys(anchor_set, 0);
    std::vector<uint32_t> bucket_status(working_set, 1);

    uint32_t i = 0;
    while (i < num_removals) {
        uint32_t removed = rng() % working_set;
        if (bucket_status[removed] == 1) {
            auto rnode = engine.removeBucket(removed);
            bucket_status[rnode] = 0;
            i++;
        }
    }

    std::ofstream results_file(filename, std::ofstream::out | std::ofstream::app);
    if (!results_file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return -1;
    }

    for (uint32_t i = 0; i < num_keys; ++i) {
        anchor_absorbed_keys[engine.getBucketCRC32c(rng(), rng())] += 1;
    }

    double mean = static_cast<double>(num_keys) / (working_set - num_removals);
    double lb = 0;
    for (uint32_t i = 0; i < anchor_set; i++) {
        if (bucket_status[i]) {
            double ratio = anchor_absorbed_keys[i] / mean;
            lb = std::max(lb, ratio);
        } else {
            if (anchor_absorbed_keys[i] > 0) {
                fmt::print("{}: crazy bug!\n", algorithm);
            }
        }
    }

    cout << "# [LOG] ----- @" << algorithm << "\t>_ balance        = " << lb << endl;

    // fmt::print("{}: LB is {}\n", name, lb);
    results_file << algorithm << ": Balance: " << lb << "\t" << "PCG32" << std::endl;

    results_file.close();

    return 0;
}
