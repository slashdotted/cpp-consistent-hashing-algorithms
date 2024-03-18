#include <ctime>

#include "Balance.h"
#include "Monotonicity.h"
#include <iostream>
#include "SpeedTest.h"
#include "MemoryUsage.h"
#include "InitTime.h"
#include <yaml-cpp/yaml.h>
using namespace std;

class Benchmark {
public:
    /**
     * BENCHMARK ROUTINE
    */
    template <typename Engine>
    static void execute(YAML::Node config, string algorithm, uint32_t anchor_set,
        uint32_t working_set, uint32_t num_removals, uint32_t num_keys) {
        /**
         * Starting benchmark routine.
         */
        cout << "# [LOG] ----- Parameters: (algorithm = " << algorithm << ", function = crc32, initNodes = " << working_set << ")\n#" << endl;
        for (const auto &iter : config["benchmarks"]) {
            auto benchmark = iter["name"].as<string>();
            if (benchmark == "balance") {
                Benchmark::balance<Engine>(algorithm, "balance.log", anchor_set, working_set, num_removals, num_keys);
            } else if (benchmark == "init-time") {
                Benchmark::init_time<Engine>(algorithm, anchor_set, working_set);
            } else if (benchmark == "lookup-time") {
                Benchmark::lookup_time<Engine>(algorithm, "lookup_time.log",anchor_set, working_set, num_removals, num_keys);
            } else if (benchmark == "memory-usage") {
                Benchmark::memory_usage<Engine>(algorithm, anchor_set, working_set);
            } else if (benchmark == "monotonicity") {
                Benchmark::monotonicity<Engine>(algorithm, "monotonicity.log", anchor_set, working_set, num_removals, num_keys);
            } else if (benchmark == "resize-balance") {
                /* code */
            } else if (benchmark == "resize-time") {
                /* code */
            }

        }
    }

    /**
     * BALANCE
    */
    template <typename Engine>
    static int balance(const string_view algorithm, const string &filename,
        uint32_t anchor_set, uint32_t working_set,
        uint32_t num_removals, uint32_t num_keys) {
        Balance::bench<Engine>(algorithm, filename, anchor_set, working_set, num_removals, num_keys);
        
        return 0;
    }

    /**
     * MEMORY_USAGE
     */
    template <typename Engine>
    static int memory_usage(string algorithm, uint32_t anchor_set, uint32_t working_set) {
        MemoryUsage::bench<Engine>(algorithm, anchor_set, working_set);
        return 0;
    }

    /**
     * INIT_TIME
    */
    template <typename Engine>
    static int init_time(string algorithm, uint32_t anchor_set, uint32_t working_set) {
        InitTime::bench<Engine>(algorithm, anchor_set, working_set);
        return 0;
    }

    /**
     * MONOTONICITY
    */
    template <typename Engine>
    static int monotonicity(const string_view algorithm, const string &filename,
        uint32_t anchor_set, uint32_t working_set,
        uint32_t num_removals, uint32_t num_keys) {
        Monotonicity::bench<Engine>(algorithm, filename, anchor_set, working_set, num_removals, num_keys);
        
        return 0;
    }

    /**
     * SPEED_TEST
    */
    template <typename Engine>
    static int lookup_time(const string_view algorithm, const string &filename,
        uint32_t anchor_set, uint32_t working_set,
        uint32_t num_removals, uint32_t num_keys) {
        SpeedTest::bench<Engine>(algorithm, filename, anchor_set, working_set, num_removals, num_keys);

        return 0;
    }
};
