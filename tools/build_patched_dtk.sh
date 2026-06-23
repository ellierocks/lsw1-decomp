#!/bin/bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
PATCH_FILE="$SCRIPT_DIR/patches/dtk-skip-invalid-extab.patch"
SRC_DIR="${1:-${DTK_SOURCE_DIR:-/tmp/decomp-toolkit}}"

# Pin to the exact decomp-toolkit commit the patch was authored against. LSW1
# needs a patched dtk that tolerates this ProDG binary's exception tables and
# its symbol map; see patches/dtk-skip-invalid-extab.patch and
# docs/ci_and_decomp_dev.md. (This is the commit baked into the working binary;
# the v1.8.3 *tag* has a different source layout the patch does not fit.)
DTK_COMMIT="${DTK_COMMIT:-e4219e7644fb7b96d920d5bc3d1d950f5569dcaf}"
DTK_REPO="${DTK_REPO:-https://github.com/encounter/decomp-toolkit}"

if [ ! -f "$SRC_DIR/Cargo.toml" ]; then
    echo "Fetching decomp-toolkit $DTK_COMMIT into $SRC_DIR"
    git init -q "$SRC_DIR"
    git -C "$SRC_DIR" remote add origin "$DTK_REPO" 2>/dev/null || true
    git -C "$SRC_DIR" fetch -q --depth 1 origin "$DTK_COMMIT"
    git -C "$SRC_DIR" checkout -q FETCH_HEAD
fi

if [ ! -f "$PATCH_FILE" ]; then
    echo "Patch file missing: $PATCH_FILE" >&2
    exit 1
fi

echo "Using DTK source: $SRC_DIR"

if git -C "$SRC_DIR" apply --check "$PATCH_FILE" >/dev/null 2>&1; then
    echo "Applying patch: $(basename "$PATCH_FILE")"
    git -C "$SRC_DIR" apply "$PATCH_FILE"
else
    if git -C "$SRC_DIR" apply --reverse --check "$PATCH_FILE" >/dev/null 2>&1; then
        echo "Patch already applied"
    else
        echo "Patch does not apply cleanly to $SRC_DIR" >&2
        exit 1
    fi
fi

echo "Building patched DTK..."
cargo +stable build --release --manifest-path "$SRC_DIR/Cargo.toml"

mkdir -p "$REPO_DIR/build/tools"
cp "$SRC_DIR/target/release/dtk" "$REPO_DIR/build/tools/dtk_patched"

echo "Installed patched DTK to build/tools/dtk_patched"
