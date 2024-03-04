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
#include "anchor/anchorengine.h"
#include "memento/mashtable.h"
#include "memento/mementoengine.h"
#include "jump/jumpengine.h"
#include "power/powerengine.h"
#include "dx/DxEngine.h"
#include "maglev/MaglevEngine.h"
#include "ring/RingEngine.h"
#ifdef USE_PCG32
#include "pcg_random.hpp"
#include <random>
#endif
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered_map.hpp>
#include <cxxopts.hpp>
#include <fmt/core.h>
#include <fstream>
#include <unordered_map>
#include <gtl/phmap.hpp>

/*
 * ******************************************
 * Heap allocation measurement
 * ******************************************
 */

#ifdef USE_HEAPSTATS
static unsigned long allocations{0};
static unsigned long deallocations{0};
static unsigned long allocated{0};
static unsigned long deallocated{0};
static unsigned long maximum{0};

void *operator new(size_t size) {
  void *p = malloc(size);
  allocations += 1;
  allocated += size;
  maximum = allocated > maximum ? allocated : maximum;
  return p;
}

void *operator new[](size_t size) {
  void *p = malloc(size);
  allocations += 1;
  allocated += size;
  maximum = allocated > maximum ? allocated : maximum;
  return p;
}

void operator delete(void *ptr, std::size_t size) noexcept {
  deallocations += 1;
  deallocated += size;
  free(ptr);
}

void operator delete[](void *ptr, std::size_t size) noexcept {
  deallocations += 1;
  deallocated += size;
  free(ptr);
}

void reset_memory_stats() noexcept {
  allocations = 0;
  allocated = 0;
  deallocations = 0;
  deallocated = 0;
  maximum = 0;
}

void print_memory_stats(std::string_view label) noexcept {
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
}
#endif

/*
 * ******************************************
 * Benchmark routine
 * ******************************************
 */
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

  std::ofstream results_file;
  results_file.open(filename, std::ofstream::out | std::ofstream::app);

  double norm_keys_rate = (double)num_keys / 1000000.0;

  uint32_t *bucket_status = new uint32_t[anchor_set]();

  for (uint32_t i = 0; i < working_set; i++) {
      bucket_status[i] = 1;
  }

#ifdef USE_HEAPSTATS
  reset_memory_stats();
  print_memory_stats("StartBenchmark");
#endif

  Algorithm engine(anchor_set, working_set);

#ifdef USE_HEAPSTATS
  print_memory_stats("AfterAlgorithmInit");
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
  print_memory_stats("AfterRemovals");
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
  print_memory_stats("EndBenchmark");
#endif

  auto elapsed{static_cast<double>(end - start) / CLOCKS_PER_SEC};
#ifdef USE_HEAPSTATS
  auto maxheap{maximum};
  fmt::println("{} Elapsed time is {} seconds, maximum heap allocated memory is {} bytes, sizeof({}) is {}", name, elapsed, maxheap, name, sizeof(Algorithm));
  results_file << name << ":\tAnchor\t" << anchor_set << "\tWorking\t"
               << working_set << "\tRemovals\t" << num_removals << "\tRate\t"
               << norm_keys_rate / elapsed << "\tMaxHeap\t" << maxheap << "\tAlgoSizeof\t" << sizeof(Algorithm)<< "\n";
#else
  fmt::println("{} Elapsed time is {} seconds", name, elapsed);
  results_file << name << ":\tAnchor\t" << anchor_set << "\tWorking\t"
               << working_set << "\tRemovals\t" << num_removals << "\tRate\t"
               << norm_keys_rate / elapsed << "\n";
#endif



  results_file.close();

  delete[] bucket_status;

  return 0;
}

int main(int argc, char *argv[]) {
  cxxopts::Options options("speed_test", "MementoHash vs AnchorHash benchmark");
  options.add_options()(
      "Algorithm",
      "Algorithm (null|baseline|anchor|memento|mementoboost|mementomash|mementostd|mementogtl|jump|power)",
      cxxopts::value<std::string>())(
      "AnchorSet", "Size of the AnchorSet (ignored by Memento)",
      cxxopts::value<int>())("WorkingSet", "Size of the WorkingSet",
                             cxxopts::value<int>())(
      "NumRemovals", "Number of random removals", cxxopts::value<int>())(
      "NumKeys", "Number of keys to lookup for",
      cxxopts::value<int>())("ResFileName", "Number of keys to lookup for",
                             cxxopts::value<std::string>());
  options.positional_help(
      "Algorithm AnchorSet WorkingSet NumRemovals Numkeys ResFilename");
  options.parse_positional({"Algorithm", "AnchorSet", "WorkingSet",
                            "NumRemovals", "NumKeys", "ResFileName"});
  auto result = options.parse(argc, argv);
  if (argc != 7) {
    fmt::println("{}", options.help());
    exit(1);
  }

  auto algorithm = result["Algorithm"].as<std::string>();
  auto anchor_set = static_cast<uint32_t>(result["AnchorSet"].as<int>());
  auto working_set = static_cast<uint32_t>(result["WorkingSet"].as<int>());
  auto num_removals = static_cast<uint32_t>(result["NumRemovals"].as<int>());
  auto num_keys = static_cast<uint32_t>(result["NumKeys"].as<int>());
  auto filename = result["ResFileName"].as<std::string>();

#ifdef USE_PCG32
  fmt::println("Algorithm: {}, AnchorSet: {}, WorkingSet: {}, NumRemovals: {}, "
               "NumKeys: {}, ResFileName: {}, Random: PCG32",
               algorithm, anchor_set, working_set, num_removals, num_keys,
               filename);
#else
  fmt::println("Algorithm: {}, AnchorSet: {}, WorkingSet: {}, NumRemovals: {}, "
               "NumKeys: {}, ResFileName: {}, Random: rand()",
               algorithm, anchor_set, working_set, num_removals, num_keys,
               filename);
#endif

  if (algorithm == "null") {
    // do nothing
  } else if (algorithm == "baseline") {
#ifdef USE_PCG32
    pcg_extras::seed_seq_from<std::random_device> seed;
    pcg32 rng(seed);
#else
    srand(time(NULL));
#endif
    fmt::println("Allocating {} buckets of size {} bytes...", anchor_set,
                 sizeof(uint32_t));
    uint32_t *bucket_status = new uint32_t[anchor_set]();
    for (uint32_t i = 0; i < working_set; i++) {
      bucket_status[i] = 1;
    }
    uint32_t i = 0;
    while (i < num_removals) {
#ifdef USE_PCG32
      uint32_t removed = rng() % working_set;
#else
        uint32_t removed = rand() % working_set;
#endif
      if (bucket_status[removed] == 1) {
        bucket_status[removed] = 0;
        i++;
      }
    }
    delete[] bucket_status;
  } else if (algorithm == "anchor") {
    return bench<AnchorEngine>("Anchor", filename, anchor_set, working_set,
                               num_removals, num_keys);
  } else if (algorithm == "memento") {
    return bench<MementoEngine<boost::unordered_flat_map>>(
        "Memento<boost::unordered_flat_map>", filename, anchor_set, working_set,
        num_removals, num_keys);
  } else if (algorithm == "mementoboost") {
    return bench<MementoEngine<boost::unordered_map>>(
        "Memento<boost::unordered_map>", filename, anchor_set, working_set,
        num_removals, num_keys);
  } else if (algorithm == "mementostd") {
    return bench<MementoEngine<std::unordered_map>>(
        "Memento<std::unordered_map>", filename, anchor_set, working_set,
        num_removals, num_keys);
  } else if (algorithm == "mementogtl") {
      return bench<MementoEngine<gtl::flat_hash_map>>(
          "Memento<std::gtl::flat_hash_map>", filename, anchor_set, working_set,
          num_removals, num_keys);
  } else if (algorithm == "mementomash") {
    return bench<MementoEngine<MashTable>>("Memento<MashTable>", filename,
                                           anchor_set, working_set,
                                           num_removals, num_keys);
  } else if (algorithm == "jump") {
      return bench<JumpEngine>("JumpEngine", filename,
                                             anchor_set, working_set,
                                             num_removals, num_keys);
  } else if (algorithm == "power") {
      return bench<PowerEngine>("PowerEngine", filename,
                               anchor_set, working_set,
                               num_removals, num_keys);
  } else {
    fmt::println("Unknown algorithm {}", algorithm);
    return 2;
  }
}