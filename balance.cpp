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
#ifdef USE_PCG32
#include "pcg_random.hpp"
#include <random>
#endif
#include "anchor/anchorengine.h"
#include "memento/mashtable.h"
#include "memento/mementoengine.h"
#include "jump/jumpengine.h"
#include "power/powerengine.h"
#include "dx/DxEngine.h"
#include "maglev/MaglevEngine.h"
#include "ring/RingEngine.h"
#include <fmt/core.h>
#include <fstream>
#include <unordered_map>
#include <gtl/phmap.hpp>

/*
 * Benchmark routine
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

  fmt::println("Algorithm: {}, AnchorSet: {}, WorkingSet: {}, NumRemovals: {}, "
               "NumKeys: {}, ResFileName: {}",
               algorithm, anchor_set, working_set, num_removals, num_keys,
               filename);

  srand(time(NULL));

  if (algorithm == "null") {
    // do nothing
  } else if (algorithm == "baseline") {
    fmt::println("Allocating {} buckets of size {} bytes...", anchor_set,
                 sizeof(uint32_t));
    uint32_t *bucket_status = new uint32_t[anchor_set]();
    for (uint32_t i = 0; i < working_set; i++) {
      bucket_status[i] = 1;
    }
    uint32_t i = 0;
    while (i < num_removals) {
      uint32_t removed = rand() % working_set;
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
  } else if (algorithm == "dx") {
    return bench<DxEngine>("DxEngine", filename,
                          anchor_set, working_set,
                          num_removals, num_keys);
  } else if (algorithm == "maglev") {
    return bench<MaglevEngine>("MaglevEngine", filename,
                          anchor_set, working_set,
                          num_removals, num_keys);
  } else if (algorithm == "ring") {
    return bench<RingEngine>("RingEngine", filename,
                          anchor_set, working_set,
                          num_removals, num_keys); 
  } else {
    fmt::println("Unknown algorithm {}", algorithm);
    return 2;
  }
}
