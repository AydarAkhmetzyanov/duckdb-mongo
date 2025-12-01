# Enable clangd Extension

The clangd extension **IS installed** but might not be **enabled** or **activated**. Here's how to fix it:

## Steps:

1. **Open Extensions:**
   - Press `Cmd+Shift+X` (or View → Extensions)

2. **Find clangd:**
   - Search for: `clangd`
   - Look for: **"clangd" by LLVM**
   - Check the status:
     - If it says **"Disable"** → it's enabled ✅
     - If it says **"Enable"** → click it to enable
     - If it says **"Reload Required"** → click it

3. **Enable if Needed:**
   - If you see "Enable" button, click it
   - Wait for it to enable

4. **Reload Window:**
   - Press `Cmd+Shift+P`
   - Type: `Developer: Reload Window`
   - Press Enter

5. **Wait 10 seconds** for clangd to activate

6. **Try Commands Again:**
   - Press `Cmd+Shift+P`
   - Type: `clangd`
   - You should now see commands:
     - `clangd: Restart language server`
     - `clangd: Show status`
     - `clangd: Toggle indexing`

## Alternative: Check Output Panel

1. **Open Output Panel:**
   - Press `Cmd+Shift+U` (View → Output)

2. **Select "clangd" from dropdown:**
   - If "clangd" appears in the dropdown → extension is active ✅
   - If "clangd" doesn't appear → extension is not active ❌

3. **If clangd output appears:**
   - Look for "LSP ready" or "Indexed" messages
   - If you see errors, share them

## If Commands Still Don't Appear

Try these alternatives:

### Option 1: Use Right-Click Menu
1. **Right-click** on a symbol (e.g., `MongoScanData`)
2. **Select:** "Go to Definition" or "Go to Declaration"
3. If this works, clangd is active!

### Option 2: Check Extension Status
1. **Press `Cmd+Shift+X`**
2. **Find clangd extension**
3. **Check:**
   - Is it **enabled**? (should show "Disable" button)
   - Are there any **error messages**?
   - Does it show **"Reload Required"**?

### Option 3: Reinstall Extension
1. **Uninstall clangd:**
   - Press `Cmd+Shift+X`
   - Find clangd extension
   - Click "Uninstall"

2. **Reload Window:**
   - `Cmd+Shift+P` → `Developer: Reload Window`

3. **Reinstall clangd:**
   - Press `Cmd+Shift+X`
   - Search for `clangd`
   - Click "Install"

4. **Reload Window again**

## Test After Enabling

1. **Open:** `src/mongo_table_function.cpp`
2. **Command+Click** on `MongoScanData` (line 21)
3. **Should navigate to:** `src/include/mongo_table_function.hpp`

If Command+Click works, clangd is active! 🎉

