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
# Download stock tools by running ninja with a tools target
if [ ! -f "build/tools/dtk" ]; then
    echo "Downloading build tools (first time only)..."
    ninja build/tools/dtk build/tools/objdiff-cli build/tools/wibo build/binutils build/compilers 2>&1 | grep -E "(Downloading|TOOL)" || true
fi

# LSW1 needs a patched dtk (skips invalid ProDG extab entries; stock dtk panics).
# Build it from source the first time. Requires a Rust toolchain (cargo).
# See docs/ci_and_decomp_dev.md for the upstreaming plan to remove this fork.
if [ ! -f "build/tools/dtk_patched" ]; then
    echo "Building patched dtk (first time only; requires cargo)..."
    bash tools/build_patched_dtk.sh
    # Re-run configure so the build wires the patched dtk.
    python3 configure.py
fi

echo "Using native dtk dol split (patched for ProDG)"

# Step 2: Native split. configure.py emits a build.ninja whose SPLIT rule runs
# `dtk dol split`; ninja regenerates config.json (and the per-unit target objects)
# from config.yml/splits.txt/symbols.txt on demand, so just drive it through ninja.
echo "[2/3] Running dtk dol split via ninja..."
ninja build/GL5E4F/config.json

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
