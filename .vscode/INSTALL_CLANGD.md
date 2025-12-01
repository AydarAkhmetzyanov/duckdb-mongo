# Install clangd Extension

The clangd extension is **not installed**. Here's how to install it:

## Steps:

1. **Open Extensions:**
   - Press `Cmd+Shift+X` (or View → Extensions)

2. **Search for clangd:**
   - Type: `clangd` in the search box
   - Look for: **"clangd" by LLVM**
   - It should show a green "Install" button

3. **Install:**
   - Click "Install" on the clangd extension
   - Wait for installation to complete

4. **Verify Installation:**
   - After installation, you should see "clangd" in the Extensions list
   - Status should show "Enabled" or "Installed"

5. **Reload Window:**
   - Press `Cmd+Shift+P`
   - Type: `Developer: Reload Window`
   - Press Enter

6. **Test Commands:**
   - Press `Cmd+Shift+P`
   - Type: `clangd`
   - You should now see:
     - `clangd: Restart language server`
     - `clangd: Show status`
     - `clangd: Toggle indexing`

## Alternative: Install via Command Line

If the extension doesn't show up in Cursor, you can try installing it manually:

```bash
# This might not work in Cursor, but worth trying
cursor --install-extension llvm-vs-code-extensions.vscode-clangd
```

## After Installation

Once clangd is installed:

1. **Restart clangd:**
   - `Cmd+Shift+P` → `clangd: Restart language server`

2. **Wait 30-60 seconds** for indexing

3. **Test Command+Click:**
   - Open `src/mongo_table_function.cpp`
   - Command+Click on `MongoScanData` (line 21)
   - Should navigate to header file

## If clangd Extension Still Doesn't Show

Cursor might not support the clangd extension. In that case:

1. **Use the C++ extension** (but fix the missing binaries issue)
2. **Or use a different editor** (VS Code) for C++ development
3. **Or use command-line tools:**
   - `clangd --check=src/mongo_table_function.cpp`
   - `ccls` (alternative LSP server)

Let me know if you can find the clangd extension in Cursor's extension marketplace!

