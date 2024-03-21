#!/bin/bash

# Ensure script has executable permissions:
# chmod +x repo.sh

git submodule update --init
cd vcpkg || exit
./bootstrap-vcpkg.sh -useSystemBinaries
