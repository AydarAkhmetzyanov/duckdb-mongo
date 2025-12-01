# Force IntelliSense to Activate

You're seeing C/C++ commands but NOT the IntelliSense-specific ones. This means the extension is loaded but IntelliSense hasn't started.

## Step 1: Check C/C++ Output Logs

1. **Open Output Panel:**
   - Press `Cmd+Shift+U` (View → Output)

2. **Select "C/C++" from dropdown:**
   - At the top of the Output panel, click the dropdown
   - Select "C/C++"

3. **Look for:**
   - "IntelliSense Engine starting..."
   - "Initializing IntelliSense..."
   - Any error messages
   - Configuration loading messages

## Step 2: Force IntelliSense to Start

1. **Open a C++ file:**
   - Make sure `mongo_table_function.cpp` is open (it should be)

2. **Wait 30 seconds** for IntelliSense to detect the file

3. **Check Status Bar (bottom right):**
   - Look for "C/C++" or "IntelliSense" status
   - May show "Parsing..." or "Indexing..."

## Step 3: Try Selecting Configuration

Even though you don't see "Reset IntelliSense Database", try:

1. **Press `Cmd+Shift+P`**
2. **Type:** `C/C++: Select a Configuration...`
3. **If it appears, select "Mac"**

## Step 4: Check if IntelliSense is Actually Working

Even without the commands, IntelliSense might be working:

1. **Hover over a symbol:**
   - Hover over `MongoScanData` in `mongo_table_function.cpp`
   - Do you see a tooltip with definition?

2. **Try autocomplete:**
   - Type `MongoScan` and press `Ctrl+Space`
   - Do you see suggestions?

3. **Check for red squiggles:**
   - Are there red underlines on errors?
   - If yes, IntelliSense IS working (just commands are missing)

## Step 5: Alternative - Use clangd Instead

If Microsoft C/C++ extension continues to have issues, try clangd:

1. **Install clangd:**
   ```bash
   brew install llvm
   ```

2. **Install clangd Extension:**
   - Press `Cmd+Shift+X`
   - Search for: `clangd` (by LLVM)
   - Click "Install"

3. **Disable Microsoft C/C++ Extension:**
   - Press `Cmd+Shift+X`
   - Find "C/C++" (Microsoft)
   - Click "Disable (Workspace)"

4. **Reload Window:**
   - `Cmd+Shift+P` → "Developer: Reload Window"

5. **clangd will automatically use `compile_commands.json`**

clangd is often better for large codebases like DuckDB and provides "Go to Definition" immediately.


