#!/usr/bin/env python3

###
# Generates build files for the project.
# This file also includes the project configuration,
# such as compiler flags and the object matching status.
#
# Usage:
#   python3 configure.py
#   ninja
#
# Append --help to see available options.
###

import argparse
import sys
from pathlib import Path
from typing import Any, Dict, List, Optional

from tools.project import (
    Object,
    ProgressCategory,
    ProjectConfig,
    calculate_progress,
    generate_build,
    is_windows,
)

# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "GL5E4F",  # 0 - LEGO Star Wars: The Video Game (GameCube, NTSC-U)
]

parser = argparse.ArgumentParser()
parser.add_argument(
    "mode",
    choices=["configure", "progress"],
    default="configure",
    help="script mode (default: configure)",
    nargs="?",
)
parser.add_argument(
    "-v",
    "--version",
    choices=VERSIONS,
    type=str.upper,
    default=VERSIONS[DEFAULT_VERSION],
    help="version to build",
)
parser.add_argument(
    "--build-dir",
    metavar="DIR",
    type=Path,
    default=Path("build"),
    help="base build directory (default: build)",
)
parser.add_argument(
    "--binutils",
    metavar="BINARY",
    type=Path,
    help="path to binutils (optional)",
)
parser.add_argument(
    "--compilers",
    metavar="DIR",
    type=Path,
    help="path to compilers (optional)",
)
parser.add_argument(
    "--map",
    action="store_true",
    help="generate map file(s)",
)
parser.add_argument(
    "--debug",
    action="store_true",
    help="build with debug info (non-matching)",
)
if not is_windows():
    parser.add_argument(
        "--wrapper",
        metavar="BINARY",
        type=Path,
        help="path to wibo or wine (optional)",
    )
parser.add_argument(
    "--dtk",
    metavar="BINARY | DIR",
    type=Path,
    help="path to decomp-toolkit binary or source (optional)",
)
parser.add_argument(
    "--objdiff",
    metavar="BINARY | DIR",
    type=Path,
    help="path to objdiff-cli binary or source (optional)",
)
parser.add_argument(
    "--sjiswrap",
    metavar="EXE",
    type=Path,
    help="path to sjiswrap.exe (optional)",
)
parser.add_argument(
    "--ninja",
    metavar="BINARY",
    type=Path,
    help="path to ninja binary (optional)",
)
parser.add_argument(
    "--verbose",
    action="store_true",
    help="print verbose output",
)
parser.add_argument(
    "--non-matching",
    dest="non_matching",
    action="store_true",
    help="builds equivalent (but non-matching) or modded objects",
)
parser.add_argument(
    "--warn",
    dest="warn",
    type=str,
    choices=["all", "off", "error"],
    help="how to handle warnings",
)
parser.add_argument(
    "--no-progress",
    dest="progress",
    action="store_false",
    help="disable progress calculation",
)
parser.add_argument(
    "--prodg-cc1-native",
    type=Path,
    help="evaluate a native cc1 (tools/sn-gcc/build.sh) instead of the pinned cc1.exe",
)
parser.add_argument(
    "--prodg-arg",
    action="append",
    default=[],
    metavar="ARG",
    help="extra tools/prodg_cc.py argument for ProDG units (repeatable; evaluation only)",
)
args = parser.parse_args()

config = ProjectConfig()
config.version = str(args.version)
version_num = VERSIONS.index(config.version)

patched_dtk = Path("build/tools/dtk_patched")

# Apply arguments
config.build_dir = args.build_dir
config.dtk_path = args.dtk
config.objdiff_path = args.objdiff
config.binutils_path = args.binutils
config.compilers_path = args.compilers
config.generate_map = args.map
if args.prodg_cc1_native:
    config.prodg_extra_args += ["--cc1-native", str(args.prodg_cc1_native)]
    config.prodg_extra_inputs.append(args.prodg_cc1_native)
config.prodg_extra_args += args.prodg_arg
config.non_matching = args.non_matching
config.sjiswrap_path = args.sjiswrap
config.ninja_path = args.ninja
config.progress = args.progress
if not is_windows():
    config.wrapper = args.wrapper
# Prefer a locally built patched DTK when the user did not override --dtk.
if config.dtk_path is None and patched_dtk.is_file():
    config.dtk_path = patched_dtk
# Don't build asm unless we're --non-matching
if not config.non_matching:
    config.asm_dir = None

# Tool versions
config.binutils_tag = "2.42-2"
config.compilers_tag = "20251118"
config.dtk_tag = "v1.8.3"
config.objdiff_tag = "v3.6.1"
config.sjiswrap_tag = "v1.2.2"
config.wibo_tag = "1.0.3"

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"
# Native `dtk dol split` now works on this ProDG DOL (see the patched dtk +
# symbols.txt OSTime fixes), so the synth_config_from_splits.py / make_target_obj.py
# workaround is retired: dtk produces config.json and per-unit target objects itself.
config.asflags = [
    "-mgekko",
    "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
    f"--defsym BUILD_VERSION={version_num}",
]
config.ldflags = []
if args.debug:
    config.ldflags.append("-g")  # Or -gdwarf-2 for Wii linkers

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []

# Optional numeric ID for decomp.me preset
# Can be overridden in libraries or objects
config.scratch_preset_id = None

# Base flags, common to most GC/Wii games.
# Generally leave untouched, with overrides added below.
cflags_base = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    "-O4,p",
    "-inline auto",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-multibyte",  # For Wii compilers, replace with `-enc SJIS`
    "-i src",
    "-i include",
    f"-i build/{config.version}/include",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION_{config.version}",
]

# Debug flags
if args.debug:
    # Or -sym dwarf-2 for Wii compilers
    cflags_base.extend(["-sym on", "-DDEBUG=1"])
else:
    cflags_base.append("-DNDEBUG=1")

# Warning flags
if args.warn == "all":
    cflags_base.append("-W all")
elif args.warn == "off":
    cflags_base.append("-W off")
elif args.warn == "error":
    cflags_base.append("-W error")

# Metrowerks library flags
cflags_runtime = [
    *cflags_base,
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    "-gccinc",
    "-common off",
    "-inline auto",
]

# REL flags
cflags_rel = [
    *cflags_base,
    "-sdata 0",
    "-sdata2 0",
]

# ProDG (SN Systems GCC) flags — the real LSW1 toolchain. These are GCC-style and
# are consumed by tools/prodg_cc.py (only preprocessor/codegen flags matter; any
# leftover mwcc-style flags are ignored). Objects opt in via compiler="prodg", compiler_version=PRODG_VERSION.
PRODG_VERSION = "ProDG/3.9.3"
cflags_prodg = [
    "-O2",
    "-I src",
    "-I include",
    f"-I build/{config.version}/include",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION_{config.version}",
    "-DDEBUG=1" if args.debug else "-DNDEBUG=1",
]

# SN runtime (newlib libc and libgcc DImode helpers, src/sn). Retail flags: `-O2 -mfast-cast`,
# newlib built with -fno-common, small data threshold 1024. The ProDG driver (ngccc) injects the
# target defines below before cc1 runs; the production prodg rule does not, and without
# `__GNUC__=2` the libgcc `mode(DI)` types and newlib's `_ANSI_ARGS_` paths compile differently.
# See docs/sn_runtime.md.
cflags_sn_target_defs = [
    "-D__GNUC__=2",
    "-D__GNUC_MINOR__=95",
    "-DPPC",
    "-D__PPC__",
    "-D__PPC",
    "-D__CHAR_UNSIGNED__",
    "-D_BIG_ENDIAN",
    "-D__BIG_ENDIAN__",
    "-D__PTRDIFF_TYPE__=int",
    "-D__SIZE_TYPE__=unsigned",
    "-D_CALL_SYSV",
    "-D__SN__",
    "-DSN_TARGET_NGC",
    "-D__NOASMPP__",
    "-D_LANGUAGE_C",
    "-D__LANGUAGE_C",
    "-DLANGUAGE_C",
]
# newlib's small-data threshold is below 34 bytes: vfscanf's 34-byte `basefix` and locale's
# 48-byte `lconv` are addressed with lis/addi, while `__mb_cur_max` and `_impure_ptr` use r13.
# `-G 8` is the eabi default. newlib was configured with --enable-newlib-mb (MB_CAPABLE):
# vfscanf calls _mbtowc_r and locale's .rodata holds "C-JIS", "C-EUCJP" and "C-SJIS".
cflags_sn_libc = [
    "-O2",
    "-mfast-cast",
    "-fno-common",
    "-G 8",
    "-I src/sn/include",
    "-DNDEBUG=1",
    "-DMB_CAPABLE",
    *cflags_sn_target_defs,
]
# libgcc keeps `-G 1024`: each division object's 256-byte `__clz_tab` sits in `.sdata2`.
cflags_sn_libgcc = [
    "-G 1024" if flag == "-G 8" else flag for flag in cflags_sn_libc if flag != "-DMB_CAPABLE"
] + ["-I src/sn/libgcc"]
# `__main` reads its 4-byte `initialized` with lis/lwz from .bss, not r13-relative .sbss, so
# that object was built below `-G 4`; `-G 0` emits it with .lcomm as retail has it.
cflags_sn_libgcc_main = ["-G 0" if flag == "-G 1024" else flag for flag in cflags_sn_libgcc]

config.linker_version = "GC/1.3.2"
# Use the retail SN linker; MWLD remains the fallback for template REL support.
config.prodg_linker_version = PRODG_VERSION
config.prodg_ldscript = Path("config") / config.version / "ldscript.ld"


# Nintendo's April 2004 release flags, as reconstructed by dolsdk2004/RE4.
cflags_mw_sdk = [
    "-nodefaults", "-proc gekko", "-fp hard", "-Cpp_exceptions off",
    "-enum int", "-warn pragmas", "-requireprotos", '-pragma "cats off"',
    "-O4,p", "-inline auto", "-D__GEKKO__", "-DSDK_REVISION=1", "-DRELEASE",
    "-char unsigned", "-I-", "-i include", "-i include/dolphin", "-i include/dolphin/libc",
    "-i src/dolphin/gx", "-i src/dolphin/os", "-i src/dolphin/dsp", "-i src/dolphin/ax", "-i src/dolphin/card", "-i src/dolphin/vi", "-i src/dolphin/pad", "-i src/dolphin",
]


def sdk_cflags(unit: str) -> List[str]:
    overrides = {}
    if unit.startswith("dvd/") or unit in {"mtx/mtx", "mtx/mtx44", "card/CARDOpen"}:
        overrides["-char unsigned"] = "-char signed"
    if unit == "exi/EXIBios":
        overrides["-O4,p"] = "-O3,p"
    flags = [overrides.get(flag, flag) for flag in cflags_mw_sdk]
    if unit == "Runtime/__init_cpp":
        # The retained constructor loop has unscheduled loads and two branch chains.
        flags += ["-schedule off", "-opt nopeep"]
    if unit == "Runtime/__start":
        # The reset-button helper loads the MMIO base after its stack prologue.
        flags += ["-schedule off"]
    return flags


def SDKObject(unit: str, matching: bool, external_bss: bool = False,
              mw_version: Optional[str] = None) -> Object:
    name = f"dolphin/{unit}.c"
    target = config.out_path() / "obj" / f"dolphin/{unit}.o"
    symbols = Path("config/GL5E4F/symbols.txt")
    flag = f" --external-bss {symbols}" if external_bss else ""
    extra = {"mw_version": mw_version} if mw_version else {}
    return Object(
        matching, name, cflags=sdk_cflags(unit), **extra,
        post_build=[
            "{dtk} elf fixup {out} {out}",
            f"$python tools/strip_unused.py --target {target}{flag} {{out}}",
        ],
        post_build_implicit=[Path("tools/strip_unused.py"), target] + ([symbols] if external_bss else []),
    )


def SnObject(path: str, cflags: List[str] = cflags_sn_libc, strip: bool = False) -> Object:
    """An SN runtime object under src/sn. `strip` reproduces the SN linker dropping functions
    nothing references (`_sprintf_r`, `_setlocale_r`, ...): tools/strip_unused.py keeps only
    the symbols of the unit's split target object."""
    extra = {}
    if strip:
        target = config.out_path() / "obj" / path.replace(".c", ".o")
        extra = {
            "post_build": [f"$python tools/strip_unused.py --target {target} {{out}}"],
            "post_build_implicit": [Path("tools/strip_unused.py"), target],
        }
    return Object(Matching, path, compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags, **extra)


def DolphinLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name, "mw_version": "GC/1.2.5n", "cflags": cflags_mw_sdk,
        "progress_category": "sdk", "objects": objects,
        "mwcc_depflag": "-MD", "shift_jis": False,
    }


# Helper function for REL script objects
def Rel(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.3.2",
        "cflags": cflags_rel,
        "progress_category": "game",
        "objects": objects,
    }


Matching = True                   # Object matches and should be linked
NonMatching = False               # Object does not match and should not be linked
Equivalent = config.non_matching  # Object should be linked when configured with --non-matching


# Object is only matching for specific versions
def MatchingFor(*versions):
    return config.version in versions


config.warn_missing_config = True
config.warn_missing_source = False
config.libs = [
    {
        "lib": "main",
        "mw_version": config.linker_version,
        "cflags": cflags_base,
        "progress_category": "game",
        "objects": [
            Object(Matching, "nucore/nufile.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(Matching, "nucore/nufile_size.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(Matching, "nucore/nufile_exists.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(Matching, "nucore/nufpar.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(Matching, "numath/nurand.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(Matching, "numath/nuvec.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(Matching, "numath/numtx.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(Matching, "numath/nu_asm.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "nu3dx/nuanim.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(Matching, "nucore/ngc/nuerror_ngc.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nu3d/nudlist_1.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nu3d/nudlist_2.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(Matching, "auto/numath/nutrig.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(Matching, "auto/numath/nugraph_1.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(Matching, "auto/numath/nugraph_2.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(Matching, "auto/numath/nugraph_3.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/numath/nugraph_4.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nucore/nustring.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nucore/nufpar.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nucore/numem.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nucore/rnc.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nucore/nuheap.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nucore/nufile_1.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(Matching, "auto/nucore/nufile_2.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nucore/nufile_3.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nucore/ngc/nufile_ngc_1.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(Matching, "auto/nucore/ngc/nufile_ngc_2.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nucore/nufilepak.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(Matching, "nucore/ngc/nuthread.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nu3d/ngc/nuqfnt_ngc.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nu3d/nutimebar.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nu3d/NuHGobjEval.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nu3d/ngc/nucamera.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/camera/socksys/socksys.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nungc/ps2dma.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nu3d/nuspecial.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nu3d/ngc/ngc_mtl.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nu3d/ngc/nugscn.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nu3d/ngc/nuhgobj.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(Matching, "auto/nu3d/ngc/numtl.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nu3d/ngc/nurndr.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nu3d/ngc/nustream.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nufmv/ngc/nufmvmtl.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nungc/ngcapi.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nusound3/ngc/NGCStreaming.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nusound3/ngc/nusound.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nu3d/ngc/ngc_video.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(Matching, "auto/nu3d/ngc/nugobj.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nu3d/ngc/nutex.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nu3d/nuqfnt.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nusound3/ngc/NGCSound.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nu3d/ngc/nudlistngc.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/nu3d/nutexanm.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/game/chris.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/game/creature.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/game/credits.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/game/gameai.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/ai/aisys/aisys.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/characters/apiobject/apiobject.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/game/cut.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/game/gameaiscript.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/game/levels.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/game/main.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/game/obstacle.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/game/pickups.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/game/podrace.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/game/turret.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/game/rtlif.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/gamelib/audio.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/gamelib/debris.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/gamelib/gcutscn.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/gamelib/ngc/saveload.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/gamelib/terrain.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/gui/gamemenu/apimenu.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/gui/gamemenu/apisave.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "auto/rtl/rtl.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            # Auto-generated fallback units (auto_NN_*) come from dtk's config.json;
            # native split produces their asm and objects, so they are not listed here.
        ],
    },
    DolphinLib("cw_runtime", [
        SDKObject("TRK_MINNOW_DOLPHIN/ppcdown", Matching),
        SDKObject("TRK_MINNOW_DOLPHIN/exception", Matching),
        Object(NonMatching, "dolphin/Runtime/cvt_fp2unsigned.c", compiler="prodg",
               compiler_version=PRODG_VERSION, cflags=cflags_prodg),
        SDKObject("Runtime/__start", Matching),
        SDKObject("Runtime/__init_fpu", Matching),
        SDKObject("Runtime/tealeaf", Matching),
        SDKObject("Runtime/__init_cpp", Matching),
    ]),
    DolphinLib("mtx", [
        SDKObject("mtx/mtx", Matching),
        SDKObject("mtx/mtxvec", Matching),
        SDKObject("mtx/mtx44", Matching),
    ]),
    DolphinLib("base", [SDKObject("base/PPCArch", Matching)]),
    DolphinLib("os", [
        SDKObject("os/__ppc_eabi_init", Matching),
        SDKObject("os/OS", Matching),
        SDKObject("os/OSAlloc", Matching),
        SDKObject("os/OSExec", Matching),
        SDKObject("os/OSFont", Matching),
        SDKObject("os/OSLink", Matching),
        SDKObject("os/OSMessage", Matching),
        SDKObject("os/OSReboot", Matching),
        SDKObject("os/OSAlarm", Matching),
        SDKObject("os/OSContext", Matching),
        SDKObject("os/OSArena", Matching),
        SDKObject("os/OSAudioSystem", Matching),
        SDKObject("os/OSError", Matching),
        SDKObject("os/OSReset", Matching, external_bss=True),
        SDKObject("os/OSResetSW", Matching),
        SDKObject("os/OSRtc", Matching),
        SDKObject("os/OSSemaphore", Matching),
        SDKObject("os/OSSync", Matching),
        SDKObject("os/OSThread", Matching),
        SDKObject("os/OSTime", Matching),
        SDKObject("os/OSMutex", Matching),
        SDKObject("os/OSCache", Matching),
        SDKObject("os/OSInterrupt", Matching),
        SDKObject("os/OSMemory", Matching),
        SDKObject("os/OSFatal", Matching),
    ]),
    DolphinLib("exi", [SDKObject("exi/EXIBios", Matching), SDKObject("exi/EXIUart", Matching)]),
    DolphinLib("si", [SDKObject("si/SIBios", Matching), SDKObject("si/SISamplingRate", Matching)]),
    DolphinLib("db", [SDKObject("db/db", Matching)]),
    DolphinLib("vi", [SDKObject("vi/vi", Matching)]),
    DolphinLib("pad", [SDKObject("pad/Padclamp", Matching), SDKObject("pad/Pad", Matching)]),
    DolphinLib("dvd", [SDKObject("dvd/dvdfs", Matching), SDKObject("dvd/dvd", Matching), SDKObject("dvd/dvdqueue", Matching), SDKObject("dvd/dvderror", Matching), SDKObject("dvd/dvdidutils", Matching), SDKObject("dvd/dvdFatal", Matching), SDKObject("dvd/fstload", Matching), SDKObject("dvd/dvdlow", Matching), SDKObject("dvd/dvdlow_test", Matching)]),
    DolphinLib("ai", [SDKObject("ai/ai", Matching)]),
    DolphinLib("ar", [SDKObject("ar/ar", Matching), SDKObject("ar/arq", Matching)]),
    DolphinLib("ax", [
        SDKObject("ax/AX", Matching),
        SDKObject("ax/AXAlloc", Matching),
        SDKObject("ax/AXAux", Matching),
        SDKObject("ax/AXCL", Matching),
        SDKObject("ax/AXOut", Matching),
        SDKObject("ax/AXSPB", Matching, external_bss=True),
        SDKObject("ax/AXVPB", Matching),
        SDKObject("ax/AXComp", Matching),
        SDKObject("ax/AXProf", Matching),
    ]),
    DolphinLib("dsp", [
        SDKObject("dsp/dsp", Matching),
        SDKObject("dsp/dsp_debug", Matching),
        SDKObject("dsp/dsp_task", Matching),
    ]),
    DolphinLib("card", [
        SDKObject("card/CARDBios", Matching),
        SDKObject("card/CARDBlock", Matching),
        SDKObject("card/CARDDir", Matching),
        SDKObject("card/CARDCheck", Matching),
        SDKObject("card/CARDMount", Matching),
        SDKObject("card/CARDFormat", Matching),
        SDKObject("card/CARDOpen", Matching),
        SDKObject("card/CARDCreate", Matching),
        SDKObject("card/CARDRead", Matching),
        SDKObject("card/CARDWrite", Matching),
        SDKObject("card/CARDDelete", Matching),
        SDKObject("card/CARDStat", Matching),
        SDKObject("card/CARDNet", Matching),
        SDKObject("card/CARDUnlock", Matching),
        SDKObject("card/CARDRdwr", Matching),
    ]),
    DolphinLib("amcstubs", [SDKObject("amcstubs/AmcExi2Stubs", Matching)]),
    DolphinLib("gx", [
        SDKObject("gx/GXAttr", Matching),
        SDKObject("gx/GXInit", Matching),
        SDKObject("gx/GXFifo", Matching),
        SDKObject("gx/GXMisc", Matching),
        SDKObject("gx/GXGeometry", Matching),
        SDKObject("gx/GXFrameBuf", Matching),
        SDKObject("gx/GXLight", Matching),
        SDKObject("gx/GXTexture", Matching),
        SDKObject("gx/GXBump", Matching),
        SDKObject("gx/GXTev", Matching),
        SDKObject("gx/GXPixel", Matching),
        SDKObject("gx/GXDisplayList", Matching, external_bss=True),
        SDKObject("gx/GXTransform", Matching),
        SDKObject("gx/GXPerf", Matching),
    ]),
    DolphinLib("hvqm4", [
        # Hudson's middleware was built with GC/1.2.5, not the SDK's 1.2.5n:
        # see docs/sdk_import.md#per-unit-compiler-override-gc125 (epilogue mtlr placement).
        SDKObject("hvqm4/hvqm4dec", Matching, mw_version="GC/1.2.5"),
        # Retail ships the sound decoders as a second archive (hvqm4decsnd.a)
        # of four TUs: see docs/sdk_import.md#hvqm4-sound-tus.
        SDKObject("hvqm4/HVQM4DecSnd", Matching, mw_version="GC/1.2.5"),
        SDKObject("hvqm4/HVQM4Adpcm", Matching, mw_version="GC/1.2.5"),
        SDKObject("hvqm4/HVQM4Pcm16", Matching, mw_version="GC/1.2.5"),
        SDKObject("hvqm4/HVQM4Adp8x", Matching, mw_version="GC/1.2.5"),
    ]),
    DolphinLib("OdemuExi2", [
        SDKObject("OdemuExi2/DebuggerDriver", Matching, external_bss=True),
    ]),
    {
        "lib": "sn_runtime",
        "mw_version": config.linker_version,
        "cflags": cflags_base,
        "progress_category": "runtime",
        "objects": [
            SnObject("sn/libc/ctype/isdigit.c"),
            SnObject("sn/libc/ctype/tolower.c"),
            SnObject("sn/libc/ctype/toupper.c"),
            SnObject("sn/libc/stdio/sprintf.c", strip=True),
            SnObject("sn/libc/stdio/sscanf.c"),
            SnObject("sn/libc/stdio/vsprintf.c", strip=True),
            SnObject("sn/libc/stdlib/atof.c"),
            SnObject("sn/libc/stdlib/exit.c"),
            SnObject("sn/libc/stdlib/mbtowc_r.c"),
            SnObject("sn/libc/stdlib/rand.c"),
            SnObject("sn/libc/stdlib/strtoul.c"),
            SnObject("sn/libc/string/memcmp.c"),
            SnObject("sn/libc/string/memcpy.c"),
            SnObject("sn/libc/string/memset.c"),
            SnObject("sn/libc/string/strcasecmp.c"),
            SnObject("sn/libc/string/strcat.c"),
            SnObject("sn/libc/string/strcmp.c"),
            SnObject("sn/libc/string/strcpy.c"),
            SnObject("sn/libc/string/strlen.c"),
            SnObject("sn/libc/string/strncmp.c"),
            SnObject("sn/libc/string/strncpy.c"),
            SnObject("sn/libc/string/strstr.c"),
            SnObject("sn/libc/stdlib/strtol.c"),
            SnObject("sn/libc/ctype/isspace.c"),
            SnObject("sn/libc/errno/errno.c"),
            SnObject("sn/libc/locale/locale.c", strip=True),
            SnObject("sn/libc/stdio/fread.c"),
            SnObject("sn/libc/stdio/refill.c"),
            SnObject("sn/libc/stdio/ungetc.c", strip=True),
            SnObject("sn/libc/string/memchr.c"),
            SnObject("sn/libc/stdio/fflush.c"),
            SnObject("sn/libc/stdio/findfp.c", strip=True),
            SnObject("sn/libc/stdio/fwalk.c"),
            SnObject("sn/libc/stdio/makebuf.c"),
            SnObject("sn/libc/stdio/stdio.c"),
            SnObject("sn/libc/reent/closer.c"),
            SnObject("sn/libc/reent/fstatr.c"),
            SnObject("sn/libc/reent/lseekr.c"),
            SnObject("sn/libc/reent/readr.c"),
            SnObject("sn/libc/reent/writer.c"),
            SnObject("sn/libgcc/__main.c", cflags_sn_libgcc_main, strip=True),
            SnObject("sn/libgcc/_ashldi3.c", cflags_sn_libgcc),
            SnObject("sn/libgcc/_ashrdi3.c", cflags_sn_libgcc),
            SnObject("sn/libgcc/_divdi3.c", cflags_sn_libgcc),
            SnObject("sn/libgcc/_lshrdi3.c", cflags_sn_libgcc),
            SnObject("sn/libgcc/_moddi3.c", cflags_sn_libgcc),
            SnObject("sn/libgcc/_udivdi3.c", cflags_sn_libgcc),
            SnObject("sn/libgcc/_umoddi3.c", cflags_sn_libgcc),
            SnObject("sn/libc/stdio/vprintf.c"),
            SnObject("sn/libc/string/memmove.c"),
        ],
    },
    {
        "lib": "Runtime.PPCEABI.H",
        "mw_version": config.linker_version,
        "cflags": cflags_runtime,
        "progress_category": "sdk",
        "objects": [
            Object(NonMatching, "Runtime.PPCEABI.H/global_destructor_chain.c"),
            Object(NonMatching, "Runtime.PPCEABI.H/__init_cpp_exceptions.cpp"),
        ],
    },
]


# Optional callback to adjust link order. This can be used to add, remove, or reorder objects.
# This is called once per module, with the module ID and the current link order.
#
# For example, this adds "dummy.c" to the end of the DOL link order if configured with --non-matching.
# "dummy.c" *must* be configured as a Matching (or Equivalent) object in order to be linked.
def link_order_callback(module_id: int, objects: List[str]) -> List[str]:
    # Don't modify the link order for matching builds
    if not config.non_matching:
        return objects
    if module_id == 0:  # DOL
        return objects + ["dummy.c"]
    return objects


# Uncomment to enable the link order callback.
# config.link_order_callback = link_order_callback


# Optional extra categories for progress tracking
# Adjust as desired for your project
config.progress_categories = [
    ProgressCategory("game", "Game Code"),
    ProgressCategory("sdk", "SDK Code"),
    ProgressCategory("runtime", "SN Runtime"),
]
config.progress_each_module = args.verbose
# Optional extra arguments to `objdiff-cli report generate`
config.progress_report_args = [
    # Compare relocation targets and data values. The default ("none") is the most
    # lenient and counts functions whose relocations point at the wrong symbol.
    "--config",
    "functionRelocDiffs=all",
]

if args.mode == "configure":
    # Write build.ninja and objdiff.json
    generate_build(config)
elif args.mode == "progress":
    # Print progress information
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
