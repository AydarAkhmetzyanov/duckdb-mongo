#!/bin/bash
# Script to update submodules to latest and rebuild the extension

set -e

echo "=== Updating Submodules ==="
echo ""

# Initialize submodules if needed
echo "Step 1: Initializing submodules..."
git submodule update --init --recursive

echo ""
echo "Step 2: Updating submodules to latest..."
# Update each submodule to latest commit
git submodule foreach 'git fetch origin && git checkout main || git checkout master || git checkout $(git rev-parse --abbrev-ref HEAD)'

# Update submodule references
git submodule update --remote --recursive

echo ""
echo "=== Submodules Updated ==="
echo ""

# Check vcpkg setup
echo "=== Checking vcpkg Setup ==="
if ! bash scripts/check-vcpkg.sh 2>/dev/null; then
    echo "⚠️  vcpkg setup issue detected. Please check vcpkg configuration."
    echo ""
fi

echo ""
echo "=== Cleaning Previous Build ==="
make clean || true

echo ""
echo "=== Building Extension ==="
bash scripts/build.sh

echo ""
echo "✅ Update and rebuild complete!"

