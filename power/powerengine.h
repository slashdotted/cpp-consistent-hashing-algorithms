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
#ifndef POWERENGINE_H
#define POWERENGINE_H
#include <cmath>
#include <cstdint>
#include "../utils.h"
#include "pcg_random.hpp"

class PowerEngine final {
public:
    PowerEngine(uint32_t, uint32_t working_nodes)
        : m_n{working_nodes}, m_m{smallestPow2(m_n)}, m_mH{m_m >> 1}, m_mHm1{m_mH - 1}, m_mm1{m_m - 1}
    {}

    /**
   * Returns the bucket where the given key should be mapped.
   * This implementations is the same as provided by Power authors
   *
   * @param key the key to map
   * @param seed the initial seed for CRC32c
   * @return the related bucket
   */
    uint32_t getBucketCRC32c(uint64_t key, uint64_t seed) noexcept
    {
        pcg32 rng;
        auto k = crc32c_sse42_u64(key, seed);
        // r1 = f (key, m) (we pass m-1 because f expects that)
        auto r1 = f(k, m_mm1, rng);
        if (r1 < m_n) {
            return r1;
        }
        // r2 = g(key, n, m/2 − 1)
        auto r2 = g(k, m_n, m_mHm1, rng);
        if (r2 > m_mHm1) {
            return r2;
        }
        // f (key, m/2) (we pass m/2-1 because f expects that)
        return f(k, m_mHm1, rng);
    }

    /**
   * Adds a new bucket to the engine.
   *
   * @return the added bucket
   */
    uint32_t addBucket() noexcept {
        m_m = smallestPow2(m_n+1);
        m_mH = m_m >> 1;
        m_mHm1 = m_mH - 1;
        m_mm1 = m_m - 1;
        return m_n++;
    }

    /**
   * Removes the given bucket from the engine.
   * Since Power does not support random removals, it will always remove the
   * last bucket.
   *
   * @return the removed bucket
   */
    uint32_t removeBucket(uint32_t) noexcept
    {
        m_m = smallestPow2(--m_n);
        m_mH = m_m >> 1;
        m_mHm1 = m_mH - 1;
        m_mm1 = m_m - 1;
        return m_n;
    }

private:

    static uint32_t smallestPow2(uint32_t x) {
        --x;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return x+1;
    }

    /**
     * Algorithm-f, described in Section VI.A, pages 7 and 8
     *
     * @param key
     * @param m
     * @return
     */
    static uint32_t f(uint32_t key, uint32_t mm1, pcg32& rng){
        // (...) extracts log2(m) bits from the given key
        auto kBits = (key & mm1);
        if (kBits == 0) {
            return 0;
        }
        // [Computes] the bit index of the most significant bit set
        // to 1 in kBits
        auto j = (sizeof(kBits)<<3) - __builtin_clz(kBits) - 1;
        // (...) computes 2^j
        auto h = static_cast<uint32_t>(1) << j;
        // [must return] a pseudo-random integer deterministacally based on
        // the values of key and j
        rng.seed(key, j);
        // (...) produces a random integer r in the ranger [h,2h-1] with equal
        // probability
        auto r = h + (rng() & (h - 1));
        return r;
    }

    /**
     * Algorithm-g, described in Section VI.B, pages 8 and 9
     *
     * @param key
     * @param n
     * @param s
     * @return
     */
    static uint32_t g(uint32_t key, uint32_t n, uint32_t s, pcg32& rng) {
        auto x = s; // (...) Initially, x is set to the value of s
        for (;;) {
            // (...) 1. Generate U
            //          U denotes the next random number from a generator U (0, 1)
            //          that generates random numbers
            //          uniformly over range (0, 1) and deterministically based on the given key.
            rng.seed(key);
            auto u = (static_cast<double>(rng())/static_cast<double>(rng.max()));
            // (...) 2. Compute r = min{j: U>(x+1)/(j+1)
            auto r = (uint32_t) ceil((static_cast<uint64_t>(x) + 1) / u) - 1;
            // (...) 3. Set x = r if r < n
            if (r < n) {
                x = r;
            } else {
                // (...) Otherwise, the algorithm returns the current
                // value of x as the result
                return x;
            }
        }
    }

    /* Number of nodes  in the cluster */
    uint32_t m_n;

    /* Smallest power of 2 greater or equal to n */
    uint32_t m_m;

    /* Smallest power of 2 greater or equal to n minus 1 */
    uint32_t m_mm1;

    /* Half of m */
    uint32_t m_mH;

    /* Half of m minus 1 */
    uint32_t m_mHm1;

};

#endif // POWERENGINE_H
