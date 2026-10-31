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
from typing import Any, Dict, List

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
    "-i src/dolphin/gx", "-i src/dolphin/os", "-i src/dolphin/dsp", "-i src/dolphin/ax", "-i src/dolphin/card", "-i src/dolphin",
]


def sdk_cflags(unit: str) -> List[str]:
    overrides = {}
    if unit.startswith("dvd/") or unit in {"mtx/mtx", "mtx/mtx44", "card/CARDOpen"}:
        overrides["-char unsigned"] = "-char signed"
    if unit == "exi/EXIBios":
        overrides["-O4,p"] = "-O3,p"
    return [overrides.get(flag, flag) for flag in cflags_mw_sdk]


def SDKObject(unit: str, matching: bool, external_bss: bool = False) -> Object:
    name = f"dolphin/{unit}.c"
    target = config.out_path() / "obj" / f"dolphin/{unit}.o"
    symbols = Path("config/GL5E4F/symbols.txt")
    flag = f" --external-bss {symbols}" if external_bss else ""
    return Object(
        matching, name, cflags=sdk_cflags(unit),
        post_build=[
            "{dtk} elf fixup {out} {out}",
            f"$python tools/strip_unused.py --target {target}{flag} {{out}}",
        ],
        post_build_implicit=[Path("tools/strip_unused.py"), target] + ([symbols] if external_bss else []),
    )


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
            Object(NonMatching, "numath/nuvec.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "numath/numtx.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            Object(NonMatching, "numath/nu_asm.c"),
            Object(NonMatching, "nu3dx/nuanim.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg),
            # Auto-generated fallback units (auto_NN_*) come from dtk's config.json;
            # native split produces their asm and objects, so they are not listed here.
        ],
    },
    DolphinLib("mtx", [
        SDKObject("mtx/mtx", Matching),
        SDKObject("mtx/mtxvec", Matching),
        SDKObject("mtx/mtx44", Matching),
    ]),
    DolphinLib("base", [SDKObject("base/PPCArch", Matching)]),
    DolphinLib("os", [
        SDKObject("os/OSAlloc", Matching),
        SDKObject("os/OSFont", Matching, external_bss=True),
        SDKObject("os/OSLink", Matching),
        SDKObject("os/OSMessage", Matching),
        SDKObject("os/OSReboot", Matching),
        SDKObject("os/OSAlarm", Matching),
        SDKObject("os/OSContext", Matching),
        SDKObject("os/OSArena", Matching),
        SDKObject("os/OSAudioSystem", Matching),
        SDKObject("os/OSError", Matching, external_bss=True),
        SDKObject("os/OSReset", Matching, external_bss=True),
        SDKObject("os/OSResetSW", Matching, external_bss=True),
        SDKObject("os/OSRtc", Matching, external_bss=True),
        SDKObject("os/OSSemaphore", Matching),
        SDKObject("os/OSSync", Matching),
        SDKObject("os/OSTime", Matching),
        SDKObject("os/OSMutex", Matching),
        SDKObject("os/OSCache", Matching),
        SDKObject("os/OSInterrupt", Matching),
        SDKObject("os/OSMemory", Matching),
        SDKObject("os/OSFatal", Matching, external_bss=True),
    ]),
    DolphinLib("exi", [SDKObject("exi/EXIUart", Matching)]),
    DolphinLib("si", [SDKObject("si/SIBios", Matching, external_bss=True), SDKObject("si/SISamplingRate", Matching),
                      Object(NonMatching, "dolphin/si/SISamplingRateBSS.c")]),
    DolphinLib("db", [SDKObject("db/db", Matching)]),
    DolphinLib("pad", [SDKObject("pad/Padclamp", Matching)]),
    DolphinLib("dvd", [SDKObject("dvd/dvdfs", Matching, external_bss=True), SDKObject("dvd/dvd", Matching, external_bss=True), SDKObject("dvd/dvdqueue", Matching, external_bss=True), SDKObject("dvd/dvderror", Matching), SDKObject("dvd/dvdidutils", Matching), SDKObject("dvd/dvdFatal", Matching), SDKObject("dvd/fstload", Matching), SDKObject("dvd/dvdlow", Matching, external_bss=True)]),
    DolphinLib("ai", [SDKObject("ai/ai", Matching)]),
    DolphinLib("ar", [SDKObject("ar/ar", Matching), SDKObject("ar/arq", Matching)]),
    DolphinLib("ax", [
        SDKObject("ax/AX", Matching),
        SDKObject("ax/AXAlloc", Matching, external_bss=True),
        SDKObject("ax/AXAux", Matching, external_bss=True),
        SDKObject("ax/AXCL", Matching, external_bss=True),
        SDKObject("ax/AXOut", Matching, external_bss=True),
        SDKObject("ax/AXSPB", Matching, external_bss=True),
        SDKObject("ax/AXVPB", Matching, external_bss=True),
        SDKObject("ax/AXComp", Matching),
        SDKObject("ax/AXProf", Matching),
    ]),
    DolphinLib("dsp", [
        SDKObject("dsp/dsp", Matching),
        SDKObject("dsp/dsp_debug", Matching),
        SDKObject("dsp/dsp_task", Matching),
    ]),
    DolphinLib("card", [
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
    DolphinLib("gx", [
        SDKObject("gx/GXAttr", Matching),
        SDKObject("gx/GXInit", Matching, external_bss=True),
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
