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
#include "dx/DxEngine.h"
#include "maglev/MaglevEngine.h"
#include "ring/RingEngine.h"
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
        YAML::Node yaml_config = yaml_file["monotonocity"][config];

        if (yaml_config) {
            int anchor_set = yaml_config["anchor_set"].as<int>();
            int num_keys = yaml_config["num_keys"].as<int>();
            int num_removals = yaml_config["num_removals"].as<int>();
            int working_set = yaml_config["working_set"].as<int>();

            if (algo == "anchor") {
                return bench<AnchorEngine>("AnchorEngine", "anchor_monotonocity.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "dx") {
                return bench<DxEngine>("DxEngine", "dx_monotonocity.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "jump") {
                return bench<JumpEngine>("JumpEngine", "jump_monotonocity.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "maglev") {
                return bench<MaglevEngine>("MaglevEngine", "maglev_monotonocity.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "memento") {
                return bench<MementoEngine<boost::unordered_flat_map>>("MementoEngine<boost::unordered_flat_map>", "memento.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "power") {
                return bench<PowerEngine>("PowerEngine", "power_monotonocity.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "ring") {
                return bench<RingEngine>("RingEngine", "ring_monotonocity.log", anchor_set, working_set, num_removals, num_keys);
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
