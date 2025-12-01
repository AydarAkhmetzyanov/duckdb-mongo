#!/bin/bash
# Cleanup test database files created during testing
# Tests run from duckdb_unittest_tempdir, but files may also be created in root

cd "$(dirname "$0")/.." || exit 1

# Remove DuckDB database files from root (legacy - tests now run from tempdir)
find . -maxdepth 1 -name "host=*" -type f -delete 2>/dev/null
find . -maxdepth 1 -name "*.db" -type f -delete 2>/dev/null

# Remove files from test temp directory (where tests actually run)
if [ -d "duckdb_unittest_tempdir" ]; then
    find duckdb_unittest_tempdir -maxdepth 1 -name "host=*" -type f -delete 2>/dev/null
    find duckdb_unittest_tempdir -maxdepth 1 -name "*.db" -type f -delete 2>/dev/null
fi

echo "Cleaned up test database files"

