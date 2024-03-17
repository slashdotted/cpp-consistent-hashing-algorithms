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
#include "benchmarks/Benchmark.h"
#include "dx/DxEngine.h"
#include <iostream>
#include "jump/jumpengine.h"
#include "memento/mementoengine.h"
#include <yaml-cpp/yaml.h>
using namespace std;

/**
 * Main function.
 *
 * @param argc number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return Integer indicating the execution status.
 */
int main(int argc, char* argv[]) {
    try {
        /**
         * Checking terminal usage.
        */
        if (argc != 2) {
            cerr << "Usage: " << argv[0] << " <your_configuration>.yaml" << endl;
            return 1;
        }

        /**
         * Loading YAML file.
        */
        string filename = "../configs/" + string(argv[1]);
        YAML::Node config = YAML::LoadFile(filename);

        /**
         * Running benchmark routine.
         */
        cout << "#######################################" << endl;
        cout << "########## BENCHMARK ROUTINE ##########" << endl;
        cout << "#######################################\n#\n#" << endl;

        /**
         * Starting benchmark routine.
         */
        for (const auto &iter_1 : config["common"]["init-nodes"]) {
            /**
             * Running each number of node for every engine.
             */
            auto num = iter_1.as<int>();
            for (const auto &iter_2: config["algorithms"]) {
                auto algorithm = iter_2["name"].as<string>();
                cout << "##############################" << endl;
                cout << "########## NEW ALGO ##########" << endl;
                cout << "##############################\n#\n#" << endl;

                if (algorithm == "anchor") {
                    // Benchmark::execute<AnchorEngine>(config, "anchor", 1000000, 1000000, 20000, 1000000);
                } else if (algorithm == "dx") {
                    Benchmark::execute<DxEngine>(config, "dx", 1000000, 1000000, 20000, 1000000);
                } else if (algorithm == "jump") {
                    // Benchmark::execute<JumpEngine>(config, "jump", 1000000, 1000000, 20000, 1000000);
                } else if (algorithm == "maglev") {
                    /* code */
                } else if (algorithm == "memento") {
                    // Benchmark::execute<MementoEngine<boost::unordered_flat_map>>(config, "memento", 1000000, 1000000, 20000, 1000000);
                } else if (algorithm == "multi-probe") {
                    /* code */
                } else if (algorithm == "power") {
                    // Benchmark::execute<PowerEngine>(config, "power", 1000000, 1000000, 20000, 1000000);
                } else if (algorithm == "rendezvous") {
                    /* code */
                } else if (algorithm == "ring") {
                    /* code */
                }

                cout << "##############################" << endl;
                cout << "########## END ALGO ##########" << endl;
                cout << "##############################\n#\n#" << endl;
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
