#!/bin/bash
echo "Cleaning up SandBoxX temporary files..."
rm -rf build/
rm -rf runtime/sandboxes/*
rm -rf runtime/logs/*
touch runtime/sandboxes/.gitkeep
touch runtime/logs/.gitkeep
echo "Cleanup completed."
