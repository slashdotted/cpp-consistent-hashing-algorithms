/**
 * @author Amos Brocco
 * @author Roberto Vicario
 * 
 * @copyright (c) 2023 Amos Brocco.
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
#include <filesystem>
#include <fmt/core.h>
#include <iostream>
#include <yaml-cpp/yaml.h>

using namespace std;

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            cerr << "Usage: " << argv[0] << " <your_configuration>.yaml" << endl;
            return 1;
        }

        /**
         * YAML configuration
        */
        string filename = "../configs/" + string(argv[1]);
        YAML::Node config = YAML::LoadFile(filename);

        /**
         * Here starts the benchmark routine
        */
        cout << "#############################\n##### BENCHMARK ROUTINE #####\n#############################\n" << endl;

        for (const auto& algorithm : config["algorithms"]) {
            string name = algorithm["name"].as<string>();
            if (name == "anchor") {
                Benchmark::runBenchmarkRoutine<AnchorEngine>(name, 1000000, 1000000, 20000, 1000000);
            } else if (name == "dx") {
                Benchmark::runBenchmarkRoutine<DxEngine>(name, 1000000, 1000000, 20000, 1000000);
            } else if (name == "jump") {
                /* code */
            } else if (name == "maglev") {
                /* code */
            } else if (name == "memento") {
                /* code */
            } else if (name == "multi-probe") {
                /* code */
            } else if (name == "power") {
                /* code */
            } else if (name == "rendezvous") {
                /* code */
            } else if (name == "ring") {
                /* code */
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
