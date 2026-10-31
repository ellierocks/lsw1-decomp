#!/usr/bin/env bash
# Clone the pinned doldecomp/dolsdk2004 donor into the shared cache (default ~/.cache/lsw1/dolsdk2004).
# The commit matches src/dolphin/README.md. sdk_probe.py and sdk_import_unit.py find it automatically.
set -euo pipefail
COMMIT=2328b4164b1a98422a2255d83ce9a5a7548990cc
dest="${DOLSDK2004:-${LSW1_CACHE:-$HOME/.cache/lsw1}/dolsdk2004}"
if [[ ! -d $dest/.git ]]; then
    mkdir -p "$(dirname "$dest")"
    git clone -q https://github.com/doldecomp/dolsdk2004 "$dest"
fi
if [[ $(git -C "$dest" rev-parse HEAD) != "$COMMIT" ]]; then
    git -C "$dest" fetch -q origin "$COMMIT" 2>/dev/null || true
    git -C "$dest" checkout -q "$COMMIT"
fi
echo "dolsdk2004 $COMMIT at $dest"
