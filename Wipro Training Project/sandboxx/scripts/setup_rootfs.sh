#!/bin/bash
set -e

echo "Setting up SandBoxX base rootfs structure..."
mkdir -p rootfs/{bin,lib,lib64,usr,tmp,proc,dev,sys,etc}

# Copy essential shell binaries if present
if [ -f /bin/bash ]; then
    cp /bin/bash rootfs/bin/ 2>/dev/null || true
fi
if [ -f /bin/ls ]; then
    cp /bin/ls rootfs/bin/ 2>/dev/null || true
fi

echo "Rootfs directory structure prepared."
