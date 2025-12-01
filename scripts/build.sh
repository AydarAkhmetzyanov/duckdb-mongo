#!/bin/bash
# Build script that sets up vcpkg environment correctly

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJ_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJ_DIR"

# Check for vcpkg - use absolute paths
if [ -z "$VCPKG_TOOLCHAIN_PATH" ]; then
    # Try to find vcpkg and convert to absolute path
    if [ -f "../vcpkg/scripts/buildsystems/vcpkg.cmake" ]; then
        # Convert relative path to absolute
        VCPKG_DIR="$(cd "$PROJ_DIR/../vcpkg" && pwd)"
        export VCPKG_TOOLCHAIN_PATH="$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake"
    elif [ -f "$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake" ]; then
        export VCPKG_TOOLCHAIN_PATH="$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake"
    else
        echo "Error: VCPKG_TOOLCHAIN_PATH not set and vcpkg not found in common locations"
        echo "Please set VCPKG_TOOLCHAIN_PATH or install vcpkg"
        exit 1
    fi
fi

# Ensure absolute path (in case user provided relative path)
if [[ "$VCPKG_TOOLCHAIN_PATH" != /* ]]; then
    # Convert relative path to absolute
    export VCPKG_TOOLCHAIN_PATH="$(cd "$(dirname "$VCPKG_TOOLCHAIN_PATH")" && pwd)/$(basename "$VCPKG_TOOLCHAIN_PATH")"
fi

# Determine triplet
if [[ "$OSTYPE" == "darwin"* ]]; then
    if [[ $(uname -m) == "arm64" ]]; then
        export VCPKG_TARGET_TRIPLET="arm64-osx"
    else
        export VCPKG_TARGET_TRIPLET="x64-osx"
    fi
else
    export VCPKG_TARGET_TRIPLET="x64-linux"
fi

echo "Using vcpkg toolchain: $VCPKG_TOOLCHAIN_PATH"
echo "Using triplet: $VCPKG_TARGET_TRIPLET"
echo ""

# Build
make release

echo ""
echo "✓ Build completed successfully!"

