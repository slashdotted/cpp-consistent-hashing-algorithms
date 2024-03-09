/*
 * Copyright (c) 2023 Amos Brocco.
 * Adapted from cpp-anchorhash Copyright (c) 2020 anchorhash (MIT License)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered_map.hpp>
#include <cxxopts.hpp>
#include <fmt/core.h>
#include <fstream>
#include <gtl/phmap.hpp>
#ifdef USE_PCG32
#include <random>
#include "pcg_random.hpp"
#endif
#include <unordered_map>

template <typename Algorithm>
int bench(const std::string_view name, const std::string &filename,
    uint32_t anchor_set, uint32_t working_set, uint32_t num_removals,
    uint32_t num_keys) {

    #ifdef USE_PCG32
        pcg_extras::seed_seq_from<std::random_device> seed;
        pcg32 rng{seed};
    #else
        srand(time(NULL));
    #endif

    Algorithm engine(anchor_set, working_set);
    uint32_t *anchor_absorbed_keys = new uint32_t[anchor_set]();
    uint32_t *bucket_status = new uint32_t[anchor_set]();

    for (uint32_t i = 0; i < working_set; i++)
        bucket_status[i] = 1;

    uint32_t i = 0;
    while (i < num_removals) {
        #ifdef  USE_PCG32
            uint32_t removed = rng() % working_set;
        #else
            uint32_t removed = rand() % working_set;
        #endif

        if (bucket_status[removed] == 1) {
            auto rnode = engine.removeBucket(removed);
            bucket_status[rnode] = 0;
            i++;
        }
    }

    std::ofstream results_file;
    results_file.open(filename, std::ofstream::out | std::ofstream::app);

    for (uint32_t i = 0; i < num_keys; ++i) {
        #ifdef USE_PCG32
            anchor_absorbed_keys[engine.getBucketCRC32c(rng(), rng())] += 1;
        #else
            anchor_absorbed_keys[engine.getBucketCRC32c(rand(), rand())] += 1;
        #endif
    }

    double mean = (double)num_keys / (working_set - num_removals);
    double lb = 0;

    for (uint32_t i = 0; i < anchor_set; i++) {
        if (bucket_status[i]) {
            if (anchor_absorbed_keys[i] / mean > lb)
                lb = anchor_absorbed_keys[i] / mean;
        }
        else {
            if (anchor_absorbed_keys[i] > 0)
                fmt::println("{}: crazy bug!", name);
        }
    }

    #ifdef USE_PCG32
        fmt::println("{}: LB is {}\n", name, lb);
        results_file << name << ": Balance: " << lb << "\tPCG32\n";
    #else
        fmt::println("{}: LB is {}\n", name, lb);
        results_file << name << ": Balance: " << lb << "\trand()\n";
    #endif

    results_file.close();

    delete[] bucket_status;
    delete[] anchor_absorbed_keys;

    return 0;
}
