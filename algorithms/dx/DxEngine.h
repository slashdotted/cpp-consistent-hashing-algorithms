/**
 * @copyright Roberto Vicario (C) 2024
*/

#pragma once

#include "DxHash.hpp"

class DxEngine {
    private:
        DxHash dx;

    public:
        DxEngine(uint32_t anchor_set, uint32_t working_set)
                : dx{anchor_set, working_set}
                {}

        uint32_t getBucketCRC32c(uint32_t key, uint32_t seed) { return dx.getNodeID(key, &seed); }

        uint32_t addBucket() { return dx.updateAddition(); }

        uint32_t removeBucket(uint32_t bucket) {
            dx.updateRemoval(bucket);
            return bucket;
        }
};
