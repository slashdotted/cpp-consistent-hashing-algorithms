/**
 * @author Roberto Vicario @ SUPSI
 * 
 * @cite https://github.com/ChaosD/DxHash.git
*/

#ifndef CRC32C_SSE42_U64_H
#define CRC32C_SSE42_U64_H

#include <stdint.h>

static inline uint32_t crc32c_sse42_u64(uint64_t key, uint64_t seed) {
    #ifdef __x86_64
        __asm__ volatile("crc32q %[key], %[seed];"
                        : [seed] "+r"(seed)
                        : [key] "rm"(key));
                        
        return seed;
    #elif __aarch64__
        uint32_t crc = 0xFFFFFFFF;

        asm volatile(
            "crc32cx %w0, %w0, %x1"
            : "+r" (crc)
            : "r" (key)
            );

        crc ^= seed;

        return crc;
    #endif
}

static uint32_t generate32RandomNumber(const uint32_t key){
    return (key * 421757ULL + 1);
}

#endif // CRC32C_SSE42_U64_H