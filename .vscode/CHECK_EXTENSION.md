# Check if C/C++ Extension is Installed

## Step 1: Verify Extension is Installed

1. **Open Extensions Panel:**
   - Press `Cmd+Shift+X` (or View → Extensions)

2. **Search for C/C++:**
   - Type: `C/C++` in the search box
   - Look for: **"C/C++" by Microsoft** (Extension ID: `ms-vscode.cpptools`)

3. **Check Status:**
   - If you see **"Install"** button → Extension is NOT installed
   - If you see **"Disable"** or **"Uninstall"** → Extension IS installed
   - If you see **"Enable"** → Extension is installed but DISABLED

## Step 2: Install the Extension (if needed)

1. **Click "Install"** on the C/C++ extension by Microsoft
2. **Wait for installation** to complete
3. **Reload Window:**
   - Press `Cmd+Shift+P`
   - Type: `Developer: Reload Window`
   - Press Enter

## Step 3: Verify Extension is Active

After reloading, check:

1. **Status Bar (bottom right):**
   - You should see "C/C++" or "IntelliSense" status
   - May show "Parsing..." or "Indexing..."

2. **Command Palette:**
   - Press `Cmd+Shift+P`
   - Type: `C/C++`
   - You should see commands like:
     - `C/C++: Reset IntelliSense Database`
     - `C/C++: Select a Configuration...`
     - `C/C++: Log Diagnostics`

## If Commands Still Don't Appear

The extension might not be activated. Try:

1. **Disable and Re-enable:**
   - Press `Cmd+Shift+X`
   - Find "C/C++" (Microsoft)
   - Click "Disable" (or "Disable (Workspace)")
   - Click "Enable" again
   - Reload Window

2. **Check Output Panel:**
   - Press `Cmd+Shift+U` (View → Output)
   - Select "C/C++" from dropdown
   - Look for error messages

3. **Alternative: Use clangd**
   - If C/C++ extension continues to have issues, try clangd instead
   - See `.vscode/INSTALL_EXTENSIONS.md` for instructions



