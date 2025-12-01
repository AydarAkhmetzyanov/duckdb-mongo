#!/bin/bash
# Script to clean up repository by removing tracked files that should be ignored
# This removes files from git tracking but keeps them locally

set -e

echo "=== Cleaning up repository ==="
echo ""

# Remove build artifacts from git tracking
echo "Removing build artifacts from git tracking..."

# Remove directories that should be ignored
for dir in build vcpkg_installed duckdb_unittest_tempdir .vscode .idea; do
    if git ls-files | grep -q "^${dir}/"; then
        echo "  Removing $dir/ from git tracking..."
        git rm -r --cached "$dir/" 2>/dev/null || true
    fi
done

# Remove test database files
echo "Removing test database files from git tracking..."
git ls-files | grep -E "(host=|\.db$|\.duckdb$)" | while read file; do
    echo "  Removing $file from git tracking..."
    git rm --cached "$file" 2>/dev/null || true
done

# Remove workspace files
echo "Removing workspace files from git tracking..."
git ls-files | grep -E "\.code-workspace$" | while read file; do
    echo "  Removing $file from git tracking..."
    git rm --cached "$file" 2>/dev/null || true
done

echo ""
echo "=== Cleanup complete ==="
echo ""
echo "Files have been removed from git tracking but kept locally."
echo "Review changes with: git status"
echo "Commit the cleanup with: git commit -m 'Remove build artifacts and test files from tracking'"
echo "Then force push: git push --force origin main"
echo ""
echo "⚠️  WARNING: Force pushing rewrites history. Make sure you coordinate with collaborators!"

