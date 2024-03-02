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
#ifndef MEMENTOENGINE_H
#define MEMENTOENGINE_H
#include "memento.h"
#include "../utils.h"
#include <string_view>
#include <xxhash.h>

template <template <typename...> class MementoMap, typename... Args>
class MementoEngine final {
public:
  /**
   * Creates a new MementoHash engine.
   *
   * @param size          initial number of working buckets (0 < size)
   */
  MementoEngine(uint32_t, uint32_t size)
      : m_lastRemoved{size}, m_bArraySize{size} {}

  /**
   * Returns the bucket where the given key should be mapped.
   *
   * @param key the key to map
   * @return the related bucket
   */
  uint32_t getBucket(std::string_view key) const noexcept {
    const auto hash{XXH64(key.data(), key.size(), 0)};
    /*
     * We invoke JumpHash to get a bucket
     * in the range [0,bArraySize-1].
     */
    auto b = JumpConsistentHash(hash, m_bArraySize);

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
      const auto h = XXH64(key.data(), key.size(), b);
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
     * We invoke JumpHash to get a bucket
     * in the range [0,bArraySize-1].
     */
    auto b = JumpConsistentHash(hash, m_bArraySize);

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
      return bucket;
    }

    /* Otherwise, we add the entry to the memento table using the removed bucket
     * as the key. */
    m_lastRemoved = m_memento.remember(bucket, size() - 1, m_lastRemoved);

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

  // From Jump paper
  static int32_t JumpConsistentHash(uint64_t key, int32_t num_buckets) {
    int64_t b = 1, j = 0;
    while (j < num_buckets) {
      b = j;
      key = key * 2862933555777941757ULL + 1;
      j = (b + 1) * (double(1LL << 31) / double((key >> 33) + 1));
    }
    return b;
  }

  Memento<MementoMap> m_memento;
  uint32_t m_bArraySize;
  uint32_t m_lastRemoved;
};

#endif // MEMENTOENGINE_H
