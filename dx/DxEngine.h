/**
 * @author Roberto Vicario @ SUPSI
*/

#ifndef DXENGINE_H
#define DXENGINE_H

#include <cstdint>
#include "DxHash.cpp"

class DxEngine final {
public:
    DxEngine(uint32_t anchor_set, uint32_t working_set)
    : dx{anchor_set, working_set}
    {}

    uint32_t getBucketCRC32c(uint32_t key, uint32_t* seed) noexcept { return dx.getNodeID(key, seed); }

    uint32_t addBucket() noexcept { return dx.updateAddition(); }

    uint32_t removeBucket(uint32_t bucket) noexcept {
        dx.updateRemoval(bucket);
        return bucket;
    }

private:
    DxHash dx;
};

#endif // DXENGINE_H
