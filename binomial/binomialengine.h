/*
 * Copyright (c) 2024 Amos Brocco.
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
#ifndef BINOMIALENGINE_H
#define BINOMIALENGINE_H
#include <cstdint>
#include "../utils.h"
#include "pcg_random.hpp"


class BinomialEngine final {
public:
    BinomialEngine(uint32_t, uint32_t working_nodes)
        : m_size{working_nodes}
    {
        // Computes the next power of 2 greater than m_size
        int highestOneBit = BinomialEngine::highestOneBit(m_size);
        if(m_size > highestOneBit ) {
            highestOneBit = highestOneBit << 1;
        }
        // If K is the power of 2 greater than m_size
        // the upper tree filter is K-1
        m_upperTreeFilter = highestOneBit - 1;
        // the lower tree filter is K/2
        m_lowerTreeFilter = m_upperTreeFilter >> 1;
    }


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
        if( m_size < 2 )
            return 0;

        auto hash = crc32c_sse42_u64(key, seed);

        /* If the cluster counts only one node we return such a node. */

        /* We get a position within the upper tree based on the value of the key hash. */
        uint32_t bucket = (uint32_t) hash & m_upperTreeFilter;

        /* We relocate the bucket randomly inside the same tree level. */
        bucket = relocateInsideLevel( bucket, hash );

        /* If the final position is valid, we return it. */
        if( bucket < m_size )
            return bucket;

        /*
         * Otherwise, we get a new random position in the upper tree
         * and return it if in the range [lowerTreeFilter+1,size-1].
         * We repeat the operation twice (if needed) to get a better balance.
         */
        long h = hash;
        for( uint32_t i = 0; i < 2; ++i )
        {
            m_rng.seed(m_upperTreeFilter, h);
            h = m_rng();
            bucket = (uint32_t) h & m_upperTreeFilter;

            if( bucket > m_lowerTreeFilter && bucket < m_size )
                return bucket;

        }

        /*
         * Finally, if none of the previous operations succeed,
         * we remap the key in the range covered by the lower tree,
         * which is guaranteed valid.
         */
        bucket = (uint32_t) hash & m_lowerTreeFilter;
        return relocateInsideLevel( bucket, hash );

    }

    /**
   * Adds a new bucket to the engine.
   *
   * @return the added bucket
   */
    uint32_t addBucket() noexcept {
        uint32_t newBucket = m_size;
        if( ++m_size > m_upperTreeFilter )
        {
            m_upperTreeFilter = (m_upperTreeFilter << 1) | 1;
            m_lowerTreeFilter = (m_lowerTreeFilter << 1) | 1;
        }
        return newBucket;
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
        if( --m_size <= m_lowerTreeFilter )
        {
            m_lowerTreeFilter = m_lowerTreeFilter >> 1;
            m_upperTreeFilter = m_upperTreeFilter >> 1;
        }

        return m_size;

    }

private:

    uint32_t relocateInsideLevel( uint32_t bucket, uint64_t hash ) noexcept
    {

        /*
         * If the bucket is {@code 0} or {@code 1}, we are in the root
         * of the tree. Therefore, no relocation is needed.
         */
        if( bucket < 2 )
            return bucket;

        uint32_t levelBaseIndex = BinomialEngine::highestOneBit( bucket );
        uint32_t levelFilter = levelBaseIndex - 1;


        m_rng.seed(levelFilter, hash);
        uint64_t levelHash = m_rng();
        uint32_t levelIndex = (uint32_t) levelHash & levelFilter;

        return levelBaseIndex + levelIndex;
    }

    /*
     * Returns an integer with at most a single one-bit which is in
     * the position of the highest-order one-bit of the input value.
     */
    static unsigned int highestOneBit(unsigned int i) noexcept {
        i |= (i >> 1);
        i |= (i >> 2);
        i |= (i >> 4);
        i |= (i >> 8);
        i |= (i >> 16);
        return i - (i >> 1);
    }

    pcg32 m_rng;
    uint32_t m_size;
    uint32_t m_upperTreeFilter;
    uint32_t m_lowerTreeFilter;

};

#endif // BINOMIALENGINE_H
