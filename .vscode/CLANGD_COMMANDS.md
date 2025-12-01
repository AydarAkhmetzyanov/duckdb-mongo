# clangd Commands in Cursor

The clangd extension **IS installed**! The commands use **dots** (`.`) not colons (`:`).

## Try These Commands:

1. **Press `Cmd+Shift+P`**

2. **Type one of these:**
   - `clangd.restart` ← **Try this first!**
   - `clangd.activate`
   - `clangd.memoryUsage`
   - `clangd.typeHierarchy`

3. **Or just type:** `clangd` and you should see all clangd commands

## Quick Test:

1. **Restart clangd:**
   - Press `Cmd+Shift+P`
   - Type: `clangd.restart`
   - Press Enter

2. **Wait 30-60 seconds** for indexing

3. **Test Command+Click:**
   - Open `src/mongo_table_function.cpp`
   - Command+Click on `MongoScanData` (line 21)
   - Should navigate to header file

## Check clangd Output:

1. **Press `Cmd+Shift+U`** (View → Output)
2. **Select "clangd"** from dropdown
3. **Look for:**
   - ✅ "LSP ready" or "Indexed" messages
   - ❌ Error messages (if any)

## If Commands Still Don't Appear:

1. **Check Extension Status:**
   - Press `Cmd+Shift+X`
   - Search for `clangd`
   - Make sure it shows **"Disable"** (meaning it's enabled)

2. **Reload Window:**
   - Press `Cmd+Shift+P`
   - Type: `Developer: Reload Window`
   - Press Enter

3. **Try commands again**

## Alternative: Use Right-Click

If commands don't work, try:
1. **Right-click** on `MongoScanData` (line 21)
2. **Select:** "Go to Definition"
3. If this works, clangd is active!

**Try `clangd.restart` now!**

