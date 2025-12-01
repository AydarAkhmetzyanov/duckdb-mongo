#!/bin/bash
# Script to run tests with MongoDB Docker container
# This sets up MongoDB, creates test data, and runs the tests

set -e

echo "=== Setting up MongoDB test environment ==="

# Check if Docker is available
if ! command -v docker &> /dev/null; then
    echo "Error: Docker is not installed. Please install Docker."
    exit 1
fi

# Start MongoDB container
echo "Starting MongoDB container..."
if docker ps -a | grep -q mongodb-test; then
    echo "Container exists, starting it..."
    docker start mongodb-test || true
else
    echo "Creating new MongoDB container..."
    docker run -d -p 27017:27017 --name mongodb-test mongo:latest
fi

# Wait for MongoDB to be ready
echo "Waiting for MongoDB to be ready..."
sleep 5

# Check if MongoDB is responding
if ! docker exec mongodb-test mongosh --eval "db.adminCommand('ping')" &> /dev/null; then
    echo "Waiting a bit more for MongoDB to start..."
    sleep 5
fi

# Create test data
echo "Creating test data..."
bash test/create-mongo-tables.sh

# Set environment variable
export MONGODB_TEST_DATABASE_AVAILABLE=1

# Always rebuild to ensure latest code is tested
echo ""
echo "=== Building extension ==="

# Check vcpkg setup
if ! bash scripts/check-vcpkg.sh 2>/dev/null; then
    echo "vcpkg setup issue detected. Run 'bash scripts/check-vcpkg.sh' for details."
    echo ""
    echo "Please build the extension manually:"
    echo "  1. Ensure vcpkg is set up (see README.md or run: bash scripts/check-vcpkg.sh)"
    echo "  2. Set VCPKG_TOOLCHAIN_PATH if needed"
    echo "  3. Run: bash scripts/build.sh"
    exit 1
fi

# Build using the build script
if ! bash scripts/build.sh; then
    echo ""
    echo "Build failed. Please check the errors above and build manually:"
    echo "  bash scripts/build.sh"
    exit 1
fi

# Run tests
# Tests will run from duckdb_unittest_tempdir (configured in Makefile)
# This ensures any database files created by ATTACH are contained in that directory
echo ""
echo "=== Running tests ==="
make test

echo ""
echo "=== Tests completed ==="

# Cleanup test database files
echo "Cleaning up test database files..."
bash scripts/cleanup_test_files.sh

echo ""
echo "To stop MongoDB container: docker stop mongodb-test"
echo "To remove MongoDB container: docker rm mongodb-test"

