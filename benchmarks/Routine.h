/**
 * @author Roberto Vicario (C) 2024
 */

#pragma once

#include <iostream>
#include <yaml-cpp/yaml.h>

#include "Balance.h"
#include "InitTime.h"
#include "MemoryUsage.h"
#include "Monotonicity.h"
#include "LookupTime.h"
#include "ResizeBalance.h"
#include "ResizeTime.h"

using namespace std;

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
    cout << "#" << endl;
    cout << "# [LOG] ----- Parameters: (algorithm = " << algorithm << ", function = crc32, initNodes = " << working_set << ")" << endl;
    cout << "#" << endl;
    cout << "#" << endl;
    
    for (const auto &iter: config["benchmarks"]) {
        auto benchmark = iter["name"].as<string>();
        if (benchmark == "balance") {
            /**
             * BALANCE
             */
            computeBalance<Engine>(algorithm, "balance.log", anchor_set, working_set, num_removals, num_keys);
        } else if (benchmark == "init-time") {
            /**
             * INIT_TIME
             */
            computeInitTime<Engine>(algorithm, anchor_set, working_set);
        } else if (benchmark == "lookup-time") {
            /**
             * LOOKUP_TIME
             */
            computeLookupTime<Engine>(algorithm, "speed_test.log", anchor_set, working_set, num_removals, num_keys);
        } else if (benchmark == "memory-usage") {
            /**
             * MEMORY_USAGE
             */
            computeMemoryUsage<Engine>(algorithm, anchor_set, working_set);
        } else if (benchmark == "monotonicity") {
            /**
             * MONOTONICITY
             */
            computeMonotonicity<Engine>(algorithm, "monotonicity.log", anchor_set, working_set, num_removals, num_keys);
        } else if (benchmark == "resize-balance") {
            /**
             * RESIZE_BALANCE
             */
            computeResizeBalance<Engine>(algorithm, anchor_set, working_set);
        } else if (benchmark == "resize-time") {
            /**
             * RESIZE_TIME
             */
            computeResizeTime<Engine>(algorithm, anchor_set, working_set);
        }
    }

    /**
     * Closing single benchmark routine.
     */
    cout << "#\n#" << endl;
}
