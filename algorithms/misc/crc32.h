/**
 * @author Roberto Vicario (C) 2024
*/

#pragma once

#include <stdint.h>

/**
 *
 * @param key
 * @param seed
 * @return
 */
static inline uint32_t crc32c_sse42_u64(uint32_t key, uint32_t seed) {
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
            : "r" (key));

        crc ^= seed;

        return crc;
    #endif
}

/**
 *
 * @param key
 * @return
 */
static uint32_t generate32RandomNumber(uint32_t key) {
    return (key * 421757ULL + 1);
}
