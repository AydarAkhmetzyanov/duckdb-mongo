# TPC-H Benchmarking Guide

This directory contains scripts to benchmark TPC-H queries comparing DuckDB+MongoDB extension vs native MongoDB.

## Files

- **`benchmark-tpch.sh`** - Main benchmarking script
- **`benchmark-mongodb-queries.py`** - MongoDB aggregation pipelines for TPC-H queries
- **`create-tpch-mongo.sh`** - Generate TPC-H data and load into MongoDB

## Prerequisites

1. **Build DuckDB**: `make release`
2. **MongoDB Running**: Local instance on port 27017
3. **Python Dependencies**: `pip install pymongo`
4. **TPC-H Data**: Run `./create-tpch-mongo.sh` to generate and load data

## Quick Start

```bash
cd benchmarks

# Run all queries (3 iterations)
./benchmark-tpch.sh all 3

# Run with detailed comparison
./benchmark-tpch.sh all 3 --verbose

# Run specific query
./benchmark-tpch.sh 1 5

# Show query results
./benchmark-tpch.sh 6 1 --show-results
```

### Command Syntax

```bash
./benchmark-tpch.sh [query_number|all] [iterations] [--verbose|-v] [--show-results|-s]

Arguments:
  query_number     - Query number (1-22) or 'all'
  iterations       - Number of runs (default: 3)
  --verbose|-v     - Detailed DuckDB vs MongoDB comparison
  --show-results|-s - Display query results
```

## Comparing Results

Use `--verbose` to compare DuckDB+MongoDB extension vs native MongoDB:

```bash
./benchmark-tpch.sh all 3 --verbose
```

The output shows:
- Execution times for both approaches
- Speedup ratio: `> 1x` means native MongoDB is faster, `< 1x` means DuckDB+MongoDB is faster
- Per-iteration timings for consistency analysis

Default mode (without `--verbose`) shows summary statistics only.

## Setup

### Generate and Load TPC-H Data

```bash
cd benchmarks
./create-tpch-mongo.sh
```

This script:
- Generates TPC-H data (scale factor 0.01)
- Loads data into MongoDB collections
- Creates indexes
- Sets environment variables (`MONGODB_TEST_DATABASE_AVAILABLE=1`, `MONGO_TPCH_DATABASE=tpch`)

### Verify Setup

```bash
mongosh tpch --eval "db.lineitem.countDocuments()"
mongosh tpch --eval "db.getCollectionNames()"
```

## Tips

- Use 3-5 iterations for reliable averages
- First run may be slower (cold start)
- Run on idle system for consistent results
- Verify indexes: `mongosh tpch --eval "db.lineitem.getIndexes()"`

## Troubleshooting

### MongoDB Connection

```bash
mongosh --eval "db.adminCommand('ping')"
mongosh "mongodb://localhost:27017/tpch"
```

### DuckDB Not Found

```bash
make release
ls -lh build/release/duckdb
```

### Python Errors

```bash
python3 -c "import pymongo; print(pymongo.__version__)"
python3 -m py_compile benchmark-mongodb-queries.py
```

### Missing Data

```bash
./create-tpch-mongo.sh
mongosh tpch --eval "db.getCollectionNames()"
```

## Advanced Usage

### Custom Scale Factor

Edit `create-tpch-mongo.sh`:
```bash
CALL dbgen(sf=0.1, schema='tpch');  # Change from 0.01
```

### Export Results

```bash
./benchmark-tpch.sh all 3 > results.txt
./benchmark-tpch.sh all 3 --verbose > verbose_results.txt
```

## Resources

- [TPC-H Specification](http://www.tpc.org/tpch/)
- [DuckDB Docs](https://duckdb.org/docs/)
- [MongoDB Aggregation](https://www.mongodb.com/docs/manual/aggregation/)
