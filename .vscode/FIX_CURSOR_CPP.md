# Fix C++ Extension in Cursor

The C++ extension binaries are missing and IntelliSense is disabled. Here's how to fix it:

## Problem
- Extension: `anysphere.cpptools-2.0.2-darwin-arm64` (Cursor's C++ extension)
- Missing binaries: `cpptools`, `cpptools-srv`, `cpptools-wordexp`
- IntelliSense: **disabled**

## Solution 1: Reinstall C++ Extension

1. **Uninstall Extension:**
   - Press `Cmd+Shift+X` (Extensions)
   - Search for "C/C++" or "C++"
   - Find the C++ extension (by Anysphere or Microsoft)
   - Click "Uninstall"

2. **Reload Window:**
   - Press `Cmd+Shift+P`
   - Type: `Developer: Reload Window`
   - Press Enter

3. **Reinstall Extension:**
   - Press `Cmd+Shift+X`
   - Search for: `C/C++` (by Microsoft) or `C++` (by Anysphere)
   - Click "Install"
   - Wait for installation to complete

4. **Reload Window again**

5. **Check Output:**
   - Press `Cmd+Shift+U` (View → Output)
   - Select "C/C++" from dropdown
   - Should see "IntelliSense Engine starting..." instead of errors

## Solution 2: Use clangd Instead (Recommended for Cursor)

clangd works better with Cursor and large codebases like DuckDB:

1. **Install clangd:**
   ```bash
   brew install llvm
   ```

2. **Install clangd Extension:**
   - Press `Cmd+Shift+X`
   - Search for: `clangd` (by LLVM)
   - Click "Install"

3. **Disable C++ Extension:**
   - Press `Cmd+Shift+X`
   - Find "C/C++" or "C++" extension
   - Click "Disable (Workspace)"

4. **Reload Window:**
   - `Cmd+Shift+P` → "Developer: Reload Window"

5. **clangd will automatically:**
   - Use `compile_commands.json`
   - Provide "Go to Definition" (Command+Click)
   - Work immediately

## Why clangd is Better for This Project

- ✅ Faster indexing
- ✅ Better with large codebases (DuckDB is ~22MB)
- ✅ More accurate symbol resolution
- ✅ Works better with `compile_commands.json`
- ✅ No missing binary issues

## Test After Fix

1. **Open:** `src/mongo_table_function.cpp`
2. **Command+Click** on `MongoScanData` (line 21)
3. **Should navigate to:** `src/include/mongo_table_function.hpp`

If it works, IntelliSense is active! 🎉


