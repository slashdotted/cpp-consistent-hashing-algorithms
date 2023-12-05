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
#include <string>
#include "memento/mashtable.h"
#include "cassert"

int main() {
    MashTable<uint32_t,std::string> table;
    for(auto z=0; z<10000000; ++z) {
        table.emplace(z, std::to_string(z));
    }
    for(auto z=0; z<10000000; ++z) {
        auto i{table.find(z)};
        assert(i->second == std::to_string(z));
    }

}
