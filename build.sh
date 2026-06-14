#!/bin/bash
# Build helper script for LEGO Star Wars decompilation
# Handles the dtk split workaround automatically

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "=== LEGO Star Wars Decompilation Build ==="
echo

DTK_BIN="build/tools/dtk"
if [ -f "build/tools/dtk_patched" ]; then
    DTK_BIN="build/tools/dtk_patched"
fi

# Clean build if requested
if [ "$1" = "clean" ]; then
    echo "Cleaning build directory..."
    rm -rf build/
fi

# Step 1: Configure and download tools
echo "[1/3] Running configure.py and downloading tools..."
python3 configure.py
# Download tools by running ninja with a tools target
if [ ! -f "build/tools/dtk" ]; then
    echo "Downloading build tools (first time only)..."
    ninja build/tools/dtk build/tools/objdiff-cli build/tools/wibo build/binutils build/compilers 2>&1 | grep -E "(Downloading|TOOL)" || true
fi

if [ "$DTK_BIN" = "build/tools/dtk_patched" ]; then
    echo "Using patched dtk: $DTK_BIN"
fi

# Step 2: Check if split is needed
if [ ! -f "build/GL5E4F/config.json" ] || \
   [ "config/GL5E4F/config.yml" -nt "build/GL5E4F/config.json" ] || \
   [ "config/GL5E4F/splits.txt" -nt "build/GL5E4F/config.json" ]; then
    echo "[2/3] Running dtk split (this takes ~3 minutes)..."
    echo "Note: dtk may show warnings/errors but will complete successfully."
    echo
    
    # Run split - wait for it to complete
    if ! timeout 300 "$DTK_BIN" dol split config/GL5E4F/config.yml build/GL5E4F 2>&1; then
        echo "Warning: dtk split may have encountered issues, but continuing..."
    fi
    
    # Fix symbols.txt if it has duplicates
    if [ -f "config/GL5E4F/symbols.txt" ]; then
        echo
        echo "Fixing symbols.txt..."
        python3 tools/fix_symbols.py
        
        echo "Applying documented symbol names..."
        python3 tools/add_documented_symbols.py
        
        echo "Applying batch string renames..."
        python3 tools/batch_rename_strings.py
    fi

    echo "Patching config.json from splits.txt..."
    python3 tools/synth_config_from_splits.py
    
    echo
    echo "Split complete! Object files are in build/GL5E4F/obj/"
    echo
    echo "NOTE: Due to a dtk bug, ninja may try to re-run the split."
    echo "To build, use: ninja -t commands build/GL5E4F/main.dol"
    echo "Or manually assemble and link the object files."
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
