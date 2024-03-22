/**
 * @copyright Roberto Vicario (C) 2024
 *
 * @cite https://arxiv.org/pdf/2107.07930.pdf
 */

#include <cstdio>
#include "../misc/ConsistentHash.hpp"

/*
class DxHash : public ConsistentHash {
private:
    std::vector<bool> nodes;
    uint32_t num_working;
    uint32_t size;

    uint32_t getInactiveNodeID(uint32_t key);
    void resize();

public:
    DxHash(uint32_t);
    DxHash(uint32_t, uint32_t);
    ~DxHash() {};

    uint32_t getNodeID(uint32_t K, uint32_t* numHash);
    uint32_t getOfflineSize();
    uint32_t getSize();
    uint32_t updateAddition();
    void updateRemoval(uint32_t index);
};
*/