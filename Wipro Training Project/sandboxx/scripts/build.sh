#!/bin/bash
set -e

echo "Building SandBoxX..."
mkdir -p build
cd build
cmake ..
make -j$(nproc)
ctest --output-on-failure
echo "SandBoxX Build completed successfully!"
