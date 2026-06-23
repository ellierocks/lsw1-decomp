#!/bin/bash
# Build helper script for LEGO Star Wars decompilation
# Uses the project-side synthesized split configuration.

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "=== LEGO Star Wars Decompilation Build ==="
echo

# Clean build if requested
if [ "$1" = "clean" ]; then
    echo "Cleaning build directory..."
    rm -rf build/
fi

# Step 1: Configure and download tools
echo "[1/3] Running configure.py and downloading tools..."
python3 configure.py
# Download tools by running ninja with a tools target
if [ ! -f "build/tools/dtk_patched" ]; then
    echo "Downloading build tools (first time only)..."
    ninja build/tools/dtk_patched build/tools/objdiff-cli build/tools/wibo build/binutils build/compilers 2>&1 | grep -E "(Downloading|TOOL)" || true
fi

echo "Using synthesized split configuration"

# Step 2: Check if split is needed
if [ ! -f "build/GL5E4F/config.json" ] || \
   [ "config/GL5E4F/config.yml" -nt "build/GL5E4F/config.json" ] || \
   [ "config/GL5E4F/splits.txt" -nt "build/GL5E4F/config.json" ]; then
    echo "[2/3] Synthesizing config.json from splits.txt..."
    python3 tools/synth_config_from_splits.py
    
    echo
    echo "Configuration complete! Object files are in build/GL5E4F/obj/"
    echo
    echo "Ninja uses the same synthesized configuration and will not rerun dtk split."
else
    echo "[2/3] Split up to date, skipping..."
fi

# Step 3: Show status
echo "[3/3] Build status:"
echo
if [ -d "build/GL5E4F/obj" ]; then
    echo "Object files:"
    ls -lh build/GL5E4F/obj/*.o 2>/dev/null | awk '{print "  " $9 " (" $5 ")"}'
    echo
fi

if [ -f "build/GL5E4F/report.json" ]; then
    echo "Progress summary:"
    python3 -c "
import json
d = json.load(open('build/GL5E4F/report.json'))
measures = d.get('measures', {})
print(f\"  Total code: {int(measures.get('total_code', 0)):,} bytes\")
print(f\"  Total functions: {int(measures.get('total_functions', 0)):,}\")
print(f\"  Total units: {int(measures.get('total_units', 0))}\")
" 2>/dev/null || echo "  (progress info unavailable)"
fi

echo
echo "=== Build setup complete! ==="
echo
echo "Next steps:"
echo "  - Use objdiff to compare your code with the original"
echo "  - Start decompiling functions from build/GL5E4F/asm/"
echo "  - See README.md (Contributing section) for guidelines"
