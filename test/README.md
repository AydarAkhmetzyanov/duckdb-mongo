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
   ```

3. **Run tests**:
   ```bash
   make test
   ```
   
   The `MONGODB_TEST_DATABASE_AVAILABLE` environment variable is automatically set by the Makefile.

The setup script creates a database `duckdb_mongo_test` with the following collections:
- `users` - Sample user data with various types (strings, numbers, booleans, dates, nested objects, arrays)
- `products` - Product data with nested specs
- `orders` - Order data with nested arrays
- `empty_collection` - Empty collection for edge case testing
- `type_conflicts` - Collection with type conflicts
- `deeply_nested` - Collection with deeply nested documents

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