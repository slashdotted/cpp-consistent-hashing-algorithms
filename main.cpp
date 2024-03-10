/**
 * @author Amos Brocco
 * @author Roberto Vicario
 * @date 2024
*/

#include "ring/RingEngine.h"
#include "dx/DxEngine.h"
#include "benchmark/Benchmark.cpp"

#include <filesystem>
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
            if (name == "dx") {
                Benchmark::bench<DxEngine>(name, "balance.log", 1000000, 1000000, 20000, 1000000);
            } else if (name == "ring") {
                Benchmark::bench<RingEngine>(name, "balance.log", 1000000, 1000000, 20000, 1000000);
            }
        }


            /*
            - name: anchor
            - name: dx
            - name: jump
            - name: maglev
            - name: memento
            - name: multi-probe
            - name: power
            - name: rendezvous
            - name: ring
            */
    } catch (const YAML::Exception& e)
    {
        cerr << "YAML Exception: " << e.what() << endl;
        return 1;
    } catch (const exception& e)
    {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}
