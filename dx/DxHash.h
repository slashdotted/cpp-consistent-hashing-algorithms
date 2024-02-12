/**
 * @author Roberto Vicario @ SUPSI
 * @cite https://github.com/ChaosD/DxHash.git
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stack>
#include <vector>
#include "../misc/ConsistentHash.h"

#ifndef DXHASH_H
#define DXHASH_H

using namespace std;

class DxHash : public ConsistentHash {
private:
    vector<bool> nodes;
    uint32_t size;
    uint32_t num_working;

    void resize();
    uint32_t getInactiveNodeID(uint32_t key);
    const char *name = "DxHash";

public:
    DxHash(uint32_t);
    DxHash(uint32_t, uint32_t);
    virtual ~DxHash(){};

public:
    uint32_t getSize();
    uint32_t getOfflineSize();
    void updateRemoval(uint32_t index);
    uint32_t updateAddition();
    uint32_t getNodeID(uint32_t K, uint32_t* numHash);
    const char* getName() { return name; }
};

/*

///////////////////////////////////////////////////////
////////// THE FOLLOWING ARE OTHER FEATURES ///////////
///////////////////////////////////////////////////////

class DxHash_TO1 : public ConsistentHash {
private:
    uint8_t* nodes;
    uint32_t size;
    uint32_t num_working;

    void resize();
    uint32_t getInactiveNodeID(uint32_t key);
    const char *name = "DxHash_TO1";

public:
    DxHash_TO1(uint32_t);
    DxHash_TO1(uint32_t, uint32_t);
    virtual ~DxHash_TO1();

public:
    uint32_t getSize();
    uint32_t getOfflineSize();
    void updateRemoval(uint32_t index);
    uint32_t updateAddition();
    uint32_t getNodeID(uint32_t K, uint32_t* numHash);
    const char* getName() { return name; }
};

class DxHash_TO2 : public ConsistentHash {
private:
    uint8_t* nodes;
    uint32_t* IQueue;
    uint32_t IQueue_size;
    uint32_t size;
    uint32_t num_working;

    void resize();
    const char *name = "DxHash_TO2";

public:
    DxHash_TO2(uint32_t);
    DxHash_TO2(uint32_t, uint32_t);
    virtual ~DxHash_TO2();

public:
    uint32_t getSize();
    uint32_t getOfflineSize();
    void updateRemoval(uint32_t index);
    uint32_t updateAddition();
    uint32_t getNodeID(uint32_t K, uint32_t* numHash);
    const char* getName() { return name; }
};

*/

#endif
