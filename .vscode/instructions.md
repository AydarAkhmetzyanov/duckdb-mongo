# VS Code IntelliSense Setup Instructions

To enable "Go to Definition" (Command+Click):

1. **Reload VS Code Window:**
   - Press `Cmd+Shift+P`
   - Type "Developer: Reload Window"
   - Press Enter

2. **Wait for IntelliSense to Index:**
   - Check the status bar (bottom right) for "Parsing..." or "Indexing..."
   - This may take 2-5 minutes for the DuckDB codebase
   - You can continue working while it indexes

3. **If it still doesn't work, try:**
   - Press `Cmd+Shift+P`
   - Type "C/C++: Reset IntelliSense Database"
   - Press Enter
   - Wait for re-indexing

4. **Verify the configuration is active:**
   - Press `Cmd+Shift+P`
   - Type "C/C++: Select a Configuration..."
   - Select "Mac" (should be the only option)

5. **Test navigation:**
   - Command+Click on `MongoExtension` in `mongo_extension.cpp` (line 34)
   - Should navigate to `src/include/mongo_extension.hpp`
