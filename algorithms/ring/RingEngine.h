/**
 * @author Roberto Vicario @ SUPSI
*/

#ifndef RINGENGINE_H
#define RINGENGINE_H

#include <cstdint>
#include "RingHash.cpp"

class RingEngine final {
public:
    RingEngine(uint32_t anchor_set, uint32_t working_set)
    : ring{anchor_set, working_set}
    {}

    uint32_t getBucketCRC32c(uint32_t key, uint32_t seed) noexcept { return ring.getNodeID(key, &seed); }

    uint32_t addBucket() noexcept { return ring.updateAddition(); }

    uint32_t removeBucket(uint32_t bucket) noexcept {
        ring.updateRemoval(bucket);
        return bucket;
    }

private:
    RingHash ring;
};

#endif // RINGENGINE_H
