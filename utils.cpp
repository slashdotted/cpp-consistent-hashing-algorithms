/*
 * Copyright (c) 2023 Amos Brocco.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "utils.h"

uint32_t crc32c_sse42_u64(uint64_t key, uint64_t seed) {
#ifdef __x86_64
    __asm__ volatile("crc32q %[key], %[seed];"
                     : [seed] "+r"(seed)
                     : [key] "rm"(key));
#elif __aarch64__
    __asm__ volatile("crc32x %[key], %[seed];"
                     : [seed] "+r"(seed)
                     : [key] "rm"(key));
#endif
    return seed;
}
