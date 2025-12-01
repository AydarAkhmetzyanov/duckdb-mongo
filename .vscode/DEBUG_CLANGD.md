# Debug clangd Command+Click

If Command+Click still isn't working, follow these steps:

## 1. Check if clangd is Running

1. **Open Output Panel:**
   - Press `Cmd+Shift+U` (View → Output)
   - Select "clangd" from the dropdown

2. **Look for:**
   - ✅ "LSP ready" or "Indexed" messages
   - ❌ Error messages about missing files

## 2. Restart clangd

1. **Press `Cmd+Shift+P`**
2. **Type:** `clangd: Restart language server`
3. **Press Enter**
4. **Wait 30-60 seconds** for re-indexing

## 3. Check clangd Status

1. **Press `Cmd+Shift+P`**
2. **Type:** `clangd: Show status`
3. **Check:**
   - Should show "Indexed" or "Indexing..."
   - Should NOT show errors

## 4. Verify compile_commands.json

Run this command:
```bash
cd /Users/steph.wang/Documents/funprojects/duckdb-mongo
head -50 build/release/compile_commands.json
```

Should see entries for `mongo_table_function.cpp` and `mongo_extension.cpp`.

## 5. Test Command+Click

1. **Open:** `src/mongo_table_function.cpp`
2. **Go to line 30:** `case bsoncxx::type::k_array:`
3. **Command+Click on:** `bsoncxx::type` (or any type/class)
4. **Should navigate to:** Header file

## 6. If Still Not Working

Try these:

### Option A: Force Re-index
1. **Close Cursor completely**
2. **Delete clangd cache:**
   ```bash
   rm -rf ~/.cache/clangd
   ```
3. **Reopen Cursor**
4. **Wait 2-3 minutes** for full re-index

### Option B: Check Extension
1. **Press `Cmd+Shift+X`**
2. **Search:** `clangd`
3. **Verify:** "clangd" by LLVM is **enabled**
4. **If disabled:** Click "Enable"

### Option C: Use Right-Click Menu
1. **Right-click** on symbol (e.g., `MongoScanData`)
2. **Select:** "Go to Definition" or "Go to Declaration"
3. **If this works:** Command+Click should work too

## 7. Check for Conflicts

Make sure **only ONE** C++ extension is active:
- ✅ **clangd** (by LLVM) - **ENABLED**
- ❌ **C/C++** (by Microsoft/Anysphere) - **DISABLED**

## 8. Last Resort: Manual Test

Test if clangd can find symbols:
```bash
cd /Users/steph.wang/Documents/funprojects/duckdb-mongo
clangd --check=src/mongo_table_function.cpp --compile-commands-dir=build/release 2>&1 | head -20
```

Should NOT show "file not found" errors.

