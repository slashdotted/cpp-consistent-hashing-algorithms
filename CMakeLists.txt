cmake_minimum_required(VERSION 3.15)
project(cpp-consistent-hashing LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disable building shared libraries
set(BUILD_SHARED_LIBS OFF)

# Options
option(WITH_PCG32 "Use PCG32 random number generator" OFF)
option(WITH_HEAPSTATS "Enable heap allocation statistics" ON)

# Find packages
find_package(Boost REQUIRED)
find_package(cxxopts REQUIRED)
find_package(fmt REQUIRED)
find_package(xxHash REQUIRED)
find_package(yaml-cpp REQUIRED)

# Additional include directories
find_path(GTL_INCLUDE_DIRS "gtl/adv_utils.hpp")

if(WITH_PCG32)
    find_path(PCG_INCLUDE_DIRS "pcg_extras.hpp")
    add_definitions(-DUSE_PCG32)
endif()

if(WITH_HEAPSTATS)
    add_definitions(-DUSE_HEAPSTATS)
endif()

# Define sources
set(MAIN_SOURCES
        algorithms/anchor/AnchorHashQre.cpp
        algorithms/anchor/AnchorHashQre.hpp
        algorithms/anchor/anchorengine.h
        algorithms/dx/DxEngine.h
        algorithms/jump/jumpengine.h
        algorithms/memento/mashtable.h
        algorithms/memento/memento.h
        algorithms/memento/mementoengine.h
        algorithms/misc/HashFunctions.h
        algorithms/power/powerengine.h
        benchmarks/Routine.h
        main.cpp
        vcpkg.json
)

# Sort filenames alphabetically
list(SORT MAIN_SOURCES)

# Add executable
add_executable(main ${MAIN_SOURCES})

# Include directories
target_include_directories(main PRIVATE ${GTL_INCLUDE_DIRS})
if(WITH_PCG32)
    target_include_directories(main PRIVATE ${PCG_INCLUDE_DIRS})
endif()

# Link libraries
target_link_libraries(main PRIVATE xxHash::xxhash fmt::fmt cxxopts::cxxopts yaml-cpp)

# Install target
include(GNUInstallDirs)
install(TARGETS main
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
