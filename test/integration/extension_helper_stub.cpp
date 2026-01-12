// Stub implementation of ExtensionHelper::LoadAllExtensions for DuckDB main compatibility
// This function is referenced by DuckDB's constructor when load_extensions=true,
// but may not be available in extension builds. This stub provides a minimal implementation
// that loads core_functions if available.

#include "duckdb/main/database.hpp"
#include "duckdb/main/extension_helper.hpp"

namespace duckdb {

void ExtensionHelper::LoadAllExtensions(DuckDB &db) {
	// Stub implementation - try to auto-load core_functions extension if available
	// This avoids linking errors when ExtensionHelper::LoadAllExtensions is not available
	// in DuckDB main extension builds, while still providing basic functionality
	try {
		ExtensionHelper::TryAutoLoadExtension(*db.instance, "core_functions");
	} catch (...) {
		// Ignore exceptions - this is a stub implementation
		// core_functions may not be available or already loaded
	}
}

} // namespace duckdb
