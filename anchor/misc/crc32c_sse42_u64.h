// Harware based crc calculation
#include <stdint.h>

static inline uint32_t crc32c_sse42_u64(uint64_t key, uint64_t seed) {
#ifdef __x86_64
    __asm__ volatile("crc32q %[key], %[seed];"
                     : [seed] "+r"(seed)
                     : [key] "rm"(key));
    return seed;
#elif __aarch64__
    uint32_t crc = 0xFFFFFFFF; // Inizializza il CRC con il valore iniziale

    // Calcola il CRC utilizzando l'istruzione assembly crc32cx
    asm volatile(
        "crc32cx %w0, %w0, %x1" // Calcola il CRC tra il valore corrente e key
        : "+r" (crc)           // Output: il valore aggiornato del CRC
        : "r" (key)            // Input: il valore di key
        );

    // Applica il seed al risultato finale
    crc ^= seed;

    return crc;
#endif
}

