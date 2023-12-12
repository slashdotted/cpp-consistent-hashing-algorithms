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
#ifndef MEMENTONEENGINE_H
#define MEMENTONEENGINE_H
#include "memento.h"
#include "pcg_random.hpp"
#include <cmath>
#include <xxhash.h>

template <template <typename...> class MementoMap, typename... Args>
class MementoneEngine final {
public:
  /**
   * Creates a new MementoHash engine.
   *
   * @param size          initial number of working buckets (0 < size)
   */
  MementoneEngine(uint32_t, uint32_t size)
      : m_bArraySize{size}, m_m{smallestPow2(size)}, m_mH{m_m >> 1}, m_mHm1{m_mH - 1}, m_mm1{m_m - 1}, m_lastRemoved{size} {
  }

  /**
   * Returns the bucket where the given key should be mapped.
   * This version uses the same hash function as Anchor
   *
   * @param key the key to map
   * @param seed the initial seed for CRC32c
   * @return the related bucket
   */
  uint32_t getBucketCRC32c(uint64_t key, uint64_t seed) const noexcept {
    const auto hash = crc32c_sse42_u64(key, seed);

    /*
     * We invoke PowerHash to get a bucket
     * in the range [0,bArraySize-1].
     */
    auto b = powerConsistentHash(hash, m_bArraySize, m_mm1, m_mHm1);

    /*
     * We check if the bucket was removed, if not we are done.
     * If the bucket was removed the replacing bucket is >= 0,
     * otherwise it is -1.
     */
    auto replacer = m_memento.replacer(b);
    while (replacer >= 0) {

      /*
       * If the bucket was removed, we must re-hash and find
       * a new bucket in the remaining slots. To know the
       * remaining slots, we look at 'replacer' that also
       * represents the size of the working set when the bucket
       * was removed and get a new bucket in [0,replacer-1].
       */
      const auto h = crc32c_sse42_u64(key, b);
      b = h % replacer;

      /*
       * If we hit a removed bucket we follow the replacements
       * until we get a working bucket or a bucket in the range
       * [0,replacer-1]
       */
      auto r = m_memento.replacer(b);
      while (r >= replacer) {
        b = r;
        r = m_memento.replacer(b);
      }

      /* Finally we update the entry of the external loop. */
      replacer = r;
    }

    return b;
  }

  /**
   * Adds a new bucket to the engine.
   *
   * @return the added bucket
   */
  uint32_t addBucket() noexcept {
    /* The new bucket to add is the last removed one. */
    auto bucket = m_lastRemoved;

    /**
     * We restore the bucket from the replacement set
     * and update the value of the last removed bucket.
     */
    m_lastRemoved = m_memento.restore(bucket);

    /**
     * If the restored bucket is 'bArraySize'
     * we must update the actual size by 1.
     */
    m_bArraySize = m_bArraySize > bucket ? m_bArraySize : bucket + 1;

    updatePowerParameters();

    return bucket;
  }

  /**
   * Removes the given bucket from the engine.
   *
   * @param bucket the bucket to remove
   * @return the removed bucket
   */
  uint32_t removeBucket(uint32_t bucket) noexcept {
    /*
     * If the lookup table is empty and the bucket to remove is the last one,
     * we are in the same use case as JumpHash. In this case we don't need
     * to remember the bucket, we just need to reduce the size of the b-array.
     */
    if ((m_lastRemoved == m_bArraySize) && bucket == m_bArraySize - 1) {
      m_lastRemoved = m_bArraySize = bucket;
      updatePowerParameters();
      return bucket;
    }

    /* Otherwise, we add the entry to the memento table using the removed bucket
     * as the key. */
    m_lastRemoved = m_memento.remember(bucket, size() - 1, m_lastRemoved);

    updatePowerParameters();

    return bucket;
  }

  /**
   * Returns the size of the working set.
   *
   * @return size of the working set.
   */
  uint32_t size() const noexcept { return m_bArraySize - m_memento.size(); }

  /**
   * Returns the size of the b-array.
   *
   * @return the size of the b-array.
   */
  uint32_t bArraySize() const noexcept { return m_bArraySize; }

private:

  void updatePowerParameters() noexcept {
      m_m = smallestPow2(m_bArraySize);
      m_mH = m_m >> 1;
      m_mHm1 = m_mH - 1;
      m_mm1 = m_m - 1;
  }

  // From AnchorHash
  static uint32_t crc32c_sse42_u64(uint64_t key, uint64_t seed) {
    __asm__ volatile("crc32q %[key], %[seed];"
                     : [seed] "+r"(seed)
                     : [key] "rm"(key));
    return seed;
  }

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
     * Powerhash algorithm
     *
     * @param key
     * @param n
     * @param m-1
     * @param m/2-1
     * @return
     */
  static uint32_t powerConsistentHash(uint32_t k, uint32_t n, uint32_t mm1, uint mHm1) noexcept {
      pcg32 rng;
      // r1 = f (key, m) (we pass m-1 because f expects that)
      auto r1 = f(k, mm1, rng);
      if (r1 < n) {
          return r1;
      }
      // r2 = g(key, n, m/2 − 1)
      auto r2 = g(k, n, mHm1, rng);
      if (r2 > mHm1) {
          return r2;
      }
      // f (key, m/2) (we pass m/2-1 because f expects that)
      return f(k, mHm1, rng);
  }

  /**
     * Algorithm-f, described in Section VI.A, pages 7 and 8
     *
     * @param key
     * @param m
     * @return
     */
  static uint32_t f(uint32_t key, uint32_t mm1, pcg32& rng) {
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
  uint32_t m_bArraySize;

  /* Smallest power of 2 greater or equal to n */
  uint32_t m_m;

  /* Smallest power of 2 greater or equal to n minus 1 */
  uint32_t m_mm1;

  /* Half of m */
  uint32_t m_mH;

  /* Half of m minus 1 */
  uint32_t m_mHm1;

  Memento<MementoMap> m_memento;
  uint32_t m_lastRemoved;
};

#endif // MEMENTONEENGINE_H
