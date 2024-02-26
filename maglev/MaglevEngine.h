/**
 * @author Roberto Vicario @ SUPSI
*/

#ifndef MAGLEVENGINE_H
#define MAGLEVENGINE_H

#include <cstdint>
#include "MaglevHash.cpp"

class MaglevEngine final {
public:
    MaglevEngine(uint32_t anchor_set, uint32_t working_set)
    : maglev{anchor_set, working_set}
    {}

    uint32_t getBucketCRC32c(uint32_t key, uint32_t seed) noexcept { return maglev.getNodeID(key, &seed); }

    uint32_t addBucket() noexcept { return maglev.updateAddition(); }

    uint32_t removeBucket(uint32_t bucket) noexcept {
        maglev.updateRemoval(bucket);
        return bucket;
    }

private:
    MaglevHash maglev;
};

#endif // MAGLEVENGINE_H
