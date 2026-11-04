#!/usr/bin/env bash
# Clone the pinned matt-kempster/m2c into the shared cache (default ~/.cache/lsw1/m2c) and apply
# tools/patches/m2c-*.patch on top. M2C overrides the checkout. tools/draft_fn.py finds it
# automatically.
#
# Patch state is detected with `git apply --check` (forward = not applied, reverse = applied), so
# the script is idempotent and safe to rerun. The cache is shared between worktrees, so the whole
# update runs under a lock.
set -euo pipefail
COMMIT=c632946b8cfbe8fce442b522dba92dfa72f1f317
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
dest="${M2C:-${LSW1_CACHE:-$HOME/.cache/lsw1}/m2c}"

update() {
    if [[ ! -d $dest/.git ]]; then
        mkdir -p "$(dirname "$dest")"
        git clone -q https://github.com/matt-kempster/m2c "$dest"
    fi
    shopt -s nullglob
    patches=("$here"/patches/m2c-*.patch)
    if [[ $(git -C "$dest" rev-parse HEAD) != "$COMMIT" ]]; then
        # Drop our patches before moving HEAD so the checkout cannot trip over them.
        for p in "${patches[@]}"; do
            ! git -C "$dest" apply --check -R "$p" 2>/dev/null || git -C "$dest" apply -R "$p"
        done
        git -C "$dest" fetch -q origin "$COMMIT" 2>/dev/null || true
        git -C "$dest" checkout -q "$COMMIT"
    fi
    for p in "${patches[@]}"; do
        if git -C "$dest" apply --check -R "$p" 2>/dev/null; then
            continue # already applied
        elif git -C "$dest" apply --check "$p" 2>/dev/null; then
            git -C "$dest" apply "$p"
        else
            echo "error: $(basename "$p") neither applies to nor is applied in $dest" >&2
            echo "       (stray edits? inspect with git -C $dest diff)" >&2
            return 1
        fi
    done
}

mkdir -p "$(dirname "$dest")"
if command -v flock >/dev/null; then
    exec 9>"$(dirname "$dest")/.m2c.lock"
    flock 9
fi
update
echo "m2c $COMMIT at $dest (patched: $(cd "$here" && ls patches/m2c-*.patch 2>/dev/null | wc -l))"
