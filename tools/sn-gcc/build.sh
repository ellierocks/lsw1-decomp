#!/usr/bin/env bash
# Build SN Systems' GCC 2.95.3 "SN BUILD v1.79 for Nintendo Gamecube" cc1/cc1plus natively for
# Linux from SN's GPL source release. This is an *isolated evaluation* compiler: it never touches
# build/compilers/ProDG/3.9.3 (the pinned v1.76 baseline) and never deletes anything.
#
#   SN_GCC_SRC=<extracted NGC_GNU_SRC/NGC> tools/sn-gcc/build.sh OUT_DIR [PATCH ...]
#
# PATCH is a name in tools/sn-gcc/patches (without .patch), applied after linux-host:
#   alloc-diagnostics        print-only GDBG/LADBG allocation hooks (output unchanged when unset)
#   shipped-build-mem-flags  EXPERIMENTAL RE4 alias hypothesis; not adopted for LSW1
# OUT_DIR gets cc1, cc1plus, BUILD_INFO and a work/ tree. A work/ tree built with a different
# patch list is refused; pick a new OUT_DIR instead of overwriting.
#
# Adapted from adonis-singh/re4 tools/sn-gcc/build.sh (CC0). Provenance and findings:
# docs/code_matching_workflow.md ("Native SN GCC v1.79 evaluation").
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
DROP="${SN_GCC_SRC:?set SN_GCC_SRC to the extracted NGC_GNU_SRC/NGC directory}"
OUT="${1:?usage: build.sh OUT_DIR [PATCH ...]}"
shift
EXTRA=("$@")

[ -d "$DROP/gcc/cp" ] || { echo "source drop not found at $DROP" >&2; exit 1; }
for name in "${EXTRA[@]}"; do
    [ -f "$HERE/patches/$name.patch" ] && [ "$name" != linux-host ] ||
        { echo "unknown patch: $name" >&2; exit 1; }
done

mkdir -p "$OUT"
OUT="$(cd "$OUT" && pwd)"
WORK="$OUT/work"
STAMP="linux-host ${EXTRA[*]:-}"

if [ -e "$WORK/src" ]; then
    [ "$(cat "$WORK/.patches" 2>/dev/null)" = "$STAMP" ] ||
        { echo "$WORK was built with a different patch list; use a new OUT_DIR" >&2; exit 1; }
else
    mkdir -p "$WORK"
    cp -r "$DROP/gcc" "$DROP/include" "$WORK/" 
    mkdir "$WORK/src" && mv "$WORK/gcc" "$WORK/include" "$WORK/src/"
    rm -rf "$WORK/src/gcc/ch" # CHILL front end, not used
    chmod -R u+w "$WORK/src"
    # The rtl reader chokes on '\r' in rs6000.md, so normalise line endings like RE4 does.
    find "$WORK/src" -type f \( -name '*.c' -o -name '*.h' -o -name '*.md' -o -name '*.def' \
        -o -name '*.y' -o -name '*.cc' -o -name '*.cpp' -o -name '*.in' \) -print0 |
        xargs -0 sed -i 's/\r$//'
    (cd "$WORK/src" && patch -p1 < "$HERE/patches/linux-host.patch")
    for name in "${EXTRA[@]}"; do
        case "$name" in
            alloc-diagnostics) (cd "$WORK" && patch -p0 < "$HERE/patches/$name.patch") ;;
            *) (cd "$WORK/src" && patch -p1 < "$HERE/patches/$name.patch") ;;
        esac
    done
    echo "$STAMP" > "$WORK/.patches"
fi

cp "$HERE/Makefile" "$WORK/Makefile"
make -C "$WORK" -j"$(nproc)" all
cp "$WORK/cc1" "$WORK/cc1plus" "$OUT/"

{
    echo "built: $(date -u +%FT%TZ)"
    echo "host cc: $(${CC:-gcc} --version | head -1)"
    echo "patches: $STAMP"
    (cd "$HERE/patches" && sha256sum linux-host.patch "${EXTRA[@]/%/.patch}" 2>/dev/null)
    (cd "$OUT" && sha256sum cc1 cc1plus)
    echo "cc1: $("$OUT/cc1" -version -quiet /dev/null -o /dev/null 2>&1 | head -1)"
} | tee "$OUT/BUILD_INFO"
