#include "mongo_catalog.hpp"
#include "mongo_instance.hpp"
#include "mongo_schema_entry.hpp"
#include "duckdb/catalog/catalog_entry/schema_catalog_entry.hpp"
#include "duckdb/catalog/catalog_entry/view_catalog_entry.hpp"
#include "duckdb/parser/parsed_data/create_view_info.hpp"
#include "duckdb/parser/parsed_data/create_schema_info.hpp"
#include "duckdb/parser/parsed_data/drop_info.hpp"
#include "duckdb/planner/operator/logical_create_table.hpp"
#include "duckdb/planner/operator/logical_insert.hpp"
#include "duckdb/planner/operator/logical_delete.hpp"
#include "duckdb/planner/operator/logical_update.hpp"
#include "duckdb/main/attached_database.hpp"
#include "duckdb/common/mutex.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/unique_ptr.hpp"
#include <bsoncxx/builder/basic/document.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/client.hpp>
#include <chrono>
#include <iostream>

namespace duckdb {

// Forward declaration
class MongoCatalog;

// Default generator for MongoDB collections (creates views dynamically).
class MongoCollectionGenerator : public DefaultGenerator {
public:
	MongoCollectionGenerator(Catalog &catalog, SchemaCatalogEntry &schema, const string &connection_string_p,
	                         const string &database_name_p, MongoCatalog *mongo_catalog_p = nullptr)
	    : DefaultGenerator(catalog), schema(schema), connection_string(connection_string_p),
	      database_name(database_name_p), collections_loaded(false), mongo_catalog(mongo_catalog_p) {
		GetMongoInstance();
		// Pre-warm connection to reduce latency on first SHOW TABLES
		try {
			GetOrCreateClient();
		} catch (...) {
			// Connection will be retried when actually needed
		}
	}

public:
	unique_ptr<CatalogEntry> CreateDefaultEntry(ClientContext &context, const string &entry_name) override {
		EnsureCollectionsLoaded();
		for (const auto &collection_name : collection_names) {
			if (StringUtil::CIEquals(entry_name, collection_name)) {
				return CreateEntryForCollection(context, collection_name);
			}
		}
		return nullptr;
	}

	vector<string> GetDefaultEntries() override {
		EnsureCollectionsLoaded();
		return collection_names;
	}

	unique_ptr<CatalogEntry> CreateEntryForCollection(ClientContext &context, const string &collection_name) {
		auto result = make_uniq<CreateViewInfo>();
		result->schema = schema.name;
		result->view_name = collection_name;
		
		auto escape_sql_string = [](const string &str) -> string {
			string result;
			for (char c : str) {
				if (c == '\'') {
					result += "''";
				} else {
					result += c;
				}
			}
			return result;
		};
		
		if (cached_escaped_connection_string.empty()) {
			cached_escaped_connection_string = escape_sql_string(connection_string);
			cached_escaped_database_name = escape_sql_string(database_name);
		}
		string escaped_collection_name = escape_sql_string(collection_name);
		
		result->sql = StringUtil::Format("SELECT * FROM mongo_scan('%s', '%s', '%s')",
		                                 cached_escaped_connection_string, cached_escaped_database_name,
		                                 escaped_collection_name);
		auto start_parse = std::chrono::high_resolution_clock::now();
		auto view_info = CreateViewInfo::FromSelect(context, std::move(result));
		auto end_parse = std::chrono::high_resolution_clock::now();
		auto parse_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_parse - start_parse).count();
		if (parse_ms > 10) {
			std::cerr << "[MongoCollectionGenerator] CreateEntryForCollection('" << collection_name << "') SQL parsing took " << parse_ms << "ms" << std::endl;
		}
		auto start_create = std::chrono::high_resolution_clock::now();
		auto entry = make_uniq_base<CatalogEntry, ViewCatalogEntry>(catalog, schema, *view_info);
		auto end_create = std::chrono::high_resolution_clock::now();
		auto create_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_create - start_create).count();
		if (create_ms > 10) {
			std::cerr << "[MongoCollectionGenerator] CreateEntryForCollection('" << collection_name << "') ViewCatalogEntry creation took " << create_ms << "ms" << std::endl;
		}
		return entry;
	}

private:
	mongocxx::client& GetOrCreateClient() {
		if (!cached_client || cached_connection_string != connection_string) {
			auto start_conn = std::chrono::high_resolution_clock::now();
			string conn_str = connection_string;
			bool has_query_params = conn_str.find('?') != string::npos;
			
			if (conn_str.find("connectTimeoutMS") == string::npos) {
				if (!has_query_params) {
					conn_str += "?connectTimeoutMS=10000";
					has_query_params = true;
				} else {
					conn_str += "&connectTimeoutMS=10000";
				}
			}
			if (conn_str.find("serverSelectionTimeoutMS") == string::npos) {
				if (!has_query_params) {
					conn_str += "?serverSelectionTimeoutMS=10000";
				} else {
					conn_str += "&serverSelectionTimeoutMS=10000";
				}
			}
			
			mongocxx::uri uri(conn_str);
			cached_client = make_uniq<mongocxx::client>(uri);
			cached_connection_string = connection_string;
			auto end_conn = std::chrono::high_resolution_clock::now();
			auto conn_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_conn - start_conn).count();
			std::cerr << "[MongoCollectionGenerator] MongoDB connection creation took " << conn_ms << "ms" << std::endl;
		}
		return *cached_client;
	}

	void EnsureCollectionsLoaded() {
		if (collections_loaded) {
			return;
		}
		
		if (mongo_catalog) {
			auto cached = mongo_catalog->GetCachedCollectionNames(database_name);
			if (!cached.empty()) {
				collection_names = cached;
				collections_loaded = true;
				std::cerr << "[MongoCollectionGenerator] Using cached collection names for database '" << database_name << "' (" << cached.size() << " collections)" << std::endl;
				return;
			}
		}
		
		collections_loaded = true;

		try {
			auto& client = GetOrCreateClient();
			auto mongo_db = client[database_name];
			auto start_list = std::chrono::high_resolution_clock::now();
			auto collections = mongo_db.list_collection_names();
			auto end_list = std::chrono::high_resolution_clock::now();
			auto list_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_list - start_list).count();
			std::cerr << "[MongoCollectionGenerator] list_collection_names() took " << list_ms << "ms" << std::endl;
			
			vector<string> filtered_collections;
			for (const auto &collection : collections) {
				if (StringUtil::StartsWith(collection, "system.")) {
					continue;
				}
				filtered_collections.push_back(collection);
				collection_names.push_back(collection);
			}
			
			if (mongo_catalog && !filtered_collections.empty()) {
				mongo_catalog->CacheCollectionNames(database_name, filtered_collections);
			}
		} catch (...) {
			// Leave collection_names empty on error
		}
	}

	SchemaCatalogEntry &schema;
	string connection_string;
	string database_name;
	vector<string> collection_names;
	bool collections_loaded;
	MongoCatalog *mongo_catalog; // Reference to catalog for shared caching
	// Cache the MongoDB client connection to avoid recreating it
	unique_ptr<mongocxx::client> cached_client;
	string cached_connection_string;
	// Cache escaped strings to avoid re-escaping on every entry creation
	string cached_escaped_connection_string;
	string cached_escaped_database_name;
};

MongoCatalog::MongoCatalog(AttachedDatabase &db, const string &connection_string, const string &database_name)
    : Catalog(db), connection_string(connection_string), database_name(database_name), schemas_scanned(false) {
	// Ensure MongoDB instance is initialized.
	GetMongoInstance();
}

void MongoCatalog::Initialize(bool load_builtin) {
	// No built-in initialization needed for MongoDB catalog
}

optional_ptr<CatalogEntry> MongoCatalog::CreateSchema(CatalogTransaction transaction, CreateSchemaInfo &info) {
	lock_guard<mutex> lock(schemas_lock);
	
	// Check if schema already exists
	auto it = schemas.find(info.schema);
	if (it != schemas.end()) {
		switch (info.on_conflict) {
		case OnCreateConflict::ERROR_ON_CONFLICT:
			throw CatalogException::EntryAlreadyExists(CatalogType::SCHEMA_ENTRY, info.schema);
		case OnCreateConflict::REPLACE_ON_CONFLICT:
			// Remove existing schema
			schemas.erase(it);
			break;
		case OnCreateConflict::IGNORE_ON_CONFLICT:
			return it->second.get();
		default:
			throw InternalException("Unsupported OnCreateConflict for CreateSchema");
		}
	}

	// Create new schema entry
	auto schema_entry = make_uniq<MongoSchemaEntry>(*this, info);
	auto result = schema_entry.get();
	schemas[info.schema] = shared_ptr<MongoSchemaEntry>(schema_entry.release());
	
	return result;
}

void MongoCatalog::ScanSchemas(ClientContext &context, std::function<void(SchemaCatalogEntry &)> callback) {
	// List MongoDB databases as schemas.
	// Let exceptions propagate - if we can't connect or list databases during ATTACH,
	// we should fail fast with a clear error message rather than deferring the error.
	
	// Mark as scanned to prevent recursive calls
	{
		lock_guard<mutex> lock(schemas_lock);
		if (schemas_scanned) {
			// Already scanned, just call callback for existing schemas
			for (auto &[name, schema] : schemas) {
				callback(*schema);
			}
			return;
		}
		schemas_scanned = true;
	}
	
	auto client = GetClient();
	vector<string> databases;

		// If a specific database is specified, use the default schema (empty string = main/default).
		// Otherwise, list all databases as separate schemas.
		if (!database_name.empty()) {
			// Verify the database exists by trying to list its collections.
			auto mongo_db = client[database_name];
			auto collections = mongo_db.list_collection_names();
			// Use empty string to indicate default schema.
			databases.push_back("");
		} else {
			// List all databases.
			auto db_list = client.list_database_names();
			for (const auto &db_name : db_list) {
				databases.push_back(db_name);
			}
		}

		auto system_transaction = CatalogTransaction::GetSystemTransaction(GetDatabase());

		// Track first non-system schema for default
		string first_non_system_schema;

		for (const auto &schema_name : databases) {
			// Skip system databases (unless specifically requested via dbname)
			if (database_name.empty() &&
			    (schema_name == "admin" || schema_name == "local" || schema_name == "config")) {
				continue;
			}

			// Create or get schema
			// If schema_name is empty and dbname is specified, use the database name as schema name
			// This ensures the schema is created in our catalog and is unique
			string actual_schema_name;
			if (schema_name.empty() && !database_name.empty()) {
				// When dbname is specified, use the database name as the schema name
				// This ensures the schema is in our MongoCatalog and collections are accessible
				actual_schema_name = database_name;
			} else if (schema_name.empty()) {
				// No dbname specified and empty schema - shouldn't happen, but use "main" as fallback
				actual_schema_name = "main";
			} else {
				actual_schema_name = schema_name;
			}
			
			// Track first non-system schema for default
			if (first_non_system_schema.empty() && 
			    actual_schema_name != "admin" && actual_schema_name != "local" && actual_schema_name != "config") {
				first_non_system_schema = actual_schema_name;
			}

			CreateSchemaInfo schema_info;
			schema_info.schema = actual_schema_name;
			schema_info.on_conflict = OnCreateConflict::IGNORE_ON_CONFLICT;
			auto schema_entry = CreateSchema(system_transaction, schema_info);
			if (schema_entry) {
				auto &schema = schema_entry->Cast<MongoSchemaEntry>();
				// Set up default generator for collections in this schema
				// When dbname is specified, use that database name for the generator
				string generator_db_name = database_name.empty() ? schema_name : database_name;

				// Always set the default generator, even if schema already existed.
				// This ensures collections are accessible.
				auto default_generator =
				    make_uniq<MongoCollectionGenerator>(*this, schema, connection_string, generator_db_name, this);
				schema.SetDefaultGenerator(std::move(default_generator));

				callback(schema);
			}
		}
		
		// Set default schema (similar to Postgres extension)
		// Do this outside the schemas_lock to avoid deadlock
		if (default_schema.empty()) {
			if (!database_name.empty()) {
				// When database_name is specified, use it as default schema
				default_schema = database_name;
			} else if (!first_non_system_schema.empty()) {
				// Otherwise, use first non-system schema
				default_schema = first_non_system_schema;
			}
		}
	}

optional_ptr<SchemaCatalogEntry> MongoCatalog::LookupSchema(CatalogTransaction transaction,
                                                            const EntryLookupInfo &schema_lookup,
                                                            OnEntryNotFound if_not_found) {
	auto schema_name = schema_lookup.GetEntryName();

	// If looking for default schema, use GetDefaultSchema()
	if (schema_name.empty()) {
		schema_name = GetDefaultSchema();
	}
	
	// Special case: when database_name is specified and user tries to USE the database name as schema,
	// we should use the MongoDB database name as the schema (which is what we actually created during ScanSchemas)
	// This handles cases where the schema was created with the MongoDB database name
	if (!schema_name.empty() && !database_name.empty()) {
		// First try to find schema with the MongoDB database name (most common case)
		lock_guard<mutex> lock(schemas_lock);
		auto it = schemas.find(database_name);
		if (it != schemas.end()) {
			return it->second.get();
		}
		// If not found, unlock and continue with original schema_name lookup
	}

	// Try to find the schema first.
	optional_ptr<SchemaCatalogEntry> schema;
	{
		lock_guard<mutex> lock(schemas_lock);
		auto it = schemas.find(schema_name);
		schema = it != schemas.end() ? it->second.get() : nullptr;
	}

	// If schema not found, try to create it on-demand.
	// When no specific database_name is configured, any schema_name can be a valid MongoDB database.
	if (!schema && !schema_name.empty()) {
		try {
			// Determine which MongoDB database this schema corresponds to.
			string mongo_db_name;
			if (!database_name.empty()) {
				// Only create schema for the specific database we're attached to.
				if (schema_name == database_name) {
					mongo_db_name = database_name;
				}
			} else {
				// No specific database - schema_name is treated as a MongoDB database name.
				// Create schema lazily; errors will occur when querying if database doesn't exist.
				mongo_db_name = schema_name;
			}

			if (!mongo_db_name.empty()) {
				CreateSchemaInfo schema_info;
				schema_info.schema = schema_name;
				schema_info.on_conflict = OnCreateConflict::IGNORE_ON_CONFLICT;
				auto system_transaction = CatalogTransaction::GetSystemTransaction(GetDatabase());
				auto schema_entry = CreateSchema(system_transaction, schema_info);
				if (schema_entry) {
					auto &schema_ref = schema_entry->Cast<MongoSchemaEntry>();
					// Set up default generator for collections in this schema.
					auto default_generator =
					    make_uniq<MongoCollectionGenerator>(*this, schema_ref, connection_string, mongo_db_name, this);
					schema_ref.SetDefaultGenerator(std::move(default_generator));
					schema = &schema_ref;
				}
			}
		} catch (const std::exception &e) {
			// If we can't create the schema, fall through to return null or throw.
		}
	}

	if (!schema && if_not_found != OnEntryNotFound::RETURN_NULL) {
		throw BinderException("Schema with name \"%s\" not found", schema_name);
	}

	return schema;
}

PhysicalOperator &MongoCatalog::PlanCreateTableAs(ClientContext &context, PhysicalPlanGenerator &planner,
                                                  LogicalCreateTable &op, PhysicalOperator &plan) {
	throw NotImplementedException("CREATE TABLE AS is not supported for MongoDB catalogs");
}

PhysicalOperator &MongoCatalog::PlanInsert(ClientContext &context, PhysicalPlanGenerator &planner, LogicalInsert &op,
                                         optional_ptr<PhysicalOperator> plan) {
	throw NotImplementedException("INSERT is not supported for MongoDB catalogs");
}

PhysicalOperator &MongoCatalog::PlanDelete(ClientContext &context, PhysicalPlanGenerator &planner, LogicalDelete &op,
                                          PhysicalOperator &plan) {
	throw NotImplementedException("DELETE is not supported for MongoDB catalogs");
}

PhysicalOperator &MongoCatalog::PlanUpdate(ClientContext &context, PhysicalPlanGenerator &planner, LogicalUpdate &op,
                                         PhysicalOperator &plan) {
	throw NotImplementedException("UPDATE is not supported for MongoDB catalogs");
}

void MongoCatalog::DropSchema(ClientContext &context, DropInfo &info) {
	lock_guard<mutex> lock(schemas_lock);
	auto it = schemas.find(info.name);
	if (it != schemas.end()) {
		schemas.erase(it);
	} else if (info.if_not_found == OnEntryNotFound::THROW_EXCEPTION) {
		throw CatalogException("Schema with name \"%s\" not found", info.name);
	}
}

vector<string> MongoCatalog::GetCachedCollectionNames(const string &db_name) const {
	lock_guard<mutex> lock(collection_cache_lock);
	auto it = collection_cache.find(db_name);
	if (it != collection_cache.end()) {
		return it->second;
	}
	return vector<string>();
}

void MongoCatalog::CacheCollectionNames(const string &db_name, const vector<string> &collections) {
	lock_guard<mutex> lock(collection_cache_lock);
	collection_cache[db_name] = collections;
}

} // namespace duckdb
