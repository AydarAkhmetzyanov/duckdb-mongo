# Install C/C++ Language Support

To enable "Go to Definition" (Command+Click) in VS Code/Cursor, you need to install a C/C++ language extension.

## Option 1: Microsoft C/C++ Extension (Recommended for IntelliSense)

1. **Open Extensions:**
   - Press `Cmd+Shift+X` (or View → Extensions)

2. **Search and Install:**
   - Search for: `C/C++` (by Microsoft)
   - Click "Install"
   - Extension ID: `ms-vscode.cpptools`

3. **After Installation:**
   - Reload the window (`Cmd+Shift+P` → "Developer: Reload Window")
   - Wait 2-5 minutes for IntelliSense to index the codebase
   - Check status bar (bottom right) for "Parsing..." or "Indexing..."

4. **Test:**
   - Command+Click on `MongoExtension` in `mongo_extension.cpp` (line 34)
   - Should navigate to `src/include/mongo_extension.hpp`

## Option 2: clangd Extension (Alternative, often better for large codebases)

1. **Install clangd:**
   ```bash
   brew install llvm
   ```

2. **Install Extension:**
   - Press `Cmd+Shift+X`
   - Search for: `clangd` (by LLVM)
   - Click "Install"
   - Extension ID: `llvm-vs-code-extensions.vscode-clangd`

3. **Disable Microsoft C/C++ Extension:**
   - If you installed both, disable `ms-vscode.cpptools` to avoid conflicts
   - Press `Cmd+Shift+P` → "Extensions: Disable (Workspace)" → select "C/C++"

4. **Configure clangd:**
   - clangd will automatically use `compile_commands.json`
   - It should work immediately after installation

5. **Test:**
   - Command+Click on `MongoExtension` in `mongo_extension.cpp` (line 34)
   - Should navigate to `src/include/mongo_extension.hpp`

## Which to Choose?

- **Microsoft C/C++ Extension**: Better for IntelliSense error checking, but slower indexing
- **clangd**: Faster, more accurate, better for large codebases like DuckDB

For this project, **clangd is recommended** because:
- DuckDB codebase is large (~22MB)
- clangd handles `compile_commands.json` better
- Faster symbol navigation

