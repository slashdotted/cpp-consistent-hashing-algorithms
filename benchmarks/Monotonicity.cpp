/**
 * @author Amos Brocco
 * @author Roberto Vicario
 * @date 2024
*/

#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered_map.hpp>
#include <cxxopts.hpp>
#ifdef USE_PCG32
#include "pcg_random.hpp"
#include <random>
#endif // USE_PCG32
#include <fmt/core.h>
#include <fstream>
#include <gtl/phmap.hpp>
#include <unordered_map>

using namespace std;

class Monotonicity {

public:
template <typename Algorithm>
    static int bench(const string_view name, const string &filename,
            uint32_t anchor_set, uint32_t working_set, uint32_t num_removals,
            uint32_t num_keys) {
        Algorithm engine(anchor_set, working_set);

        uint32_t *bucket_status = new uint32_t[anchor_set]();

        for (uint32_t i = 0; i < working_set; i++) {
            bucket_status[i] = 1;
        }

        uint32_t i = 0;
        while (i < num_removals) {
        #ifdef USE_PCG32
            uint32_t removed = rng() % working_set;
        #else
            uint32_t removed = rand() % working_set;
        #endif
            if (bucket_status[removed] == 1) {
            auto rnode = engine.removeBucket(removed);
            bucket_status[rnode] = 0;
            i++;
            }
        }

        boost::unordered_flat_map<pair<uint32_t, uint32_t>, uint32_t> bucket;
        ofstream results_file;
        results_file.open(filename, ofstream::out | ofstream::app);

        for (uint32_t i = 0; i < num_keys;) {
        #ifdef USE_PCG32
            auto a{rng()};
            auto b{rng()};
        #else
            auto a{rand()};
            auto b{rand()};
        #endif
            if (bucket.contains({a, b}))
            continue;
            auto target = engine.getBucketCRC32c(a, b);
            bucket[{a, b}] = target;
            if (!bucket_status[target]) {
            throw "Crazy bug";
            }
            ++i;
        }

        fmt::println("Done determining initial assignment of {} unique keys",
                    num_keys);

        uint32_t removed{0};
        uint32_t rnode{0};
        for (;;) {
        #ifdef USE_PCG32
            removed = rng() % working_set;
        #else
            removed = rand() % working_set;
        #endif
            if (bucket_status[removed] == 1) {
            rnode = engine.removeBucket(removed);
            fmt::println("Removed node {}", rnode);
            if (!bucket_status[rnode]) {
                throw "Crazy bug";
            }
            bucket_status[rnode] = 0;
            break;
            }
        }

        uint32_t misplaced{0};
        for (const auto &i : bucket) {
            auto oldbucket = i.second;
            auto a{i.first.first};
            auto b{i.first.second};
            auto newbucket = engine.getBucketCRC32c(a, b);
            if (oldbucket != newbucket && (oldbucket != rnode)) {
            fmt::println("(After Removal) Misplaced key {},{}: before in bucket {}, "
                        "now in bucket {} (status? old bucket {}, new bucket {})",
                        a, b, oldbucket, newbucket, bucket_status[oldbucket],
                        bucket_status[newbucket]);
            ++misplaced;
            }
        }

        double m = (double)misplaced / (num_keys);
        #ifdef USE_PCG32
        fmt::println(
            "{}: after removal % misplaced keys are {}% ({} keys out of {})\n", name,
            m * 100, misplaced, num_keys);
        results_file << name << ": "
                    << "MisplacedRem: " << misplaced << "\t" << num_keys << "\t" << m
                    << "\t" << m << "\tPCG32\n";
        #else
        fmt::println("{}: after removal misplaced keys are {}% ({} keys out of {})",
                    name, m * 100, misplaced, num_keys);
        results_file << name << ": "
                    << "MisplacedRem: " << misplaced << "\t" << num_keys << "\t" << m
                    << "\t" << m << "\trand()\n";
        #endif

        misplaced = 0;
        auto anode = engine.addBucket();
        bucket_status[anode] = 1;
        fmt::println("Added node {}", anode);

        for (const auto &i : bucket) {
            auto oldbucket = i.second;
            auto a{i.first.first};
            auto b{i.first.second};
            auto newbucket = engine.getBucketCRC32c(a, b);
            if (oldbucket != newbucket) {
            fmt::println("(After Add) Misplaced key {},{}: before in bucket {}, now "
                        "in bucket {} (status? old bucket {}, new bucket {})",
                        a, b, oldbucket, newbucket, bucket_status[oldbucket],
                        bucket_status[newbucket]);
            ++misplaced;
            }
        }

        m = (double)misplaced / (num_keys);

        #ifdef USE_PCG32
        fmt::println(
            "{}: after adding back % misplaced keys are {}% ({} keys out of {})\n",
            name, m * 100, misplaced, num_keys);
        results_file << name << ": "
                    << "MisplacedAdd: " << misplaced << "\t" << num_keys << "\t" << m
                    << "\t" << m << "\tPCG32\n";
        #else
        fmt::println(
            "{}: after adding back misplaced keys are {}% ({} keys out of {})", name,
            m * 100, misplaced, num_keys);
        results_file << name << ": "
                    << "MisplacedAdd: " << misplaced << "\t" << num_keys << "\t" << m
                    << "\t" << m << "\trand()\n";
        #endif

        results_file.close();

        delete[] bucket_status;

        return 0;
    }
};
