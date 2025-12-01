#pragma once

#include "duckdb.hpp"
#include "duckdb/catalog/duck_catalog.hpp"
#include "duckdb/catalog/catalog_entry/schema_catalog_entry.hpp"
#include "mongo_instance.hpp"
#include <mongocxx/client.hpp>
#include <string>

namespace duckdb {

class MongoCatalog : public DuckCatalog {
public:
	explicit MongoCatalog(AttachedDatabase &db, const string &connection_string, const string &database_name = "");
	
	string connection_string;
	string database_name; // Specific database to use (empty means all databases)
	
	// Override to list MongoDB databases as schemas
	void ScanSchemas(ClientContext &context, std::function<void(SchemaCatalogEntry &)> callback) override;
	
	// Override LookupSchema to ensure schemas are found correctly
	optional_ptr<SchemaCatalogEntry> LookupSchema(CatalogTransaction transaction, const EntryLookupInfo &schema_lookup,
	                                              OnEntryNotFound if_not_found) override;
	
	// Get default schema name (when dbname is specified, return the database name as the default schema)
	string GetDefaultSchema() const override {
		// When a specific database is specified, return that database name as the schema name
		// This allows queries like mongo_test.users to work (where mongo_test is the attached DB)
		// and users is the collection, which will be found in the schema named after the MongoDB database
		return database_name.empty() ? "" : database_name;
	}
	
	// Get MongoDB client
	mongocxx::client GetClient() const {
		GetMongoInstance(); // Ensure instance is initialized
		return mongocxx::client(mongocxx::uri(connection_string));
	}
};

} // namespace duckdb

