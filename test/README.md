# Testing this extension
This directory contains all the tests for this extension. The `sql` directory holds tests that are written as [SQLLogicTests](https://duckdb.org/dev/sqllogictest/intro.html). DuckDB aims to have most its tests in this format as SQL statements, so for the mongo extension, this should probably be the goal too.

The root makefile contains targets to build and run all of these tests. To run the SQLLogicTests:
```bash
make test
```
or 
```bash
make test_debug
```

## Setting Up Test MongoDB Database

Many tests require a MongoDB instance with test data. The `make test` command automatically sets `MONGODB_TEST_DATABASE_AVAILABLE=1`, but you still need MongoDB running and test data created.

### Automated Setup (Recommended)

The easiest way to run tests with MongoDB is using the automated script:

```bash
bash test/run-tests-with-mongo.sh
```

This script will:
1. Start a MongoDB Docker container (if not already running)
2. Create test data
3. Run all tests

### Manual Setup

If you want to run `make test` directly, you need to:

1. **Start MongoDB** (if not already running):
   ```bash
   # Using Docker
   docker run -d -p 27017:27017 --name mongodb-test mongo:latest
   
   # Or use an existing MongoDB instance
   ```

2. **Create test data**:
   ```bash
   ./test/create-mongo-tables.sh
   ./test/create-tpch-test-db.sh  # Creates TPC-H test database (sf=0.01)
   ```

3. **Run tests**:
   ```bash
   make test
   ```
   
   The `MONGODB_TEST_DATABASE_AVAILABLE` environment variable is automatically set by the Makefile.

The setup script creates two databases:

1. **`duckdb_mongo_test`** - General test database with the following collections:
   - `users` - Sample user data with various types (strings, numbers, booleans, dates, nested objects, arrays)
   - `products` - Product data with nested specs
   - `orders` - Order data with nested arrays
   - `empty_collection` - Empty collection for edge case testing
   - `type_conflicts` - Collection with type conflicts
   - `deeply_nested` - Collection with deeply nested documents

2. **`tpch_test`** - TPC-H test database (always scale factor 0.01) for unit tests:
   - Contains all 8 TPC-H tables (region, nation, supplier, customer, part, partsupp, orders, lineitem)
   - Always uses scale factor 0.01 (~60K lineitems) to match expected test results
   - Separate from `tpch` database used for benchmarks (which can be any scale factor)

Tests that require MongoDB use `require-env MONGODB_TEST_DATABASE_AVAILABLE` and will be skipped if the environment variable is not set.

## Cleanup Test Files

Tests run from `duckdb_unittest_tempdir/` to contain test database files created by `ATTACH` commands. These files are automatically ignored by git (see `.gitignore`). 

The automated test script (`test/run-tests-with-mongo.sh`) automatically cleans up these files after running tests. You can also clean them manually:

```bash
make cleanup-test-files
```

Or directly:
```bash
bash scripts/cleanup_test_files.sh
```
## C++ Integration Tests

In addition to SQL logic tests, this extension includes C++ integration tests for testing connectivity to real MongoDB instances, including MongoDB Atlas.

### MongoDB Atlas Integration Test

The Atlas integration test (`test/integration/test_atlas_integration.cpp`) tests connectivity and basic operations against a MongoDB Atlas cluster. This test is useful for verifying that the extension works correctly with cloud-hosted MongoDB instances.

#### Prerequisites

1. A MongoDB Atlas account with a cluster
2. Atlas credentials (username and password)
3. Network access configured to allow connections from your IP

#### Running the Atlas Integration Test

The easiest way to run the Atlas integration test is using the provided script:

```bash
# Set your Atlas credentials
export MONGO_ATLAS_USERNAME='your-username'
export MONGO_ATLAS_PASSWORD='your-password'

# Run the integration test
bash test/run-atlas-integration-test.sh
```

The script will:
1. Check for required environment variables
2. Build the extension with C++ unit tests enabled (if not already built)
3. Run the Atlas integration test
4. Report the results

#### Manual Build and Run

If you prefer to build and run manually:

1. **Build with C++ unit tests enabled**:
   ```bash
   # The extension Makefile hardcodes ENABLE_UNITTEST_CPP_TESTS=FALSE
   # so we need to use CMake directly to enable it
   mkdir -p build/release
   cd build/release
   cmake -DENABLE_UNITTEST_CPP_TESTS=TRUE -GNinja ../..
   ninja test_atlas_integration
   cd ../..
   ```

2. **Set environment variables**:
   ```bash
   export MONGO_ATLAS_USERNAME='your-username'
   export MONGO_ATLAS_PASSWORD='your-password'
   ```

3. **Run the test executable**:
   ```bash
   ./build/release/extension/mongo/test_atlas_integration "[mongo][atlas][integration]"
   ```

#### Test Coverage

The Atlas integration test covers:
- ATTACH command with MongoDB Atlas connection string (mongodb+srv://)
- Database listing and verification
- Schema enumeration
- Table (collection) listing
- Basic SELECT queries
- mongo_scan table function
- DETACH command

#### Skipping the Test

If the required environment variables (`MONGO_ATLAS_USERNAME` and `MONGO_ATLAS_PASSWORD`) are not set, the test will be automatically skipped without failing.

#### Adding More Integration Tests

To add additional C++ integration tests:

1. Create a new `.cpp` file in `test/integration/`
2. Use the Catch2 test framework (same as DuckDB core tests)
3. Include appropriate test tags for filtering
4. Update `CMakeLists.txt` if needed to add the new test executable

Example test structure:
```cpp
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "duckdb.hpp"

// Helper macro for checking query success
#define REQUIRE_NO_FAIL(result) REQUIRE(!(result)->HasError())

// Simple main function for running the test
int main(int argc, char* argv[]) {
    return Catch::Session().run(argc, argv);
}

TEST_CASE("My Integration Test", "[mongo][integration]") {
    // Your test code here
}
```
