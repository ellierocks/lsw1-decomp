#!/usr/bin/env bash
# Clone the pinned matt-kempster/m2c into the shared cache (default ~/.cache/lsw1/m2c).
# M2C overrides the checkout. tools/draft_fn.py finds it automatically.
set -euo pipefail
COMMIT=c632946b8cfbe8fce442b522dba92dfa72f1f317
dest="${M2C:-${LSW1_CACHE:-$HOME/.cache/lsw1}/m2c}"
if [[ ! -d $dest/.git ]]; then
    mkdir -p "$(dirname "$dest")"
    git clone -q https://github.com/matt-kempster/m2c "$dest"
fi
if [[ $(git -C "$dest" rev-parse HEAD) != "$COMMIT" ]]; then
    git -C "$dest" fetch -q origin "$COMMIT" 2>/dev/null || true
    git -C "$dest" checkout -q "$COMMIT"
fi
echo "m2c $COMMIT at $dest"
