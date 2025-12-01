# Fix IntelliSense Red Lines and Command+Click

If you see red lines and Command+Click isn't working after installing the C/C++ extension, follow these steps:

## Step 1: Reset IntelliSense Database

1. **Press `Cmd+Shift+P`**
2. **Type:** `C/C++: Reset IntelliSense Database`
3. **Press Enter**
4. Wait for the notification that it's done

## Step 2: Reload Window

1. **Press `Cmd+Shift+P`**
2. **Type:** `Developer: Reload Window`
3. **Press Enter**

## Step 3: Wait for Indexing

- Check the **status bar** (bottom right) for "Parsing..." or "Indexing..."
- This may take **2-5 minutes** for the DuckDB codebase
- You can continue working while it indexes

## Step 4: Verify Configuration

1. **Press `Cmd+Shift+P`**
2. **Type:** `C/C++: Select a Configuration...`
3. **Select:** "Mac" (should be the only option)

## Step 5: Check Output Panel

If it still doesn't work:

1. **Press `Cmd+Shift+U`** (View → Output)
2. **Select:** "C/C++" from the dropdown
3. **Look for errors** about:
   - Missing include paths
   - Invalid compile_commands.json
   - Configuration issues

## Step 6: Verify compile_commands.json

Run this in terminal:
```bash
cd /Users/steph.wang/Documents/funprojects/duckdb-mongo
ls -la build/release/compile_commands.json
```

Should show a file ~1.2MB in size.

## Step 7: If Still Not Working

Try disabling and re-enabling the extension:

1. **Press `Cmd+Shift+X`** (Extensions)
2. **Find:** "C/C++" (Microsoft)
3. **Click:** "Disable" (or "Disable (Workspace)")
4. **Reload Window** (`Cmd+Shift+P` → "Developer: Reload Window")
5. **Re-enable** the extension
6. **Reload Window** again

## Alternative: Use clangd Instead

If the Microsoft C/C++ extension continues to have issues, try clangd:

1. **Install clangd:**
   ```bash
   brew install llvm
   ```

2. **Install clangd extension:**
   - Press `Cmd+Shift+X`
   - Search for: `clangd` (by LLVM)
   - Install it

3. **Disable Microsoft C/C++ Extension:**
   - Press `Cmd+Shift+X`
   - Find "C/C++" (Microsoft)
   - Click "Disable (Workspace)"

4. **Reload Window**

clangd is often faster and more accurate for large codebases like DuckDB.



