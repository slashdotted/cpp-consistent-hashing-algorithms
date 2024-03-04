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

#include <iostream>
#include <yaml-cpp/yaml.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <algorithm> <configuration>" << std::endl;
        return 1;
    }

    std::string algo = argv[1];
    std::string config = argv[2];

    try {
        YAML::Node yaml_file = YAML::LoadFile("../configs/config.yaml");
        YAML::Node yaml_config = yaml_file["speed_test"][config];

        if (yaml_config) {
            int anchor_set = yaml_config["anchor_set"].as<int>();
            int num_keys = yaml_config["num_keys"].as<int>();
            int num_removals = yaml_config["num_removals"].as<int>();
            int working_set = yaml_config["working_set"].as<int>();

            if (algo == "anchor") {
                return bench<AnchorEngine>("AnchorEngine", "anchor_speed_test.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "dx") {
                return bench<DxEngine>("DxEngine", "dx_speed_test.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "jump") {
                return bench<JumpEngine>("JumpEngine", "jump_speed_test.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "maglev") {
                return bench<MaglevEngine>("MaglevEngine", "maglev_speed_test.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "memento") {
                return bench<MementoEngine<boost::unordered_flat_map>>("MementoEngine<boost::unordered_flat_map>", "memento.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "power") {
                return bench<PowerEngine>("PowerEngine", "power_speed_test.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "ring") {
                return bench<RingEngine>("RingEngine", "ring_speed_test.log", anchor_set, working_set, num_removals, num_keys);
            } else {
                std::cerr << "Algorithm <" << algo << "> not found." << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Configuration <" << config << "> not found." << std::endl;
            return 1;
        }
    } catch(const YAML::Exception& e) {
        std::cerr << "Error reading YAML file: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
