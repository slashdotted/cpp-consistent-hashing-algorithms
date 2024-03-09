#!/bin/bash

# Ensure script has executable permissions:
# chmod +x setup.sh

# Check if "build" directory exists
if [ -d "build" ]; then
    # If exists, remove it recursively
    rm -r build
fi

# Configure project with CMake
cmake -B build/ -S . -GNinja -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release

# Move to build directory
cd build || exit

# Build project with Ninja
ninja
