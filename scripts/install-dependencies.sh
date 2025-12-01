#!/bin/bash
# Script to install vcpkg dependencies for the MongoDB extension

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJ_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

# Find vcpkg
VCPKG_PATH=""
if [ -n "$VCPKG_TOOLCHAIN_PATH" ]; then
    # Extract vcpkg root from toolchain path
    VCPKG_ROOT=$(dirname $(dirname $(dirname "$VCPKG_TOOLCHAIN_PATH")))
    if [ -f "$VCPKG_ROOT/vcpkg" ]; then
        VCPKG_PATH="$VCPKG_ROOT/vcpkg"
    fi
fi

# Try common locations
if [ -z "$VCPKG_PATH" ]; then
    COMMON_LOCATIONS=(
        "$HOME/vcpkg/vcpkg"
        "$HOME/.vcpkg/vcpkg"
        "../vcpkg/vcpkg"
        "../../vcpkg/vcpkg"
    )
    
    for location in "${COMMON_LOCATIONS[@]}"; do
        if [ -f "$location" ]; then
            VCPKG_PATH="$location"
            break
        fi
    done
fi

if [ -z "$VCPKG_PATH" ] || [ ! -f "$VCPKG_PATH" ]; then
    echo "Error: vcpkg not found"
    echo ""
    echo "Please ensure vcpkg is installed and VCPKG_TOOLCHAIN_PATH is set correctly."
    echo "Or install vcpkg:"
    echo "  git clone https://github.com/Microsoft/vcpkg.git ~/vcpkg"
    echo "  cd ~/vcpkg"
    echo "  ./bootstrap-vcpkg.sh"
    exit 1
fi

echo "Found vcpkg at: $VCPKG_PATH"
echo ""

# Determine triplet
if [[ "$OSTYPE" == "darwin"* ]]; then
    if [[ $(uname -m) == "arm64" ]]; then
        TRIPLET="arm64-osx"
    else
        TRIPLET="x64-osx"
    fi
else
    TRIPLET="x64-linux"
fi

echo "Installing dependencies from vcpkg.json for triplet: $TRIPLET"
echo "This may take several minutes (10-20 minutes for first build)..."
echo ""

cd "$PROJ_DIR"
# In manifest mode, vcpkg install installs all packages from vcpkg.json
# We can specify the triplet but not individual packages
"$VCPKG_PATH" install --triplet "$TRIPLET"

echo ""
echo "✓ Dependencies installed successfully!"
echo ""
echo "You can now build the extension:"
echo "  make release"

