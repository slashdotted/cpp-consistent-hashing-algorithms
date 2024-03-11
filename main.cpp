/**
 * @author Amos Brocco
 * @author Roberto Vicario
 * @date 2024
*/

#include "anchor/anchorengine.h"
#include "benchmark/Benchmark.cpp"
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
        for (const auto& algorithm : config["algorithms"]) {
            string name = algorithm["name"].as<string>();
            if (name == "anchor") {
                Benchmark::balance<AnchorEngine>(name, "balance.log", 1000000, 1000000, 20000, 1000000);
                Benchmark::monotonicity<AnchorEngine>(name, "monotonicity.log", 1000000, 1000000, 1000, 1000000);
                Benchmark::speed_test<AnchorEngine>(name, "speed_test.log", 1000000, 1000000, 20000, 1000000);
            } else if (name == "dx") {
                Benchmark::balance<DxEngine>(name, "balance.log", 1000000, 1000000, 20000, 1000000);
                Benchmark::monotonicity<DxEngine>(name, "monotonicity.log", 1000000, 1000000, 1000, 1000000);
                Benchmark::speed_test<DxEngine>(name, "speed_test.log", 1000000, 1000000, 20000, 1000000);
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
