#!/usr/bin/env bash
# Fetch the compilers and tools that tools/score_fn.py needs, without splitting or compiling the DOL.
# Run once per checkout; build.sh does the same and more.
#
# Worktrees share one toolchain: build/tools, build/compilers and build/binutils are symlinks into
# $LSW1_CACHE/toolchain (default ~/.cache/lsw1/toolchain), populated by the first checkout that builds
# them (or the primary checkout's, when it already has them). The retail DOL is linked from the primary checkout, and the pinned dolsdk2004 donor is cloned
# into the same cache. Existing shared tools are not re-downloaded; LSW1_REFRESH=1 refetches them. Set LSW1_CACHE to relocate it, or LSW1_NO_SHARE=1 for a private toolchain.
set -euo pipefail
cd "$(dirname "$(realpath "$0")")/.."
cache="${LSW1_CACHE:-$HOME/.cache/lsw1}"
shared=(tools compilers binutils)

if [[ -z ${LSW1_NO_SHARE:-} ]]; then
    mkdir -p build "$cache/toolchain"
    primary=$(git worktree list --porcelain | sed -n '1s/^worktree //p')
    for d in "${shared[@]}"; do
        [[ -e build/$d ]] && continue
        for src in "$cache/toolchain/$d" "$primary/build/$d"; do
            if [[ -d $src && ! -L $src && $src != "$PWD/build/$d" ]]; then
                ln -s "$src" "build/$d"
                break
            fi
        done
    done
    if [[ ! -e orig/GL5E4F/sys/main.dol ]]; then
        for src in "$primary/orig/GL5E4F/sys/main.dol" "$cache/main.dol"; do
            if [[ -f $src && $src != "$PWD/orig/GL5E4F/sys/main.dol" ]]; then
                mkdir -p orig/GL5E4F/sys
                ln -s "$src" orig/GL5E4F/sys/main.dol
                break
            fi
        done
    fi
fi

if [[ ! -f orig/GL5E4F/sys/main.dol ]]; then
    echo 'Missing orig/GL5E4F/sys/main.dol; see README.md for extraction. Fast scoring needs only the toolchain, but verify_fn needs the DOL.' >&2
fi
python3 configure.py
# A fresh worktree has no ninja log, so ninja would re-download shared tools that other worktrees
# are using. Skip the fetch when everything is already present; LSW1_REFRESH=1 forces it (after a pin bump).
if [[ -n ${LSW1_REFRESH:-} || ! -x build/tools/wibo || ! -x build/tools/objdiff-cli \
      || ! -f build/compilers/ProDG/3.9.3/ngcld.exe || ! -f build/compilers/GC/1.2.5n/mwcceppc.exe \
      || ! -x build/binutils/powerpc-eabi-nm ]]; then
    ninja build/tools/wibo build/compilers build/binutils build/tools/objdiff-cli
fi
if [[ ! -x build/tools/dtk_patched ]]; then
    bash tools/build_patched_dtk.sh
fi

if [[ -z ${LSW1_NO_SHARE:-} ]]; then
    for d in "${shared[@]}"; do
        if [[ -d build/$d && ! -L build/$d && ! -e $cache/toolchain/$d ]]; then
            mv "build/$d" "$cache/toolchain/$d"
            ln -s "$cache/toolchain/$d" "build/$d"
        fi
    done
fi
# Tell ninja the shared tools are current: a fresh checkout has no log entry for them and newer
# sources than their outputs, so any later ninja build would otherwise refetch them. Log lines are
# copied from the primary checkout, so a changed pin (different command) still triggers a fetch.
if [[ -z ${LSW1_NO_SHARE:-} && -z ${LSW1_REFRESH:-} && -n ${primary:-} && $primary != "$PWD" \
      && -f $primary/.ninja_log ]]; then
    tools_re='\tbuild/(tools/(wibo|objdiff-cli|sjiswrap\.exe)|compilers|binutils)\t'
    if ! grep -qP "$tools_re" .ninja_log 2>/dev/null; then
        [[ -f .ninja_log ]] || echo '# ninja log v7' > .ninja_log
        grep -P "$tools_re" "$primary/.ninja_log" >> .ninja_log || true
    fi
    touch -c build/tools/wibo build/tools/objdiff-cli build/tools/sjiswrap.exe build/compilers build/binutils
fi
bash tools/fetch_dolsdk.sh >/dev/null 2>&1 || echo 'note: dolsdk2004 donor not fetched; run tools/fetch_dolsdk.sh when online' >&2
echo 'Toolchain ready. Next: python3 tools/next_target.py'
