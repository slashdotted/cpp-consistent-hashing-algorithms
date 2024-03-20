/**
 * @author Roberto Vicario (C) 2024
 */

#include <iostream>
#include <yaml-cpp/yaml.h>

#include "algorithms/anchor/anchorengine.h"
#include "algorithms/dx/DxEngine.h"
#include "algorithms/jump/jumpengine.h"
#include "algorithms/memento/mementoengine.h"
#include "algorithms/power/powerengine.h"
#include "benchmarks/Routine.h"

using namespace std;

int main(int argc, char* argv[]) {
    try {
        /**
         * Handling terminal usage.
         */
        string configName = (argc == 2) ? argv[1] : "default.yaml";

        /**
         * Loading YAML file.
         */
        string fileName = "../configs/" + configName;
        YAML::Node yaml = YAML::LoadFile(fileName);

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
            for (const auto &iter_2: yaml["algorithms"]) {
                auto algorithm = iter_2["name"].as<string>();
                if (algorithm == "anchor") {
                    /**
                     * ANCHOR
                     */
                    execute<AnchorEngine>(yaml, "anchor", num, num, 20000, 1000000);
                } else if (algorithm == "dx") {
                    /**
                     * DX
                     */
                    execute<DxEngine>(yaml, "dx", num, num, 20000, 1000000);
                } else if (algorithm == "jump") {
                    /**
                     * JUMP
                     */
                    execute<JumpEngine>(yaml, "jump", num, num, 20000, 1000000);
                } else if (algorithm == "maglev") {
                    /**
                     * MAGLEV
                     */
                } else if (algorithm == "memento") {
                    /**
                     * MEMENTO
                     */
                    execute<MementoEngine<boost::unordered_flat_map>>(yaml, "memento", num, num, 20000, 1000000);
                } else if (algorithm == "multi-probe") {
                    /**
                     * MULTI-PROBE
                     */
                    /* code */
                } else if (algorithm == "power") {
                    /**
                     * POWER
                     */
                    execute<PowerEngine>(yaml, "power", num, num, 20000, 1000000);
                } else if (algorithm == "rendezvous") {
                    /**
                     * RENDEZVOUS
                     */
                } else if (algorithm == "ring") {
                    /**
                     * RING
                     */
                }
            // }
        }
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
