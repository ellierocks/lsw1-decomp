#!/bin/bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
PATCH_FILE="$SCRIPT_DIR/patches/dtk-skip-invalid-extab.patch"
SRC_DIR="${1:-${DTK_SOURCE_DIR:-/tmp/decomp-toolkit}}"

if [ ! -f "$SRC_DIR/Cargo.toml" ]; then
    echo "DTK source checkout not found: $SRC_DIR" >&2
    echo "Usage: $0 /path/to/decomp-toolkit" >&2
    exit 1
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
