#!/bin/bash
# Comprehensive cleanup script to remove unwanted files from git tracking
# and push the cleaned repository

set -e

echo "=== Repository Cleanup ==="
echo ""

# Check if we're in a git repo
if ! git rev-parse --git-dir > /dev/null 2>&1; then
    echo "Error: Not in a git repository"
    exit 1
fi

# Get current branch
BRANCH=$(git branch --show-current)
echo "Current branch: $BRANCH"
echo ""

# Step 1: Remove unwanted directories from tracking
echo "Step 1: Removing unwanted directories from git tracking..."
for dir in .vscode build vcpkg_installed duckdb_unittest_tempdir .idea; do
    if git ls-files | grep -q "^${dir}/"; then
        echo "  Removing $dir/..."
        git rm -r --cached "$dir/" 2>/dev/null || true
    else
        echo "  $dir/ not tracked (skipping)"
    fi
done

# Step 2: Remove test database files
echo ""
echo "Step 2: Removing test database files..."
TEST_FILES=$(git ls-files | grep -E "(^host=|\.db$|\.duckdb$|\.duckdb\.wal$)" || true)
if [ -n "$TEST_FILES" ]; then
    echo "$TEST_FILES" | while read file; do
        echo "  Removing $file..."
        git rm --cached "$file" 2>/dev/null || true
    done
else
    echo "  No test database files found"
fi

# Step 3: Remove workspace files
echo ""
echo "Step 3: Removing workspace files..."
WORKSPACE_FILES=$(git ls-files | grep "\.code-workspace$" || true)
if [ -n "$WORKSPACE_FILES" ]; then
    echo "$WORKSPACE_FILES" | while read file; do
        echo "  Removing $file..."
        git rm --cached "$file" 2>/dev/null || true
    done
else
    echo "  No workspace files found"
fi

# Step 4: Remove editor/linter config files (optional - these are usually kept)
echo ""
echo "Step 4: Removing editor/linter config files..."
for file in .clang-format .clang-tidy .editorconfig; do
    if git ls-files | grep -q "^${file}$"; then
        echo "  Removing $file..."
        git rm --cached "$file" 2>/dev/null || true
    fi
done

# Step 5: Stage .gitignore
echo ""
echo "Step 5: Staging updated .gitignore..."
git add .gitignore

# Step 6: Show status
echo ""
echo "Step 6: Review changes..."
echo ""
git status --short

echo ""
echo "=== Cleanup Complete ==="
echo ""
echo "Files have been removed from git tracking (but kept locally)."
echo ""
echo "To commit and push, run:"
echo "  git commit -m 'Remove build artifacts, IDE configs, and test files from tracking'"
echo "  git push --force origin $BRANCH"
echo ""
echo "⚠️  WARNING: Force push rewrites history. Make sure no one else is working on this branch!"
echo ""
read -p "Do you want to commit and push now? (y/N) " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo ""
    echo "Committing changes..."
    git commit -m "Remove build artifacts, IDE configs, and test files from tracking"
    
    echo ""
    echo "Pushing to remote..."
    git push --force origin "$BRANCH"
    
    echo ""
    echo "✅ Cleanup and push complete!"
else
    echo ""
    echo "Skipped commit/push. You can run these commands manually:"
    echo "  git commit -m 'Remove build artifacts, IDE configs, and test files from tracking'"
    echo "  git push --force origin $BRANCH"
fi

