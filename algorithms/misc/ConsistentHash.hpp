/**
 * @author Roberto Vicario (C) 2024
*/

#pragma once

#include <cstdint>

/**
 *
 */
class ConsistentHash {
public:
    virtual ~ConsistentHash() {}
    virtual const char* getName() = 0;
    virtual uint32_t getSize() = 0;
    virtual void updateRemoval(uint32_t index) = 0;
    virtual uint32_t updateAddition() = 0;
    virtual uint32_t getNodeID(uint32_t key, uint32_t* numHash) = 0;
};
