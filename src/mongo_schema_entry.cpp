#include "mongo_schema_entry.hpp"
#include "duckdb/catalog/catalog_entry/view_catalog_entry.hpp"
#include "duckdb/catalog/catalog_entry/table_catalog_entry.hpp"
#include "duckdb/parser/parsed_data/create_view_info.hpp"
#include "duckdb/parser/parsed_data/create_table_info.hpp"
#include "duckdb/parser/parsed_data/create_function_info.hpp"
#include "duckdb/parser/parsed_data/create_index_info.hpp"
#include "duckdb/parser/parsed_data/create_sequence_info.hpp"
#include "duckdb/parser/parsed_data/create_table_function_info.hpp"
#include "duckdb/parser/parsed_data/create_copy_function_info.hpp"
#include "duckdb/parser/parsed_data/create_pragma_function_info.hpp"
#include "duckdb/parser/parsed_data/create_collation_info.hpp"
#include "duckdb/parser/parsed_data/create_type_info.hpp"
#include "duckdb/parser/parsed_data/drop_info.hpp"
#include "duckdb/parser/parsed_data/alter_info.hpp"
#include "duckdb/planner/parsed_data/bound_create_table_info.hpp"
#include "duckdb/main/client_context.hpp"
#include "duckdb/common/mutex.hpp"
#include "duckdb/common/exception.hpp"
#include <chrono>
#include <iostream>
namespace duckdb {

MongoSchemaEntry::MongoSchemaEntry(Catalog &catalog, CreateSchemaInfo &info)
    : SchemaCatalogEntry(catalog, info) {
}

optional_ptr<CatalogEntry> MongoSchemaEntry::LookupEntry(CatalogTransaction transaction,
                                                         const EntryLookupInfo &lookup_info) {
	// Collections are represented as views, but queries may look for TABLE_ENTRY
	// Handle both types since they refer to the same collections
	if (lookup_info.GetCatalogType() != CatalogType::VIEW_ENTRY && 
	    lookup_info.GetCatalogType() != CatalogType::TABLE_ENTRY) {
		return nullptr;
	}

	const auto &entry_name = lookup_info.GetEntryName();
	lock_guard<mutex> lock(entry_lock);

	// First check if view already exists
	auto it = views.find(entry_name);
	if (it != views.end()) {
		return it->second.get();
	}

	// Try default generator if available
	if (default_generator && transaction.context) {
		auto default_entry = default_generator->CreateDefaultEntry(*transaction.context, entry_name);
		if (default_entry) {
			// Create the view entry
			auto view_entry = dynamic_cast<ViewCatalogEntry *>(default_entry.get());
			if (view_entry) {
				auto shared_entry = shared_ptr<CatalogEntry>(default_entry.release());
				views[entry_name] = shared_entry;
				return shared_entry.get();
			}
		}
	}

	return nullptr;
}

void MongoSchemaEntry::SetDefaultGenerator(unique_ptr<DefaultGenerator> generator) {
	lock_guard<mutex> lock(entry_lock);
	default_generator = std::move(generator);
}

optional_ptr<CatalogEntry> MongoSchemaEntry::CreateView(CatalogTransaction transaction, CreateViewInfo &info) {
	lock_guard<mutex> lock(entry_lock);
	
	// Check if view already exists
	auto it = views.find(info.view_name);
	if (it != views.end()) {
		if (info.on_conflict == OnCreateConflict::ERROR_ON_CONFLICT) {
			throw CatalogException::EntryAlreadyExists(CatalogType::VIEW_ENTRY, info.view_name);
		}
		return it->second.get();
	}

	// Create new view entry
	auto view_entry = make_uniq<ViewCatalogEntry>(catalog, *this, info);
	auto result = view_entry.get();
	auto shared_entry = shared_ptr<CatalogEntry>(view_entry.release());
	views[info.view_name] = shared_entry;
	
	return result;
}

optional_ptr<CatalogEntry> MongoSchemaEntry::CreateTable(CatalogTransaction transaction, BoundCreateTableInfo &info) {
	throw NotImplementedException("CREATE TABLE is not supported for MongoDB catalogs");
}

optional_ptr<CatalogEntry> MongoSchemaEntry::CreateFunction(CatalogTransaction transaction, CreateFunctionInfo &info) {
	throw NotImplementedException("CREATE FUNCTION is not supported for MongoDB catalogs");
}

optional_ptr<CatalogEntry> MongoSchemaEntry::CreateIndex(CatalogTransaction transaction, CreateIndexInfo &info,
                                                         TableCatalogEntry &table) {
	throw NotImplementedException("CREATE INDEX is not supported for MongoDB catalogs");
}

optional_ptr<CatalogEntry> MongoSchemaEntry::CreateSequence(CatalogTransaction transaction, CreateSequenceInfo &info) {
	throw NotImplementedException("CREATE SEQUENCE is not supported for MongoDB catalogs");
}

optional_ptr<CatalogEntry> MongoSchemaEntry::CreateTableFunction(CatalogTransaction transaction,
                                                                CreateTableFunctionInfo &info) {
	throw NotImplementedException("CREATE TABLE FUNCTION is not supported for MongoDB catalogs");
}

optional_ptr<CatalogEntry> MongoSchemaEntry::CreateCopyFunction(CatalogTransaction transaction,
                                                               CreateCopyFunctionInfo &info) {
	throw NotImplementedException("CREATE COPY FUNCTION is not supported for MongoDB catalogs");
}

optional_ptr<CatalogEntry> MongoSchemaEntry::CreatePragmaFunction(CatalogTransaction transaction,
                                                                  CreatePragmaFunctionInfo &info) {
	throw NotImplementedException("CREATE PRAGMA FUNCTION is not supported for MongoDB catalogs");
}

optional_ptr<CatalogEntry> MongoSchemaEntry::CreateCollation(CatalogTransaction transaction, CreateCollationInfo &info) {
	throw NotImplementedException("CREATE COLLATION is not supported for MongoDB catalogs");
}

optional_ptr<CatalogEntry> MongoSchemaEntry::CreateType(CatalogTransaction transaction, CreateTypeInfo &info) {
	throw NotImplementedException("CREATE TYPE is not supported for MongoDB catalogs");
}

void MongoSchemaEntry::Alter(CatalogTransaction transaction, AlterInfo &info) {
	throw NotImplementedException("ALTER is not supported for MongoDB catalogs");
}

void MongoSchemaEntry::TryLoadEntries(ClientContext &context) {
	auto start_total = std::chrono::high_resolution_clock::now();
	
	if (is_loaded) {
		return;
	}
	
	if (!default_generator) {
		is_loaded = true;
		return;
	}
	
	// Use separate load_lock to prevent concurrent loading (Postgres pattern)
	lock_guard<mutex> load_guard(load_lock);
	if (is_loaded) {
		return; // Double-check after acquiring lock
	}
	
	// Load collection names only - views are created lazily in Scan() or LookupEntry()
	vector<string> collection_names;
	try {
		auto start_get_entries = std::chrono::high_resolution_clock::now();
		collection_names = default_generator->GetDefaultEntries();
		auto end_get_entries = std::chrono::high_resolution_clock::now();
		auto get_entries_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_get_entries - start_get_entries).count();
		std::cerr << "[MongoSchemaEntry] GetDefaultEntries took " << get_entries_ms << "ms, found " << collection_names.size() << " collections" << std::endl;
	} catch (const std::exception &e) {
		is_loaded = true;
		return;
	} catch (...) {
		is_loaded = true;
		return;
	}
	
	lock_guard<mutex> entry_guard(entry_lock);
	loaded_collection_names = collection_names;
	
	is_loaded = true;
	auto end_total = std::chrono::high_resolution_clock::now();
	auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_total - start_total).count();
	std::cerr << "[MongoSchemaEntry] TryLoadEntries total time: " << total_ms << "ms" << std::endl;
}

shared_ptr<CatalogEntry> MongoSchemaEntry::GetOrCreateViewEntry(ClientContext &context, const string &collection_name) {
	lock_guard<mutex> lock(entry_lock);
	
	// Check if entry already exists
	auto it = views.find(collection_name);
	if (it != views.end()) {
		return it->second;
	}
	
	// Create entry lazily
	if (default_generator) {
		try {
			auto default_entry = default_generator->CreateDefaultEntry(context, collection_name);
			if (default_entry) {
				auto view_entry = dynamic_cast<ViewCatalogEntry *>(default_entry.get());
				if (view_entry) {
					auto shared_entry = shared_ptr<CatalogEntry>(default_entry.release());
					views[collection_name] = shared_entry;
					return shared_entry;
				}
			}
		} catch (...) {
			// If creation fails, return nullptr
			return nullptr;
		}
	}
	
	return nullptr;
}

void MongoSchemaEntry::Scan(ClientContext &context, CatalogType type, const std::function<void(CatalogEntry &)> &callback) {
	// Collections are represented as views, handle both TABLE_ENTRY and VIEW_ENTRY
	if (type == CatalogType::VIEW_ENTRY || type == CatalogType::TABLE_ENTRY) {
		if (!is_loaded) {
			TryLoadEntries(context);
		}
		
		vector<string> collections_to_create;
		{
			lock_guard<mutex> lock(entry_lock);
			for (auto &[name, entry] : views) {
				callback(*entry);
			}
			for (const auto &collection_name : loaded_collection_names) {
				if (views.find(collection_name) == views.end()) {
					collections_to_create.push_back(collection_name);
				}
			}
		}
		
		auto start_scan_create = std::chrono::high_resolution_clock::now();
		for (const auto &collection_name : collections_to_create) {
			auto entry = GetOrCreateViewEntry(context, collection_name);
			if (entry) {
				callback(*entry);
			}
		}
		auto end_scan_create = std::chrono::high_resolution_clock::now();
		auto scan_create_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_scan_create - start_scan_create).count();
		if (scan_create_ms > 100) {
			std::cerr << "[MongoSchemaEntry] Scan() created " << collections_to_create.size() << " view entries in " << scan_create_ms << "ms" << std::endl;
		}
	}
}

void MongoSchemaEntry::Scan(CatalogType type, const std::function<void(CatalogEntry &)> &callback) {
	if (type == CatalogType::VIEW_ENTRY) {
		lock_guard<mutex> lock(entry_lock);
		
		// First, return any views that are already created
		for (auto &[name, entry] : views) {
			callback(*entry);
		}
		
		// For the non-context version, we can't use the default generator
		// as it requires a ClientContext. Just return what we have.
	}
}

void MongoSchemaEntry::DropEntry(ClientContext &context, DropInfo &info) {
	if (info.type == CatalogType::VIEW_ENTRY) {
		lock_guard<mutex> lock(entry_lock);
		auto it = views.find(info.name);
		if (it != views.end()) {
			views.erase(it);
		} else if (info.if_not_found == OnEntryNotFound::THROW_EXCEPTION) {
			throw CatalogException("View with name \"%s\" not found", info.name);
		}
	} else {
		throw NotImplementedException("DROP is only supported for views in MongoDB catalogs");
	}
}

} // namespace duckdb

