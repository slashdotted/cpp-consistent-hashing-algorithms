#include "Balance.h"
#include "Monotonicity.h"
#include <iostream>
#include "SpeedTest.h"
#include <yaml-cpp/yaml.h>
using namespace std;

class Benchmark {
public:
    /**
     * BENCHMARK ROUTINE
    */
    template <typename Algorithm>
    static void execute(YAML::Node config, string algorithm, uint32_t anchor_set,
        uint32_t working_set, uint32_t num_removals, uint32_t num_keys) {
        /**
         * Starting benchmark routine.
         */
        cout << "# Parameters: (algorithm = " << algorithm << ", function = crc32, initNodes = " << working_set << ")\n#\n#" << endl;
        for (const auto &iter: config["benchmarks"]) {
            /**
             * Running each benchmark for every algorithm.
             */
            auto benchmark = iter["name"].as<string>();
            cout << "# @" << algorithm << " >_ @" << benchmark << ": ...\n#\n#" << endl;

            if (benchmark == "init-time") {
                /* code */
            } else if (benchmark == "lookup-time") {
                Benchmark::lookup_time<Algorithm>(algorithm, "lookup_time.log", anchor_set, working_set, num_removals, num_keys);
            } else if (benchmark == "resize-time") {
                /* code */
            } else if (benchmark == "memory-usage") {
                /* code */
            } else if (benchmark == "balance") {
                Benchmark::balance<Algorithm>(algorithm, "balance.log", anchor_set, working_set, num_removals, num_keys);
            } else if (benchmark == "resize-balance") {
                /* code */
            } else if (benchmark == "monotonicity") {
                Benchmark::monotonicity<Algorithm>(algorithm, "monotonicity.log", anchor_set, working_set, num_removals, num_keys);
            }
        }
    }

    /**
     * BALANCE
    */
    template <typename Algorithm>
    static int balance(const string_view algorithm, const string &filename,
        uint32_t anchor_set, uint32_t working_set,
        uint32_t num_removals, uint32_t num_keys) {
        Balance::bench<Algorithm>(algorithm, filename, anchor_set, working_set, num_removals, num_keys);
        
        return 0;
    }

    /**
     * MONOTONICITY
    */
    template <typename Algorithm>
    static int monotonicity(const string_view algorithm, const string &filename,
        uint32_t anchor_set, uint32_t working_set,
        uint32_t num_removals, uint32_t num_keys) {
        Monotonicity::bench<Algorithm>(algorithm, filename, anchor_set, working_set, num_removals, num_keys);
        
        return 0;
    }

    /**
     * SPEED_TEST
    */
    template <typename Algorithm>
    static int lookup_time(const string_view algorithm, const string &filename,
        uint32_t anchor_set, uint32_t working_set,
        uint32_t num_removals, uint32_t num_keys) {
        SpeedTest::bench<Algorithm>(algorithm, filename, anchor_set, working_set, num_removals, num_keys);

        return 0;
    }
};
