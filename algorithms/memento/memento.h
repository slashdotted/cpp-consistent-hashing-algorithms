#ifndef MEMENTO_H
#define MEMENTO_H
#include <stdint.h>
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

template<template <typename...> class MementoMap, typename... Args>
class Memento final
{
    struct Entry final
    {
        /**
         * Represents the bucket that will replace the current one.
         * This value also represents the size of the working set
         * after the removal of the current bucket.
         */
        uint32_t replacer;

        /** Keep track of the bucket removed before the current one. */
        uint32_t prevRemoved;
    };

    MementoMap<uint32_t, Entry> m_table;

public:
    Memento() {}

    /**
     * Returns the size of the replacement set.
     *
     * @return the size of the replacement set
     */
    int32_t size() const noexcept { return m_table.size(); }

    /**
     * Remembers that the given bucket has been removed
     * and that was replaced by the given replacer.
     * <p>
     * This method also stores the last removed bucket
     * (before the current one) to create the sequence
     * of removals.
     *
     * @param bucket      the removed bucket
     * @param replacer    the replacing bucket
     * @param prevRemoved the previous removed bucket
     * @return the value of the new last removed bucket
     */
    int32_t remember(uint32_t bucket, uint32_t replacer, uint32_t prevRemoved ) noexcept {
        m_table.emplace(bucket, Entry{replacer, prevRemoved});
        return bucket;
    }

    /**
     * Restores the given bucket by removing it
     * from the memory.
     * <p>
     * If the memory is empty the last removed bucket
     * becomes the given bucket + 1.
     *
     * @param bucket the bucket to restore
     * @return the new last removed bucket
     */
    int32_t restore(uint32_t bucket) noexcept {
        if (m_table.empty()) {
            return bucket + 1;
        }
        auto e = m_table.find(bucket);
        auto ret{e->second.prevRemoved};
        m_table.erase(e);
        return ret;
    }

    /**
     * Returns the replacer of the bucket if it
     * was removed, otherwise returns {@code -1}.
     * <p>
     * The value returned by this method represents
     * both the bucket that replaced the given one
     * and the size of the working set after removing
     * the given bucket.
     *
     * @param bucket the bucket to search for
     * @return the replacing bucket if any, {@code -1} otherwise
     */
    int32_t replacer(int32_t bucket ) const noexcept {
        auto e = m_table.find(bucket);
        if (e != m_table.end()) {
            return e->second.replacer;
        } else {
            return -1;
        }
    }
};
#endif // MEMENTO_H
