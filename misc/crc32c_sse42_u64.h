/**
 * @author Roberto Vicario @ SUPSI
 * @cite https://github.com/ChaosD/DxHash.git
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef CRC32C_SSE42_U64_H
#define CRC32C_SSE42_U64_H

#include <stdint.h>

static inline uint32_t crc32c_sse42_u64(uint64_t key, uint64_t seed) {
	__asm__ volatile(
			"crc32q %[key], %[seed];"
			: [seed] "+r" (seed)
			: [key] "rm" (key));
	return seed;
}

static uint32_t gen32bitRandNumber(uint32_t K) {
	return (K * 421757ULL + 1);
}

#endif
