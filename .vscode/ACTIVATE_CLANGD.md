# Activate clangd Extension

The clangd extension is installed but **not activated**. Here's how to activate it:

## Method 1: Open a C++ File (Easiest)

clangd activates when you open a C++ file:

1. **Open a C++ file:**
   - You already have `src/mongo_table_function.cpp` open ✅
   - This should trigger clangd activation

2. **Wait 10 seconds** for clangd to start

3. **Check Output Panel:**
   - Press `Cmd+Shift+U` (View → Output)
   - Select "clangd" from dropdown
   - You should see clangd starting up

4. **Try commands again:**
   - Press `Cmd+Shift+P`
   - Type: `clangd.restart`
   - Should now be available!

## Method 2: Enable Extension Manually

1. **Open Extensions:**
   - Press `Cmd+Shift+X`

2. **Find clangd:**
   - Search for: `clangd`
   - Find: "clangd" by LLVM

3. **Check Status:**
   - If it shows **"Disable"** → it's enabled ✅
   - If it shows **"Enable"** → click it to enable

4. **Reload Window:**
   - Press `Cmd+Shift+P`
   - Type: `Developer: Reload Window`
   - Press Enter

5. **Wait 10 seconds** for clangd to activate

## Method 3: Check Extension Settings

1. **Open Settings:**
   - Press `Cmd+,` (Preferences → Settings)

2. **Search for:** `clangd`

3. **Check:**
   - `clangd.enabled` should be `true`
   - `clangd.path` should be set (we set it to `/usr/bin/clangd`)

4. **If disabled, enable it**

## Method 4: Force Activation

1. **Close Cursor completely**

2. **Reopen Cursor**

3. **Open:** `src/mongo_table_function.cpp`

4. **Wait 30 seconds** for clangd to activate

5. **Check Output Panel:**
   - Press `Cmd+Shift+U`
   - Select "clangd"
   - Should see clangd messages

## Check if clangd is Running

1. **Open Output Panel:**
   - Press `Cmd+Shift+U`
   - Select "clangd" from dropdown

2. **Look for:**
   - ✅ "LSP ready" or "Indexed" → clangd is active!
   - ❌ Nothing or errors → clangd not active

3. **If you see errors, share them**

## Alternative: Use Right-Click Menu

Even if commands don't work, try:

1. **Right-click** on `MongoScanData` (line 21)
2. **Select:** "Go to Definition"
3. **If this works:** clangd is active, just commands aren't showing

## If Nothing Works

The clangd extension might be incompatible with Cursor. In that case:

1. **Use the C++ extension** (but it has missing binaries)
2. **Or use VS Code** for C++ development
3. **Or use command-line:**
   ```bash
   clangd --check=src/mongo_table_function.cpp
   ```

**Try Method 1 first** (you already have the C++ file open, so clangd should activate automatically)!

