/**
 * @author Amos Brocco
 * @author Roberto Vicario
 * @date 2024
*/

#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered_map.hpp>
#include <cxxopts.hpp>
#include <fmt/core.h>
#include <fstream>
#include <gtl/phmap.hpp>
#ifdef USE_PCG32
#include "pcg_random.hpp"
#include <random>
#endif // USE_PCG32
#include <unordered_map>

using namespace std;

class Balance {
public:
    template <typename Algorithm>
    static int bench(const string_view name, const string &filename,
        uint32_t anchor_set, uint32_t working_set, uint32_t num_removals,
        uint32_t num_keys) {
    #ifdef USE_PCG32
        pcg_extras::seed_seq_from<random_device> seed;
        pcg32 rng{seed};
    #else
        srand(time(nullptr));
    #endif
        Algorithm engine(anchor_set, working_set);

        vector<uint32_t> anchor_absorbed_keys(anchor_set, 0);
        vector<uint32_t> bucket_status(working_set, 1);

        uint32_t i = 0;
        while (i < num_removals) {
    #ifdef USE_PCG32
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

        ofstream results_file(filename, ofstream::out | ofstream::app);

        for (uint32_t i = 0; i < num_keys; ++i) {
    #ifdef USE_PCG32
            anchor_absorbed_keys[engine.getBucketCRC32c(rng(), rng())] += 1;
    #else
            anchor_absorbed_keys[engine.getBucketCRC32c(rand(), rand())] += 1;
    #endif
        }

        double mean = static_cast<double>(num_keys) / (working_set - num_removals);
        double lb = 0;
        for (uint32_t i = 0; i < anchor_set; i++) {
            if (bucket_status[i]) {
                if (anchor_absorbed_keys[i] / mean > lb) {
                    lb = anchor_absorbed_keys[i] / mean;
                }
            } else {
                if (anchor_absorbed_keys[i] > 0) {
                    fmt::println("{}: crazy bug!", name);
                }
            }
        }

    #ifdef USE_PCG32
        fmt::println("{}: LB is {}\n", name, lb);
        results_file << name << ": "
                                << "Balance: " << lb << "\tPCG32\n";
    #else
        fmt::println("{}: LB is {}\n", name, lb);
        results_file << name << ": "
                                << "Balance: " << lb << "\trand()\n";
    #endif

        results_file.close();

        return 0;
    }
};
