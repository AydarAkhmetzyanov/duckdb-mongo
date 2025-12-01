# Activate C/C++ Extension

The extension is installed but may not be active. Follow these steps:

## Step 1: Verify Extension is Enabled

1. **Open Extensions Panel:**
   - Press `Cmd+Shift+X` (or View → Extensions)

2. **Search for "C/C++":**
   - Type: `C/C++` in the search box
   - Find: **"C/C++" by Microsoft**

3. **Check the Status:**
   - If you see **"Enable"** button → Click it!
   - If you see **"Disable"** → Extension is already enabled
   - If you see **"Reload Required"** → Click it

## Step 2: Reload Window

After enabling:

1. **Press `Cmd+Shift+P`**
2. **Type:** `Developer: Reload Window`
3. **Press Enter**

## Step 3: Wait for Extension to Activate

1. **Check Status Bar (bottom right):**
   - You should see "C/C++" or "IntelliSense" status
   - May show "Initializing..." or "Parsing..."

2. **Check Command Palette:**
   - Press `Cmd+Shift+P`
   - Type: `C/C++`
   - You should NOW see commands like:
     - `C/C++: Reset IntelliSense Database` ✅
     - `C/C++: Select a Configuration...`
     - `C/C++: Log Diagnostics`

## Step 4: If Commands Still Don't Appear

Try this sequence:

1. **Disable Extension:**
   - Press `Cmd+Shift+X`
   - Find "C/C++" (Microsoft)
   - Click "Disable (Workspace)"

2. **Reload Window:**
   - `Cmd+Shift+P` → "Developer: Reload Window"

3. **Re-enable Extension:**
   - Press `Cmd+Shift+X`
   - Find "C/C++" (Microsoft)
   - Click "Enable (Workspace)"

4. **Reload Window again**

5. **Wait 30 seconds** for extension to initialize

6. **Check Command Palette again**

## Alternative: Check Output Panel

1. **Press `Cmd+Shift+U`** (View → Output)
2. **Select "C/C++"** from the dropdown
3. **Look for:**
   - "IntelliSense Engine starting..."
   - Any error messages
   - Configuration loading messages

If you see errors, share them and we can troubleshoot further.



