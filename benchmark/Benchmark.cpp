#include "Benchmark.h"

#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered_map.hpp>
#include <cxxopts.hpp>
#ifdef USE_PCG32
#include "pcg_random.hpp"
#include <random>
#endif

#include <fmt/core.h>
#include <fstream>
#include <unordered_map>
#include <gtl/phmap.hpp>

using namespace std;

Benchmark::Benchmark()
{}

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

  // for lb
  uint32_t *anchor_ansorbed_keys = new uint32_t[anchor_set]();

  // random removals
  uint32_t *bucket_status = new uint32_t[anchor_set]();

  for (uint32_t i = 0; i < working_set; i++) {
    bucket_status[i] = 1;
  }

  uint32_t i = 0;
  while (i < num_removals) {
#ifdef  USE_PCG32
      uint32_t removed = rng() % working_set;
#else
      uint32_t removed = rand() % working_set;
#endif
    if (bucket_status[removed] == 1) {
      auto rnode = engine.removeBucket(removed);
      bucket_status[rnode] = 0; // Remove the actually removed node
      i++;
    }
  }

  std::ofstream results_file;
  results_file.open(filename, std::ofstream::out | std::ofstream::app);

  ////////////////////////////////////////////////////////////////////
  for (uint32_t i = 0; i < num_keys; ++i) {
#ifdef USE_PCG32
    anchor_ansorbed_keys[engine.getBucketCRC32c(rng(), rng())] += 1;
#else
    anchor_ansorbed_keys[engine.getBucketCRC32c(rand(), rand())] += 1;
#endif
  }

  // check load balancing
  double mean = (double)num_keys / (working_set - num_removals);

  double lb = 0;
  for (uint32_t i = 0; i < anchor_set; i++) {

    if (bucket_status[i]) {

      if (anchor_ansorbed_keys[i] / mean > lb) {
        lb = anchor_ansorbed_keys[i] / mean;
      }

    }

    else {
      if (anchor_ansorbed_keys[i] > 0) {
        fmt::println("{}: crazy bug!", name);
      }
    }
  }


  // print lb res
#ifdef USE_PCG32
  fmt::println("{}: LB is {}\n", name, lb);
  results_file << name << ": "
               << "Balance: " << lb << "\tPCG32\n";
#else
  fmt::println("{}: LB is {}\n", name, lb);
  results_file << name << ": "
               << "Balance: " << lb << "\trand()\n";
#endif

  ////////////////////////////////////////////////////////////////////

  results_file.close();

  delete[] bucket_status;
  delete[] anchor_ansorbed_keys;

  return 0;
}

Benchmark::~Benchmark()
{}
