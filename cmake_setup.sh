#!/bin/bash

# Ensure script has executable permissions:
# chmod +x setup.sh

# Check if "build" directory exists
# If exists, remove it recursively
if [ -d "build" ]; then
    rm -r build
fi

# Configure project with CMake
cmake -B build/ -S . -GNinja -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
