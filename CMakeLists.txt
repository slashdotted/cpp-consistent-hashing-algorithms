cmake_minimum_required(VERSION 3.5)

project(cpp-consistent-hashing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")

option(WITH_PCG32 "Use PCG32 random number generator" OFF)
option(WITH_HEAPSTATS "Enable heap allocation statistics" ON)

find_package(Boost REQUIRED)
find_package(xxHash REQUIRED)
find_package(fmt REQUIRED)
find_package(cxxopts REQUIRED)
find_path(GTL_INCLUDE_DIRS "gtl/adv_utils.hpp")

if(WITH_PCG32)
    find_path(PCG_INCLUDE_DIRS "pcg_extras.hpp")
    add_definitions(-DUSE_PCG32)
endif()

if(WITH_HEAPSTATS)
    add_definitions(-DUSE_HEAPSTATS)
endif()

add_executable(speed_test speed_test.cpp
    vcpkg.json
    memento/memento.h
    memento/mementoengine.h
    anchor/AnchorHashQre.cpp anchor/AnchorHashQre.hpp  anchor/misc/crc32c_sse42_u64.h
    anchor/anchorengine.h
    memento/mashtable.h
    jump/jumpengine.h
    power/powerengine.h
    binomial/binomialengine.h
    utils.h
    utils.cpp
    )

add_executable(balance balance.cpp
    vcpkg.json
    memento/memento.h
    memento/mementoengine.h
    anchor/AnchorHashQre.cpp anchor/AnchorHashQre.hpp  anchor/misc/crc32c_sse42_u64.h
    anchor/anchorengine.h
    memento/mashtable.h
    jump/jumpengine.h
    power/powerengine.h
    binomial/binomialengine.h
    utils.h
    utils.cpp
    )

add_executable(monotonicity monotonicity.cpp
    vcpkg.json
    memento/memento.h
    memento/mementoengine.h
    anchor/AnchorHashQre.cpp anchor/AnchorHashQre.hpp  anchor/misc/crc32c_sse42_u64.h
    anchor/anchorengine.h
    memento/mashtable.h
    jump/jumpengine.h
    power/powerengine.h
    binomial/binomialengine.h
    utils.h
    utils.cpp
    )

add_executable(mashtable_test mashtable_test.cpp memento/mashtable.h)

if(WITH_PCG32)
    target_include_directories(speed_test PRIVATE ${PCG_INCLUDE_DIRS})
    target_include_directories(balance PRIVATE ${PCG_INCLUDE_DIRS})
    target_include_directories(monotonicity PRIVATE ${PCG_INCLUDE_DIRS})
endif()
target_include_directories(speed_test PRIVATE ${GTL_INCLUDE_DIRS})
target_include_directories(balance PRIVATE ${GTL_INCLUDE_DIRS})
target_include_directories(monotonicity PRIVATE ${GTL_INCLUDE_DIRS})
target_link_libraries(speed_test PRIVATE xxHash::xxhash fmt::fmt cxxopts::cxxopts)
target_link_libraries(balance PRIVATE xxHash::xxhash fmt::fmt cxxopts::cxxopts)
target_link_libraries(monotonicity PRIVATE xxHash::xxhash fmt::fmt cxxopts::cxxopts)
include(GNUInstallDirs)
install(TARGETS speed_test
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(TARGETS balance
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(TARGETS monotonicity
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
