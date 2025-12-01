#!/bin/bash
# Helper script to check and set up vcpkg for the build

# Check if VCPKG_TOOLCHAIN_PATH is set
if [ -n "$VCPKG_TOOLCHAIN_PATH" ] && [ -f "$VCPKG_TOOLCHAIN_PATH" ]; then
    echo "✓ vcpkg toolchain found at: $VCPKG_TOOLCHAIN_PATH"
    exit 0
fi

# Try to find vcpkg in common locations
COMMON_LOCATIONS=(
    "$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake"
    "$HOME/.vcpkg/scripts/buildsystems/vcpkg.cmake"
    "/usr/local/share/vcpkg/scripts/buildsystems/vcpkg.cmake"
    "/opt/vcpkg/scripts/buildsystems/vcpkg.cmake"
)

for location in "${COMMON_LOCATIONS[@]}"; do
    if [ -f "$location" ]; then
        echo "Found vcpkg at: $location"
        echo ""
        echo "To use it, run:"
        echo "  export VCPKG_TOOLCHAIN_PATH=\"$location\""
        echo "  make release"
        exit 1
    fi
done

# vcpkg not found
echo "✗ vcpkg not found"
echo ""
echo "To set up vcpkg:"
echo "  1. Clone vcpkg:"
echo "     git clone https://github.com/Microsoft/vcpkg.git ~/vcpkg"
echo ""
echo "  2. Bootstrap vcpkg:"
echo "     cd ~/vcpkg"
echo "     ./bootstrap-vcpkg.sh"
echo ""
echo "  3. Set the toolchain path:"
echo "     export VCPKG_TOOLCHAIN_PATH=~/vcpkg/scripts/buildsystems/vcpkg.cmake"
echo ""
echo "  4. Then build:"
echo "     make release"
echo ""
exit 1

