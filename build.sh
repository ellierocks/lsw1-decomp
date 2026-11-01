#!/usr/bin/env bash
# Bootstrap tools, split the retail DOL and compile the comparison report.
set -euo pipefail
cd "$(dirname "$(realpath "$0")")"
# A fresh worktree has no retail DOL; bootstrap links it and the shared toolchain from the primary checkout.
if [[ ! -e orig/GL5E4F/sys/main.dol ]]; then
    bash tools/bootstrap.sh
fi
if [[ ! -f orig/GL5E4F/sys/main.dol ]]; then
    echo 'Missing orig/GL5E4F/sys/main.dol; see README.md for extraction.' >&2
    exit 1
fi
if [[ $(sha1sum orig/GL5E4F/sys/main.dol | cut -d' ' -f1) != 95cca08a19224775d1a8d6cc64601fb7d0080981 ]]; then
    echo 'Retail DOL SHA-1 mismatch.' >&2
    exit 1
fi
if [[ ! -f build/tools/dtk_patched ]]; then
    bash tools/build_patched_dtk.sh
fi
python3 configure.py
ninja build/GL5E4F/report.json
ninja build/GL5E4F/main.dol
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
python3 tools/matching_progress.py
