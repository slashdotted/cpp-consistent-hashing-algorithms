#!/bin/bash

# Ensure script has executable permissions:
# chmod +x cmake.sh

if [ -d "build" ]; then
    rm -r build
fi

cmake -B build -S . -GNinja -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
