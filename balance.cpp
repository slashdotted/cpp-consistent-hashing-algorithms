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
        YAML::Node yaml_config = yaml_file["balance"][config];

        if (yaml_config) {
            int anchor_set = yaml_config["anchor_set"].as<int>();
            int num_keys = yaml_config["num_keys"].as<int>();
            int num_removals = yaml_config["num_removals"].as<int>();
            int working_set = yaml_config["working_set"].as<int>();

            if (algo == "anchor") {
                return bench<AnchorEngine>("AnchorEngine", "anchor_balance.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "dx") {
                return bench<DxEngine>("DxEngine", "dx_balance.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "jump") {
                return bench<JumpEngine>("JumpEngine", "jump_balance.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "maglev") {
                return bench<MaglevEngine>("MaglevEngine", "maglev_balance.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "memento") {
                return bench<MementoEngine<boost::unordered_flat_map>>("MementoEngine<boost::unordered_flat_map>", "memento.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "power") {
                return bench<PowerEngine>("PowerEngine", "power_balance.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "ring") {
                return bench<RingEngine>("RingEngine", "ring_balance.log", anchor_set, working_set, num_removals, num_keys);
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
