#!/bin/bash

# Ensure script has executable permissions:
# chmod +x setup.sh

# Update submodules
# Change directory to vcpkg
# Bootstrap vcpkg using system binaries
git submodule update --init
cd vcpkg || exit
./bootstrap-vcpkg.sh -useSystemBinaries
