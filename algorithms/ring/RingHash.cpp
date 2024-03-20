/**
 * @author Roberto Vicario @ SUPSI
 * 
 * @cite https://github.com/ChaosD/DxHash.git
*/

#include <cstdio>
#include "../../misc/crc32c_sse42_u64.h"
#include "RingHash.h"

void RingHash::insert(uint32_t nodeID) {
    //for (uint32_t i = 0; i < V; i++) {
        uint32_t key2 = generate32RandomNumber(0);
        uint32_t hashValue = crc32c_sse42_u64(nodeID, key2);
        workSet[hashValue] = nodeID;
    //}
}

RingHash::RingHash(uint32_t numNodes, uint32_t numHashFunctions) : V(numHashFunctions), failedSet(), workSet() {
    for (uint32_t i = 0; i < numNodes; i++) {
        insert(i);
    }
}

void RingHash::updateRemoval(uint32_t nodeID) {
    //for (uint32_t i = 0; i < V; i++) {
        uint32_t key2 = generate32RandomNumber(0);
        uint32_t hashValue = crc32c_sse42_u64(nodeID, key2);
        workSet.erase(hashValue);
    //}
    failedSet.push(nodeID);
}

uint32_t RingHash::updateAddition() {
    uint32_t nodeID = failedSet.top();
    failedSet.pop();
    insert(nodeID);
    return nodeID;
}

uint32_t RingHash::getNodeID(uint32_t key, uint32_t* numHash) {
    uint32_t key2 = generate32RandomNumber(*numHash);
    uint32_t hashValue = crc32c_sse42_u64(key, key2);
    auto pNode = workSet.find(hashValue);

    if (pNode == workSet.end()) {
        pNode = workSet.begin();
    }

    return pNode->second;
}

uint32_t RingHash::getSize() {
    return workSet.size() / V;
}
