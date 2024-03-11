/**
 * @author Amos Brocco
 * @author Roberto Vicario
 * 
 * @copyright Amos Brocco
 * 
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
#include "benchmarks/Benchmark.cpp"
#include "dx/DxEngine.h"
#include <iostream>
#include "jump/jumpengine.h"
// #include "maglev/MaglevEngine.h"
#include "memento/mementoengine.h"
// #include "multiprobe/MultiprobeEngine.h"
#include "power/powerengine.h"
// #include "multiprobe/MultiprobeEngine.h"
// #include "ring/RingEngine.h"
#include <yaml-cpp/yaml.h>

using namespace std;

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            cerr << "Usage: " << argv[0] << " <your_configuration>.yaml" << endl;
            return 1;
        }

        /**
         * Loading YAML file
        */
        string filename = "../configs/" + string(argv[1]);
        YAML::Node config = YAML::LoadFile(filename);

        /**
         * Running benchmark routine
         */
        cout << "#######################################" << endl;
        cout << "########## BENCHMARK ROUTINE ##########" << endl;
        cout << "#######################################" << endl;

        for (const auto& algorithm : config["algorithms"]) {
            string name = algorithm["name"].as<string>();
            if (name == "anchor") {
                Benchmark::runBenchmarkRoutine<AnchorEngine>("anchor", 1000000, 1000000, 20000, 1000000);
            } else if (name == "dx") {
                Benchmark::runBenchmarkRoutine<DxEngine>("dx", 1000000, 1000000, 20000, 1000000);
            } else if (name == "jump") {
                Benchmark::runBenchmarkRoutine<JumpEngine>("jump", 1000000, 1000000, 20000, 1000000);
            } else if (name == "maglev") {
                // Benchmark::runBenchmarkRoutine<MaglevEngine>("maglev", 1000000, 1000000, 20000, 1000000);
            } else if (name == "memento") {
                Benchmark::runBenchmarkRoutine<MementoEngine<boost::unordered_flat_map>>("memento", 1000000, 1000000, 20000, 1000000);
            } else if (name == "multi-probe") {
                // Benchmark::runBenchmarkRoutine<Multiprobe>("multi-probe", 1000000, 1000000, 20000, 1000000);
            } else if (name == "power") {
                Benchmark::runBenchmarkRoutine<PowerEngine>("power", 1000000, 1000000, 20000, 1000000);
            } else if (name == "rendezvous") {
                // Benchmark::runBenchmarkRoutine<RendezvousEngine>("rendezvous", 1000000, 1000000, 20000, 1000000);
            } else if (name == "ring") {
                // Benchmark::runBenchmarkRoutine<RingEngine>("ring", 1000000, 1000000, 20000, 1000000);
            }
        }
    } catch (const YAML::Exception& e) {
        cerr << "YAML Exception: " << e.what() << endl;
        return 1;
    } catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}
