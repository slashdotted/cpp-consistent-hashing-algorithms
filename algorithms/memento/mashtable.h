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
#ifndef MASHTABLE_H
#define MASHTABLE_H

#include <cstdint>
#include <utility>

template<class T>
concept Integral = std::is_integral<T>::value;

template <Integral K, typename V> class MashTable final {

  static constexpr uint32_t MIN_TABLE_SIZE = 1 << 4;
  static constexpr uint32_t MAX_TABLE_SIZE = 1 << 30;

  struct Item final {
    K m_key;
    V m_value;
    Item *m_next;

    Item(K key, V value, Item *next = nullptr)
        : m_key{key}, m_value{value}, m_next{next} {}
  };

  Item **m_table;
  uint32_t m_length;
  uint32_t m_size;

  void add(Item *entry, Item **table, uint32_t table_length) {
    auto kint{static_cast<unsigned int>(entry->m_key)};
    unsigned int hash = kint ^ kint >> 16;
    unsigned int index = (table_length - 1) & hash;
    entry->m_next = table[index];
    table[index] = entry;
  }

  void resizeTable(int newTableSize) {
    if (newTableSize < m_length && m_length <= MIN_TABLE_SIZE)
      return;
    if (newTableSize > m_length && m_length >= MAX_TABLE_SIZE)
      return;
    auto newTable{new Item *[newTableSize]()};
    for (unsigned int i{0}; i < m_length; ++i) {
      auto entry{m_table[i]};
      while (entry) {
        auto next = entry->m_next;
        entry->m_next = nullptr;
        add(entry, newTable, newTableSize);
        entry = next;
      }
    }
    delete[] m_table;
    m_length = newTableSize;
    m_table = newTable;
  }

  uint32_t capacity() const noexcept { return (m_length >> 2) * 3; }

  void remove(const K &key) {
    auto kint{static_cast<unsigned int>(key)};
    int hash = kint ^ kint >> 16;
    int index = (m_length - 1) & hash;
    auto entry{m_table[index]};
    if (!entry) {
      return;
    }
    Item *prev{nullptr};
    while (entry && entry->m_key != key) {
      prev = entry;
      entry = entry->m_next;
    }

    if (!entry) {
      return;
    }

    if (prev == nullptr) {
      m_table[index] = entry->m_next;
    } else {
      prev->m_next = entry->m_next;
    }
    delete entry;
  }

  void doFree(Item** t, uint32_t s) {
      for(unsigned int i{0}; i<s; ++i) {
          auto e{t[i]};
          while(e) {
              auto next{e->m_next};
              delete e;
              e = next;
          }
      }
      delete[] t;
  }

public:
  struct iterator final {
      iterator() : m_pair{nullptr, V()} {}
    iterator(Item *e) : m_pair{e, e->m_value} {}
    std::pair<Item *, V> m_pair;

    const std::pair<Item *, V> &operator*() const noexcept { return m_pair; }
    std::pair<Item *, V> *operator->() noexcept { return &m_pair; }
    bool operator==(const iterator &o) const noexcept {
      return m_pair.first == o.m_pair.first;
    }
    bool operator!=(const iterator &o) const noexcept {
      return m_pair.first != o.m_pair.first;
    }
  };

  MashTable()
      : m_table{new Item *[MIN_TABLE_SIZE]()}, m_length{MIN_TABLE_SIZE},
        m_size{0} {}

  ~MashTable() noexcept {
      doFree(m_table,m_length);
  }

  MashTable(const MashTable&) = delete;
  MashTable& operator=(const MashTable&) = delete;

  int emplace(const K &key, V &&value) noexcept {
    Item *entry{new Item{key, std::move(value)}};
    add(entry, m_table, m_length);
    ++m_size;
    if (m_size > capacity()) {
      resizeTable(m_length << 1);
    }
    return key;
  }

  bool empty() const noexcept { return m_size <= 0; }

  uint32_t size() const noexcept { return m_size; }

  iterator find(const K &key) const noexcept {
    auto kint{static_cast<unsigned int>(key)};
    int hash = kint ^ kint >> 16;
    int index = (m_length - 1) & hash;
    auto entry{m_table[index]};
    while (entry) {
      if (entry->m_key == key) {
        return iterator{entry};
      }
      entry = entry->m_next;
    }
    return iterator{};
  }

  void erase(const iterator &it) {
    if (it.m_pair.first) {
          remove(it.m_pair.first->m_key);
      --m_size;
      if (m_size <= capacity() >> 2)
        resizeTable(m_length >> 1);
    }
  }

  const iterator &end() const {
    static iterator e;
    return e;
  }
};

#endif // MASHTABLE_H
