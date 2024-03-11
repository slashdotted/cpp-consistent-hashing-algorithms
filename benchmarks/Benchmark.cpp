/**
 * @author Amos Brocco
 * @author Roberto Vicario
 * @date 2024
*/

#include "Balance.cpp"
#include "Monotonicity.cpp"
// #include "SpeedTest.cpp"

using namespace std;

class Benchmark {
public:
    /**
     * BALANCE
    */
    template <typename Algorithm>
    static int balance(const string_view name, const string &filename,
        uint32_t anchor_set, uint32_t working_set,
        uint32_t num_removals, uint32_t num_keys) {
        Balance::bench<Algorithm>(name, filename, anchor_set, working_set, num_removals, num_keys);
        
        return 0;
    }

    /**
     * MONOTONICITY
    */
    template <typename Algorithm>
    static int monotonicity(const string_view name, const string &filename,
        uint32_t anchor_set, uint32_t working_set,
        uint32_t num_removals, uint32_t num_keys) {
        Monotonicity::bench<Algorithm>(name, filename, anchor_set, working_set, num_removals, num_keys);
        
        return 0;
    }

    /**
     * SPEED_TEST
    */
    template <typename Algorithm>
    static int speed_test(const string_view name, const string &filename,
        uint32_t anchor_set, uint32_t working_set,
        uint32_t num_removals, uint32_t num_keys) {
        // SpeedTest::bench<Algorithm>(name, filename, anchor_set, working_set, num_removals, num_keys);

        return 0;
    }
};
