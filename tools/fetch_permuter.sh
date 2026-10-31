#!/usr/bin/env bash
# Clone the pinned simonlindholm/decomp-permuter into the shared cache (default ~/.cache/lsw1/decomp-permuter).
# DECOMP_PERMUTER overrides the checkout. tools/permute_setup.py finds it automatically.
set -euo pipefail
COMMIT=8556c81d80d1c1af98a858c8f4dc951357f29139
dest="${DECOMP_PERMUTER:-${LSW1_CACHE:-$HOME/.cache/lsw1}/decomp-permuter}"
if [[ ! -d $dest/.git ]]; then
    mkdir -p "$(dirname "$dest")"
    git clone -q https://github.com/simonlindholm/decomp-permuter "$dest"
fi
if [[ $(git -C "$dest" rev-parse HEAD) != "$COMMIT" ]]; then
    git -C "$dest" fetch -q origin "$COMMIT" 2>/dev/null || true
    git -C "$dest" checkout -q "$COMMIT"
fi
echo "decomp-permuter $COMMIT at $dest"
