PROJ_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# Configuration of extension
EXT_NAME=mongo
EXT_CONFIG=${PROJ_DIR}extension_config.cmake

# Include the Makefile from extension-ci-tools
include extension-ci-tools/makefiles/duckdb_extension.Makefile

# Override test target to set MongoDB test environment variable
# This allows tests that require MongoDB to run automatically
# Note: MongoDB must be running and test data must be created (see test/README.md)
.PHONY: test_release_internal
test_release_internal:
	@MONGODB_TEST_DATABASE_AVAILABLE=1 ./build/release/$(TEST_PATH) "$(TESTS_BASE_DIRECTORY)*"

# Cleanup test database files created during testing
# Tests run from duckdb_unittest_tempdir, but cleanup handles both root and tempdir
.PHONY: cleanup-test-files
cleanup-test-files:
	@$(PROJ_DIR)scripts/cleanup_test_files.sh || true