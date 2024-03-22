/**
 * @author Roberto Vicario
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

template <typename Algorithm>
int computeLookupTime(const std::string_view algorithm, const std::string &filename,
      uint32_t anchor_set, uint32_t working_set, uint32_t num_removals,
      uint32_t num_keys) {

#ifdef USE_HEAPSTATS
static unsigned long allocations{0};
static unsigned long deallocations{0};
static unsigned long allocated{0};
static unsigned long deallocated{0};
static unsigned long maximum{0};

auto reset_memory_stats = []() noexcept {
    allocations = 0;
    allocated = 0;
    deallocations = 0;
    deallocated = 0;
    maximum = 0;
};

auto print_memory_stats = [&](std::string_view label) noexcept {
    auto alloc{allocations};
    auto dealloc{deallocations};
    auto asize{allocated};
    auto dsize{deallocated};
    auto max{maximum};
    fmt::println("   @{}: Allocations: {}, Allocated: {}, Deallocations: {}, "
                "Deallocated: {}, Maximum: {}",
                label, alloc, asize, dealloc, dsize, max);
    allocations = alloc;
    deallocations = dealloc;
    allocated = asize;
    deallocated = dsize;
    maximum = max;
};
#endif

/*
 * ******************************************
 * Benchmark routine
 * ******************************************
 */

#ifdef USE_PCG32
pcg_extras::seed_seq_from<std::random_device> seed;
pcg32 rng{seed};
#else
srand(time(NULL));
#endif

std::ofstream results_file;
results_file.open(filename, std::ofstream::out | std::ofstream::app);

double norm_keys_rate = (double)num_keys / 1000000.0;

uint32_t *bucket_status = new uint32_t[anchor_set]();

for (uint32_t i = 0; i < working_set; i++) {
    bucket_status[i] = 1;
}

#ifdef USE_HEAPSTATS
reset_memory_stats();
// print_memory_stats("StartBenchmark");
#endif

Algorithm engine(anchor_set, working_set);

#ifdef USE_HEAPSTATS
// print_memory_stats("AfterAlgorithmInit");
#endif

uint32_t i = 0;
while (i < num_removals) {
    #ifdef USE_PCG32
    uint32_t removed = rng() % working_set;
    #else
    uint32_t removed = rand() % working_set;
    #endif
    if (bucket_status[removed] == 1) {
        engine.removeBucket(removed);
        bucket_status[removed] = 0;
        i++;
    }
}

#ifdef USE_HEAPSTATS
// print_memory_stats("AfterRemovals");
#endif

volatile int64_t bucket{0};
auto start{clock()};
for (uint32_t i = 0; i < num_keys; ++i) {
    #ifdef USE_PCG32
    bucket = engine.getBucketCRC32c(rng(), rng());
    #else
    bucket = engine.getBucketCRC32c(rand(), rand());
    #endif
}
auto end{clock()};

#ifdef USE_HEAPSTATS
// print_memory_stats("EndBenchmark");
auto elapsed{static_cast<double>(end - start) / CLOCKS_PER_SEC};
auto maxheap{maximum};
// fmt::println("{} Elapsed time is {} seconds, maximum heap allocated memory is {} bytes, sizeof({}) is {}", name, elapsed, maxheap, name, sizeof(Algorithm));

    cout << "# [LOG] ----- @" << algorithm << "\t>_ lookup_time    = " << elapsed << " seconds" << endl;

    results_file << algorithm << ":\tAnchor\t" << anchor_set << "\tWorking\t"
            << working_set << "\tRemovals\t" << num_removals << "\tRate\t"
            << norm_keys_rate / elapsed << "\tMaxHeap\t" << maxheap << "\tAlgoSizeof\t" << sizeof(Algorithm)<< "\n";
#else
auto elapsed{static_cast<double>(end - start) / CLOCKS_PER_SEC};
fmt::println("{} Elapsed time is {} seconds", name, elapsed);
results_file << name << ":\tAnchor\t" << anchor_set << "\tWorking\t"
            << working_set << "\tRemovals\t" << num_removals << "\tRate\t"
            << norm_keys_rate / elapsed << "\n";
#endif

results_file.close();

delete[] bucket_status;

return 0;
}
