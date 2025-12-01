#!/bin/bash
# Reset IntelliSense Database for C/C++ Extension

echo "Resetting IntelliSense Database..."

# Remove IntelliSense database
rm -f .vscode/browse.vc.db
rm -f .vscode/browse.vc.db-shm
rm -f .vscode/browse.vc.db-wal
rm -rf .vscode/.browse*

echo "IntelliSense database removed."
echo ""
echo "Next steps:"
echo "1. In VS Code/Cursor, press Cmd+Shift+P"
echo "2. Type 'C/C++: Reset IntelliSense Database' and press Enter"
echo "3. Type 'Developer: Reload Window' and press Enter"
echo "4. Wait 2-5 minutes for IntelliSense to re-index"



