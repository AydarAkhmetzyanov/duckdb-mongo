# Quick Fix: Use clangd Instead

Your Cursor C++ extension has missing binaries. **clangd is already installed** and works better!

## Steps (2 minutes):

1. **Install clangd Extension:**
   - Press `Cmd+Shift+X` (Extensions)
   - Search for: `clangd` (by LLVM)
   - Click "Install"

2. **Disable Cursor C++ Extension:**
   - Press `Cmd+Shift+X`
   - Find "C/C++" or "C++" extension (the one with missing binaries)
   - Click "Disable (Workspace)"

3. **Reload Window:**
   - Press `Cmd+Shift+P`
   - Type: `Developer: Reload Window`
   - Press Enter

4. **Wait 30 seconds** for clangd to index

5. **Test:**
   - Open `src/mongo_table_function.cpp`
   - **Command+Click** on `MongoScanData` (line 21)
   - Should navigate to `src/include/mongo_table_function.hpp` ✅

## Why clangd is Better:

- ✅ Already installed on your system
- ✅ Faster indexing
- ✅ Better for large codebases (DuckDB is ~22MB)
- ✅ Automatically uses `compile_commands.json`
- ✅ No missing binary issues
- ✅ More accurate symbol resolution

clangd will work immediately after installation!


