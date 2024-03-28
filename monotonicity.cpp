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
#include "jump/jumpengine.h"
#include "memento/mashtable.h"
#include "memento/mementoengine.h"
#include "power/powerengine.h"
#include "binomial/binomialengine.h"
#include <fmt/core.h>
#include <fstream>
#include <gtl/phmap.hpp>
#include <unordered_map>

/*
 * Benchmark routine
 */
template <typename Algorithm>
int bench(const std::string_view name, const std::string &filename,
          uint32_t anchor_set, uint32_t working_set, uint32_t num_removals,
          uint32_t num_keys) {

  Algorithm engine(anchor_set, working_set);

  // random removals
  uint32_t *bucket_status = new uint32_t[anchor_set]();

  // all nodes are working
  for (uint32_t i = 0; i < working_set; i++) {
    bucket_status[i] = 1;
  }

  // simulate num_removals removals
  uint32_t i = 0;
  while (i < num_removals) {
#ifdef USE_PCG32
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

  boost::unordered_flat_map<std::pair<uint32_t, uint32_t>, uint32_t> bucket;
  std::ofstream results_file;
  results_file.open(filename, std::ofstream::out | std::ofstream::app);

  // Determine the current key bucket assigment
  for (uint32_t i = 0; i < num_keys;) {
#ifdef USE_PCG32
    auto a{rng()};
    auto b{rng()};
#else
    auto a{rand()};
    auto b{rand()};
#endif
    if (bucket.contains({a, b}))
      continue;
    auto target = engine.getBucketCRC32c(a, b);
    bucket[{a, b}] = target;
    // Verify that we got a working bucket
    if (!bucket_status[target]) {
      throw "Crazy bug";
    }
    ++i;
  }
  fmt::println("Done determining initial assignment of {} unique keys",
               num_keys);

  // Remove a random working node
  uint32_t removed{0};
  uint32_t rnode{0};
  for (;;) {
#ifdef USE_PCG32
    removed = rng() % working_set;
#else
    removed = rand() % working_set;
#endif
    if (bucket_status[removed] == 1) {
      rnode = engine.removeBucket(removed);
      fmt::println("Removed node {}", rnode);
      if (!bucket_status[rnode]) {
        throw "Crazy bug";
      }
      bucket_status[rnode] = 0; // Remove the actually removed node
      break;
    }
  }

  uint32_t misplaced{0};
  for (const auto &i : bucket) {
    auto oldbucket = i.second;
    auto a{i.first.first};
    auto b{i.first.second};
    auto newbucket = engine.getBucketCRC32c(a, b);
    if (oldbucket != newbucket && (oldbucket != rnode)) {
      fmt::println("(After Removal) Misplaced key {},{}: before in bucket {}, "
                   "now in bucket {} (status? old bucket {}, new bucket {})",
                   a, b, oldbucket, newbucket, bucket_status[oldbucket],
                   bucket_status[newbucket]);
      ++misplaced;
    }
  }

  double m = (double)misplaced / (num_keys);
#ifdef USE_PCG32
  fmt::println(
      "{}: after removal % misplaced keys are {}% ({} keys out of {})\n", name,
      m * 100, misplaced, num_keys);
  results_file << name << ": "
               << "MisplacedRem: " << misplaced << "\t" << num_keys << "\t" << m
               << "\t" << m << "\tPCG32\n";
#else
  fmt::println("{}: after removal misplaced keys are {}% ({} keys out of {})",
               name, m * 100, misplaced, num_keys);
  results_file << name << ": "
               << "MisplacedRem: " << misplaced << "\t" << num_keys << "\t" << m
               << "\t" << m << "\trand()\n";
#endif

  misplaced = 0;
  // Add back a node
  auto anode = engine.addBucket();
  bucket_status[anode] = 1;
  fmt::println("Added node {}", anode);

  for (const auto &i : bucket) {
    auto oldbucket = i.second;
    auto a{i.first.first};
    auto b{i.first.second};
    auto newbucket = engine.getBucketCRC32c(a, b);
    if (oldbucket != newbucket) {
      fmt::println("(After Add) Misplaced key {},{}: before in bucket {}, now "
                   "in bucket {} (status? old bucket {}, new bucket {})",
                   a, b, oldbucket, newbucket, bucket_status[oldbucket],
                   bucket_status[newbucket]);
      ++misplaced;
    }
  }

  m = (double)misplaced / (num_keys);

#ifdef USE_PCG32
  fmt::println(
      "{}: after adding back % misplaced keys are {}% ({} keys out of {})\n",
      name, m * 100, misplaced, num_keys);
  results_file << name << ": "
               << "MisplacedAdd: " << misplaced << "\t" << num_keys << "\t" << m
               << "\t" << m << "\tPCG32\n";
#else
  fmt::println(
      "{}: after adding back misplaced keys are {}% ({} keys out of {})", name,
      m * 100, misplaced, num_keys);
  results_file << name << ": "
               << "MisplacedAdd: " << misplaced << "\t" << num_keys << "\t" << m
               << "\t" << m << "\trand()\n";
#endif

  ////////////////////////////////////////////////////////////////////

  results_file.close();

  delete[] bucket_status;

  return 0;
}

int main(int argc, char *argv[]) {
  cxxopts::Options options("speed_test", "MementoHash vs AnchorHash benchmark");
  options.add_options()("Algorithm",
                        "Algorithm "
                        "(null|baseline|anchor|memento|mementoboost|"
                        "mementomash|mementostd|mementogtl|jump|power)",
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
    return bench<JumpEngine>("JumpEngine", filename, anchor_set, working_set,
                             num_removals, num_keys);
  } else if (algorithm == "power") {
    return bench<PowerEngine>("PowerEngine", filename, anchor_set, working_set,
                              num_removals, num_keys);
  } else if (algorithm == "binomial") {
      return bench<BinomialEngine>("BinomialEngine", filename, anchor_set, working_set,
                                num_removals, num_keys);
  } else {
    fmt::println("Unknown algorithm {}", algorithm);
    return 2;
  }
}
