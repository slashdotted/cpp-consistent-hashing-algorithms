/**
 * @author Roberto Vicario (C) 2024
*/

#include <ctime>
#include <iostream>
#include <yaml-cpp/yaml.h>

#include "Balance.h"
#include "InitTime.h"
#include "MemoryUsage.h"
#include "Monotonicity.h"
#include "SpeedTest.h"

using namespace std;

class Benchmark {
public:
    /**
     * BENCHMARK ROUTINE
    */
    template<typename Engine>
    static void execute(YAML::Node config, string algorithm,
        uint32_t anchor_set, uint32_t working_set,
        uint32_t num_removals, uint32_t num_keys) {
        /**
         * Starting benchmark routine.
         */
        cout << "# [LOG] ----- Parameters: (algorithm = " << algorithm << ", function = crc32, initNodes = " << working_set << ")\n#" << endl;
        for (const auto &iter: config["benchmarks"]) {
            auto benchmark = iter["name"].as<string>();
            if (benchmark == "balance") {
                /**
                 * BALANCE
                 */
                Balance::bench<Engine>(algorithm, "balance.log", anchor_set, working_set, num_removals, num_keys);
            } else if (benchmark == "init-time") {
                /**
                 * INIT_TIME
                 */
                InitTime::bench<Engine>(algorithm, anchor_set, working_set);
            } else if (benchmark == "lookup-time") {
                /**
                 * LOOKUP_TIME
                 */
                // SpeedTest::bench<Engine>(algorithm, "speed_test.log", anchor_set, working_set, num_removals, num_keys);
            } else if (benchmark == "memory-usage") {
                /**
                 * MEMORY_USAGE
                 */
                MemoryUsage::bench<Engine>(algorithm, anchor_set, working_set);
            } else if (benchmark == "monotonicity") {
                /**
                 * MONOTONICITY
                 */
                // Monotonicity::bench<Engine>(algorithm, "monotonicity.log", anchor_set, working_set, num_removals, num_keys);
            } else if (benchmark == "resize-balance") {
                /**
                 * RESIZE_BALANCE
                 */
                /* code */
            } else if (benchmark == "resize-time") {
                /**
                 * RESIZE_TIME
                 */
                /* code */
            }
        }

        /**
         * Closing single benchmark routine.
         */
        cout << "#" << endl;
    }
};
