#!/usr/bin/env bash
# Fetch the compilers and tools that tools/score_fn.py needs, without splitting or compiling the DOL.
# Run once per checkout; build.sh does the same and more.
set -euo pipefail
cd "$(dirname "$(realpath "$0")")/.."
if [[ ! -f orig/GL5E4F/sys/main.dol ]]; then
    echo 'Missing orig/GL5E4F/sys/main.dol; see README.md for extraction. Fast scoring needs only the toolchain, but verify_fn needs the DOL.' >&2
fi
python3 configure.py
ninja build/tools/wibo build/compilers build/binutils build/tools/objdiff-cli
if [[ ! -x build/tools/dtk_patched ]]; then
    bash tools/build_patched_dtk.sh
fi
echo 'Toolchain ready. Next: python3 tools/next_target.py'
