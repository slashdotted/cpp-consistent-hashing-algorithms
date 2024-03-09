#ifndef BENCHMARK_H
#define BENCHMARK_H

class Benchmark {
private:
    int data;

public:
    Benchmark();
    int balance(const string_view name, const string &filename,
        uint32_t anchor_set, uint32_t working_set,
        uint32_t num_removals, uint32_t num_key
    );
    // int monotonicity();
    // int speed_test();
    // ~Benchmark();
};

#endif // BENCHMARK_H
