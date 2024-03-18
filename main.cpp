/**
 * @author Roberto Vicario (C) 2024
 */

#include <iostream>
#include <yaml-cpp/yaml.h>

#include "anchor/anchorengine.h"
#include "benchmarks/Benchmark.h"
#include "dx/DxEngine.h"
#include "jump/jumpengine.h"
#include "memento/mementoengine.h"
#include "power/powerengine.h"

using namespace std;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-branch-clone"
int main(int argc, char* argv[]) {
    try {
        /**
         * Handling terminal usage.
        */
        if (argc != 2) {
            argv[1] = "default.yaml";
        }

        /**
         * Loading YAML file.
        */
        string filename = "../configs/" + string(argv[1]);
        YAML::Node config = YAML::LoadFile(filename);

        /**
         * Starting benchmark routine.
         */
        cout << "# [SYS] ----- ****************************" << endl;
        cout << "# [SYS] ----- ***** STARTING ROUTINE *****" << endl;
        cout << "# [SYS] ----- ****************************" << endl;
        cout << "#" << endl;

        // for (const auto &iter_1 : config["common"]["init-nodes"]) {
            /**
             * Running each number of node for every engine.
             */
            auto num = 1000000; // iter_1.as<int>();
            for (const auto &iter_2: config["algorithms"]) {
                auto algorithm = iter_2["name"].as<string>();
                if (algorithm == "anchor") {
                    /**
                     * ANCHOR
                     */
                    Benchmark::execute<AnchorEngine>(config, "anchor", 1000000, num, 20000, 1000000);
                } else if (algorithm == "dx") {
                    /**
                     * DX
                     */
                    Benchmark::execute<DxEngine>(config, "dx", 1000000, num, 20000, 1000000);
                } else if (algorithm == "jump") {
                    /**
                     * JUMP
                     */
                    Benchmark::execute<JumpEngine>(config, "jump", 1000000, num, 20000, 1000000);
                } else if (algorithm == "maglev") {
                    /**
                     * MAGLEV
                     */
                    /* code */
                } else if (algorithm == "memento") {
                    /**
                     * MEMENTO
                     */
                    Benchmark::execute<MementoEngine<boost::unordered_flat_map>>(config, "memento", 1000000, num, 20000, 1000000);
                } else if (algorithm == "multi-probe") {
                    /**
                     * MULTI-PROBE
                     */
                    /* code */
                } else if (algorithm == "power") {
                    /**
                     * POWER
                     */
                    Benchmark::execute<PowerEngine>(config, "power", 1000000, num, 20000, 1000000);
                } else if (algorithm == "rendezvous") {
                    /**
                     * RENDEZVOUS
                     */
                    /* code */
                } else if (algorithm == "ring") {
                    /**
                     * RING
                     */
                }
            }
        // }
    } catch (const YAML::Exception& e) {
        cerr << "# [ERR] ----- YAML Exception: " << e.what() << endl;
        return 1;
    } catch (const exception& e) {
        cerr << "# [ERR] ----- Exception: " << e.what() << endl;
        return 1;
    }

    /**
     * Closing benchmark routine.
     */
    cout << "# [SYS] ----- *****************************" << endl;
    cout << "# [SYS] ----- ***** ROUTINE COMPLETED *****" << endl;
    cout << "# [SYS] ----- *****************************" << endl;

    return 0;
}
#pragma clang diagnostic pop
