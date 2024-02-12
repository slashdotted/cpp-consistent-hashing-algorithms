/**
 * @author Roberto Vicario @ SUPSI
 * @cite https://github.com/ChaosD/DxHash.git
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef CONSISTENTHASH_H
#define CONSISTENTHASH_H

#include <cstdint>

class ConsistentHash {
public:
    virtual const char* getName() const = 0;
    virtual uint32_t getSize() const = 0;
    virtual void updateRemoval(uint32_t index) = 0;
    virtual uint32_t updateAddition() = 0;
    virtual uint32_t getNodeID(uint32_t key, uint32_t* numHash) = 0;
    virtual ~ConsistentHash() {}
};

#endif CONSISTENTHASH_H
