# duckdb-mongo

Integrates DuckDB with MongoDB, enabling direct SQL queries over MongoDB collections without exporting data or ETL.

## Overview

The `duckdb-mongo` extension allows you to query MongoDB collections directly from DuckDB using SQL. It provides a table function `mongo_scan` that connects to MongoDB, infers the schema from your documents, and returns the data as a DuckDB table.

## Architecture

The extension provides **direct SQL access to MongoDB without exporting or copying data**. All queries execute against live MongoDB data in real-time.

```
┌──────────────────┐
│ User/Application │
│   (SQL Queries)  │
└────────┬─────────┘
         │ SQL Query
         ▼
┌─────────────────────────────────────────┐
│         DUCKDB ENGINE                   │
│  ┌───────────────────────────────────┐  │
│  │ SQL Planning & Optimization       │  │
│  │ - Query planning                  │  │
│  │ - Filter pushdown analysis        │  │
│  └───────────────────────────────────┘  │
│  ┌───────────────────────────────────┐  │
│  │ SQL Execution (Analytical)        │  │
│  │ - Joins                           │  │
│  │ - Aggregations (GROUP BY, COUNT)  │  │
│  │ - Sorting                         │  │
│  │ - Window functions                │  │
│  │ - Complex SQL operations          │  │
│  └───────────────────────────────────┘  │
└────────┬────────────────────────────────┘
         │ Table Function Call (mongo_scan)
         │ Requests data chunks
         ▼
┌─────────────────────────────────────────┐
│ duckdb-mongo Extension                  │
│                                         │
│  • Schema Inference                     │
│  • Filter Translation                   │
│  • BSON → Columnar                      │
│  • Type Conversion                      │
└────────┬────────────────────────────────┘
         │
         │ MongoDB Query (filtered)
         │ Stream documents on-demand
         ▼
┌─────────────────────────────────────────┐
│         MONGODB DATABASE                │
│  ┌───────────────────────────────────┐  │
│  │ Document Store Operations         │  │
│  │ - Index lookups                   │  │
│  │ - Document filtering              │  │
│  │ - Cursor management               │  │
│  │ - Document retrieval              │  │
│  └───────────────────────────────────┘  │
│                                         │
│  Data stays here (No ETL/Export)        │
└─────────────────────────────────────────┘
```

### mongo_scan Execution Flow

```
┌─────────────────────────────────────────────────────────────────┐
│                    mongo_scan Execution                         │
└─────────────────────────────────────────────────────────────────┘

1. BIND PHASE (happens once per query)
   ┌────────────────────────────────────────────────────────────┐
   │ Parse connection string, database, collection              │
   │ Create MongoDB connection                                  │
   │                                                            │
   │ Schema Inference:                                          │
   │   • Sample N documents from collection                     │
   │   • Collect all field paths (nested traversal)             │
   │   • Resolve type conflicts (frequency analysis)            │
   │   • Build column names and types                           │
   │                                                            │
   │ Return schema to DuckDB                                    │
   └────────────────────────────────────────────────────────────┘
                              │
                              ▼
2. INIT PHASE (happens once per query)
   ┌────────────────────────────────────────────────────────────┐
   │ Get collection reference                                   │
   │ Convert DuckDB WHERE filters → MongoDB $match query        │
   │   • Parse table filters from query plan                    │
   │   • Convert to MongoDB operators ($eq, $gt, $gte, etc.)    │
   │   • Merge multiple filters on same column                  │
   │                                                            │
   │ Create MongoDB cursor:                                     │
   │   • Execute find() with $match filter                      │
   │   • MongoDB applies filters using indexes                  │
   │   • Returns cursor iterator                                │
   └────────────────────────────────────────────────────────────┘
                              │
                              ▼
3. EXECUTION PHASE (called repeatedly for each chunk)
   ┌────────────────────────────────────────────────────────────┐
   │ Fetch documents from cursor:                               │
   │   • Retrieve BSON documents from MongoDB                   │
   │                                                            │
   │ For each document:                                         │
   │   • Parse BSON structure                                   │
   │   • Extract fields by path                                 │
   │   • Convert BSON types → DuckDB types                      │ 
   │   • Flatten nested structures                              │
   │   • Write to columnar DataChunk                            │
   │                                                            │
   │ Return chunk to DuckDB (up to STANDARD_VECTOR_SIZE rows)   │
   └────────────────────────────────────────────────────────────┘
                              │
                              ▼
   ┌────────────────────────────────────────────────────────────┐
   │ DuckDB processes chunk:                                    │
   │   • Filters already applied in MongoDB (via pushdown)      │
   │   • Performs aggregations, joins, etc.                     │
   │   • Requests next chunk if needed                          │
   └────────────────────────────────────────────────────────────┘
```

### Entity Mapping

When using `ATTACH` to connect to MongoDB, the extension maps MongoDB entities to DuckDB entities as follows:

```
MongoDB Entity          →  DuckDB Entity
─────────────────────────────────────────
MongoDB Instance        →  Catalog (via ATTACH)
MongoDB Database        →  Schema
MongoDB Collection      →  Table/View
```

**Details:**
- **MongoDB Instance → DuckDB Catalog**: `ATTACH` creates a `MongoCatalog` representing the MongoDB connection
- **MongoDB Database → DuckDB Schema**: Each MongoDB database becomes a DuckDB schema
- **MongoDB Collection → DuckDB Table/View**: Collections appear as views (backed by `mongo_scan`)

### Default Schema Behavior

Follows DuckDB conventions (similar to Postgres `"public"`):
- **Without `dbname`**: Scans all databases, defaults to `"main"` schema
- **With `dbname`**: Uses database name as default schema

```sql
ATTACH 'host=localhost port=27017' AS mongo_all (TYPE MONGO);
USE mongo_all;  -- Defaults to "main"

ATTACH 'host=localhost port=27017 dbname=mydb' AS mongo_db (TYPE MONGO);
USE mongo_db;  -- Defaults to "mydb"
```


## Features

- Direct SQL queries over MongoDB collections (no ETL/export)
- Automatic schema inference (samples 100 documents by default)
- Nested document flattening with underscore-separated names
- BSON type mapping to DuckDB SQL types
- **Filter pushdown**: WHERE clauses pushed to MongoDB to leverage indexes
- Optional MongoDB query filters
- Read-only (write support may be added)

## Pushdown Strategy

The extension uses a selective pushdown strategy that leverages MongoDB's indexing capabilities while utilizing DuckDB's analytical strengths:

**Pushed Down to MongoDB:** Filters (WHERE clauses), LIMIT clauses, and projections to reduce data transfer and leverage MongoDB indexes.

**Kept in DuckDB:** Aggregations, joins, and complex SQL features (window functions, CTEs, subqueries) where DuckDB's query optimizer and execution engine excel.

This hybrid approach provides fast indexed filtering from MongoDB combined with powerful analytical processing from DuckDB, optimizing performance for both simple filtered queries and complex analytical workloads.

## Building

### Prerequisites

- CMake 3.5 or higher
- C++ compiler with C++17 support
- vcpkg (for dependency management)

### Managing dependencies

DuckDB extensions use VCPKG for dependency management. To set up VCPKG:

```shell
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh  # On Windows: .\bootstrap-vcpkg.bat
export VCPKG_TOOLCHAIN_PATH=`pwd`/scripts/buildsystems/vcpkg.cmake
```

### Build steps

1. Clone the repository with submodules:
```sh
git clone --recurse-submodules https://github.com/stephaniewang526/duckdb-mongo.git
cd duckdb-mongo
```

2. Install dependencies (first time only):
```sh
# Install MongoDB C++ driver via vcpkg
../vcpkg/vcpkg install --triplet arm64-osx  # or x64-osx for Intel Mac
```

3. Build the extension:
```sh
# Set vcpkg environment
export VCPKG_TOOLCHAIN_PATH=../vcpkg/scripts/buildsystems/vcpkg.cmake
export VCPKG_TARGET_TRIPLET=arm64-osx  # or x64-osx for Intel Mac

# Build
make release
```

Or use the build script:
```sh
bash scripts/build.sh
```

The main binaries that will be built are:
- `./build/release/duckdb` - DuckDB shell with the extension pre-loaded
- `./build/release/test/unittest` - Test runner
- `./build/release/extension/mongo/mongo.duckdb_extension` - Loadable extension binary

## Usage

### Loading the Extension

```sh
./build/release/duckdb  # Extension auto-loaded
```

Or load explicitly:
```sql
LOAD '/path/to/mongo.duckdb_extension';
```

### Attaching MongoDB Databases

**Key-value format:**
```sql
ATTACH 'host=localhost port=27017' AS mongo_db (TYPE MONGO);
ATTACH 'host=localhost port=27017 user=myuser password=mypass dbname=mydb' AS mongo_db (TYPE MONGO);
ATTACH 'host=cluster0.xxxxx.mongodb.net user=myuser password=mypass srv=true' AS atlas_db (TYPE MONGO);
```

**MongoDB URI format:**
```sql
ATTACH 'mongodb://user:pass@localhost:27017/mydb' AS mongo_db (TYPE MONGO);
```

**Connection Parameters:**

| Name | Description | Default |
|------|-------------|---------|
| `host` | MongoDB hostname or IP address | `localhost` |
| `port` | MongoDB port number | `27017` |
| `user` / `username` | MongoDB username | - |
| `password` | MongoDB password | - |
| `dbname` / `database` | Specific database to attach (empty = all databases) | - |
| `authsource` | Authentication database | `admin` |
| `srv` | Use SRV connection format (for MongoDB Atlas) | `false` |
| `options` | Additional MongoDB connection string query parameters (e.g., `readPreference=secondary`) | - |

**Using DuckDB Secrets (recommended for production):**

Store credentials securely using DuckDB Secrets instead of embedding them in connection strings:

```sql
-- Create a secret with MongoDB credentials
CREATE SECRET mongo_creds (
    TYPE mongo,
    HOST 'cluster0.xxxxx.mongodb.net',
    USER 'myuser',
    PASSWORD 'mypassword',
    SRV 'true'
);

-- Attach using the secret
ATTACH '' AS atlas_db (TYPE mongo, SECRET 'mongo_creds');
```

**Secret Parameters:**

| Name | Description | Default |
|------|-------------|---------|
| `host` | MongoDB hostname or IP address | `localhost` |
| `port` | MongoDB port number | `27017` |
| `user` / `username` | MongoDB username | - |
| `password` | MongoDB password | - |
| `database` / `dbname` | Specific database to attach | - |
| `authsource` | Authentication database | - |
| `srv` | Use SRV connection format (for MongoDB Atlas) | `false` |

**Default secret:** Create an unnamed secret to use as the default for all ATTACH operations:
```sql
CREATE SECRET (TYPE mongo, HOST 'localhost', USER 'myuser', PASSWORD 'mypass');
ATTACH '' AS mongo_db (TYPE mongo);  -- Uses __default_mongo automatically
-- Or provide additional options that merge with the secret:
ATTACH 'dbname=mydb' AS mongo_db (TYPE mongo);  -- Uses secret + overrides/adds dbname
```

**Querying after ATTACH:**
```sql
SELECT schema_name FROM information_schema.schemata WHERE catalog_name = 'mongo_db';
SHOW TABLES FROM mydb;
SELECT * FROM mydb.mycollection;
SELECT status, COUNT(*) FROM mydb.mycollection WHERE status = 'active' GROUP BY status;
```

**Using mongo_scan directly:**
```sql
SELECT * FROM mongo_scan('mongodb://localhost:27017', 'mydb', 'mycollection');
SELECT * FROM mongo_scan('mongodb://localhost:27017', 'mydb', 'mycollection', 
                         filter := '{"status": "active"}', sample_size := 200);
```

## Schema Inference

Samples documents (default: 100, configurable via `sample_size`):

**BSON Type Mapping:**

| BSON Type | DuckDB Type |
|-----------|-------------|
| String | VARCHAR |
| Int32 / Int64 | BIGINT |
| Double | DOUBLE |
| Boolean | BOOLEAN |
| Date | TIMESTAMP |
| ObjectId | VARCHAR |
| Arrays / Nested Documents | VARCHAR (JSON string) |

- **Nested Documents**: Flattened with underscore-separated names (e.g., `user_address_city`), up to 5 levels deep
- **Type Conflicts**: Frequency-based resolution (VARCHAR if >70%, numeric if ≥30%, defaults to VARCHAR)
- **Missing Fields**: NULL values

## Limitations

- Read-only
- Schema inferred from sample (may miss fields)
- Schema re-inferred per query

## Contributing

Contributions are welcome! Please open an issue or submit a pull request.

## License

See LICENSE file for details.
