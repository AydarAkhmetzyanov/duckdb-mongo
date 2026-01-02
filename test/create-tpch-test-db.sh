#!/bin/bash
# Script to generate TPC-H test database (always sf=0.01) for unit tests
# This is separate from benchmarks/create-tpch-mongo.sh which is for benchmarks
# and can use any scale factor.

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJ_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

MONGO_HOST=${MONGO_HOST:-localhost}
MONGO_PORT=${MONGO_PORT:-27017}
MONGO_DB=${MONGO_DB:-tpch_test}
# Test database always uses sf=0.01 for unit tests
SCALE_FACTOR=0.01

echo "=== Generating TPC-H test database (scale factor: $SCALE_FACTOR) ==="
echo "Database: $MONGO_DB"
echo "This database is used for unit tests and always uses sf=0.01"
echo ""

# Reuse the benchmark script but override the database name and scale factor
cd "$PROJ_DIR"
MONGO_DB=$MONGO_DB SCALE_FACTOR=$SCALE_FACTOR bash benchmarks/create-tpch-mongo.sh

echo ""
echo "=== TPC-H test database created successfully ==="
echo "Database: $MONGO_DB"
echo "Scale factor: $SCALE_FACTOR"
echo ""
echo "You can now run unit tests with:"
echo "  MONGODB_TEST_DATABASE_AVAILABLE=1 make test_release"

