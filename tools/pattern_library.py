#!/usr/bin/env python3
"""Search already-matched ProDG code for the C that produces a given instruction shape.

Two corpora share LSW1's compiler (ProDG 3.9.3, GCC 2.95 SN BUILD v1.76, -O2):

  re4   adonis-singh/re4, whose every DOL and REL unit matches its retail bytes
        (C++ game code plus SN newlib C; -O0 sound units are skipped).
  lsw1  this project's own functions that objdiff reports at 100%.

`build` compiles each corpus with `-gstabs` (which leaves the instruction stream
unchanged; the stabs only add line records), disassembles the objects with dtk in
the same syntax as build/GL5E4F/asm, and keeps a per-function index of
instructions and source lines under work/pattern_library/. The donor source and
the index stay local.

    python3 tools/pattern_library.py build [--corpus re4|lsw1|all] [-j N]
    python3 tools/pattern_library.py similar fn_8001E76C [--top 8]
    python3 tools/pattern_library.py window fn_8001E76C --at 0x8001E7A0 [--len 8]
    python3 tools/pattern_library.py window --seq "lwz;cmpwi;beq;lbz;extsb"
    python3 tools/pattern_library.py show re4:game/cam_ctrl.cpp:IsChangeCamera__13CameraControl

`similar` ranks whole functions by instruction-shape similarity; `window` finds
corpus code containing the target's instruction run at an address (for example
the first mismatch verify_fn reports) and prints the source lines that produced
it. Hits are idioms to try, not evidence of identity: the target still has to
match through score_fn/verify_fn. RE4's matches were made with a native v1.79
cc1plus; a few -mfast-cast conversions differ from v1.76 by a dead `mr`.
"""

from __future__ import annotations

import argparse
import difflib
import gzip
import json
import os
import re
import subprocess
import sys
import tempfile
from collections import Counter, defaultdict
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
BUILD = ROOT / "build"
OUT = ROOT / "work" / "pattern_library"
WIBO = BUILD / "tools" / "wibo"
DTK = BUILD / "tools" / "dtk_patched"
AS = BUILD / "binutils" / "powerpc-eabi-as"
PRODG = BUILD / "compilers" / "ProDG" / "3.9.3"
TARGET_ASM = BUILD / "GL5E4F" / "asm"
REPORT = BUILD / "GL5E4F" / "report.json"


def re4_root() -> Path:
    cache = Path(os.environ.get("LSW1_CACHE", Path.home() / ".cache" / "lsw1"))
    return Path(os.environ.get("LSW1_RE4", cache / "donors" / "re4"))


RE4_URL = "https://github.com/adonis-singh/re4"

# What `ngccc.exe -c` passes to CPP (RE4 tools/ngccc.py); -O2 adds __OPTIMIZE__.
TARGET_DEFS = ["-D__OPTIMIZE__", "-D__GNUC__=2", "-D__GNUC_MINOR__=95", "-D__NOASMPP__"]
MACHINE_DEFS = ["-DPPC", "-D__PPC__", "-D__PPC", "-Acpu(powerpc)", "-Amachine(powerpc)",
                "-D__CHAR_UNSIGNED__", "-D_BIG_ENDIAN", "-D__BIG_ENDIAN__", "-Amachine(bigendian)",
                "-D__PTRDIFF_TYPE__=int", "-D__SIZE_TYPE__=unsigned", "-D_CALL_SYSV",
                "-DSN_TARGET_NGC", "-D__SN__"]
LANG_DEFS = {
    "c++": ["-D__GNUG__=2", "-lang-c++", "-D__cplusplus", *MACHINE_DEFS,
            "-D_LANGUAGE_C_PLUS_PLUS", "-D__LANGUAGE_C_PLUS_PLUS"],
    "c": ["-lang-c", *MACHINE_DEFS, "-D_LANGUAGE_C", "-D__LANGUAGE_C", "-DLANGUAGE_C"],
}
RE4_CPP_FLAGS = ["-DNDEBUG=1", "-DBUILD_VERSION=0", "-DVERSION_G4BE08",
                 "-Iinclude", "-Isrc", "-Ibuild/G4BE08/include"]
RE4_CC1_FLAGS = ["-O2", "-mfast-cast", "-gstabs"]

_FN = re.compile(r"^\.fn (\S+?),")
_ENDFN = re.compile(r"^\.endfn (\S+)")
_INSN = re.compile(r"^/\* ([0-9A-F]{8}) [0-9A-F]{8}  (?:[0-9A-F]{2} ){4}\*/\t(.*)$")


# ---------------------------------------------------------------- disassembly

def dtk_functions(text: str) -> dict[str, list[tuple[int, str]]]:
    """`.fn` blocks of a dtk listing: name -> [(address, instruction text)]."""
    funcs: dict[str, list[tuple[int, str]]] = {}
    cur = None
    for line in text.splitlines():
        m = _FN.match(line)
        if m:
            cur = funcs.setdefault(m.group(1), [])
            continue
        if _ENDFN.match(line):
            cur = None
            continue
        if cur is not None:
            m = _INSN.match(line)
            if m:
                cur.append((int(m.group(1), 16), m.group(2).strip()))
    return funcs


def disasm(obj: Path) -> dict[str, list[tuple[int, str]]]:
    with tempfile.TemporaryDirectory(prefix="patlib_") as tmp:
        out = Path(tmp) / "d.s"
        subprocess.run([str(DTK), "elf", "disasm", str(obj), str(out)],
                       capture_output=True, text=True, timeout=120)
        return dtk_functions(out.read_text(errors="replace")) if out.exists() else {}


def stabs_lines(asm: str) -> dict[str, list[tuple[str, int] | None]]:
    """Per function in a cc1 `-gstabs` listing: the (file, line) of each instruction."""
    funcs: dict[str, list] = {}
    main_file = cur_file = None
    cur = None
    line_no = None
    for raw in asm.replace("\r", "").splitlines():
        s = raw.strip()
        if s.startswith(".stabs"):
            m = re.match(r'\.stabs\s+"([^"]*)",\s*(\d+|0x[0-9a-f]+)', s)
            if m:
                kind = int(m.group(2), 0)
                if kind == 100 and m.group(1):          # N_SO: main source
                    main_file = cur_file = m.group(1)
                elif kind == 132:                        # N_SOL: included file
                    cur_file = m.group(1)
            continue
        if s.startswith(".stabn"):
            m = re.match(r"\.stabn\s+68,\s*0,\s*(\d+),", s)
            if m:
                line_no = int(m.group(1))
            continue
        if not s or s.startswith(".") or s.startswith("#"):
            continue
        if s.endswith(":") and not raw.startswith(("\t", " ")):
            name = s[:-1]
            if name in funcs or name.startswith(".") or name.startswith("gcc2_compiled"):
                continue
            funcs[name] = []
            cur = funcs[name]
            cur_file = main_file if cur_file is None else cur_file
            continue
        if s.endswith(":"):
            continue
        if raw.startswith(("\t", " ")) and cur is not None:
            cur.append((cur_file, line_no) if line_no is not None else None)
    return funcs


# ---------------------------------------------------------------- building

def run(cmd: list[str], cwd: Path, env: dict | None = None) -> str:
    try:
        p = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True, timeout=300, env=env)
    except subprocess.TimeoutExpired:
        return "timeout"
    return "" if p.returncode == 0 else (p.stdout + p.stderr)[-400:] or f"exit {p.returncode}"


def source_name(name: str, root: Path) -> str:
    """cc1 under wibo records `Z:/abs/path`; keep paths relative to the corpus root."""
    if re.match(r"^[A-Za-z]:[/\\]", name):
        name = name[2:]
    name = name.replace("\\", "/")
    try:
        return Path(name).relative_to(root).as_posix() if name.startswith("/") else name
    except ValueError:
        return name


def line_segments(source: Path, main: str) -> list[tuple[int, int, int, str]]:
    """`#line N "name"` segments of a source: (first physical, last physical, first logical, name).

    RE4 renumbers its sources to the retail __FILE__/__LINE__ values; the stabs carry those
    logical lines, so map them back to the line that is really in the file.
    """
    try:
        text = source.read_text(errors="replace").splitlines()
    except OSError:
        return []
    segs = [[1, len(text), 1, main]]
    for i, ln in enumerate(text, 1):
        m = re.match(r'^\s*#\s*line\s+(\d+)(?:\s+"([^"]*)")?', ln)
        if m:
            segs[-1][1] = i - 1
            segs.append([i + 1, len(text), int(m.group(1)), m.group(2) or segs[-1][3]])
    return [tuple(x) for x in segs]


def physical(segs, main: str, name: str, line: int) -> tuple[str, int] | None:
    base = Path(name.replace("\\", "/")).name
    for first, last, logical, seg_name in segs:
        if Path(seg_name.replace("\\", "/")).name != base:
            continue
        n = first + line - logical
        if first <= n <= last:
            return main, n
    return None


def index_object(corpus: str, unit: str, src_root: Path, obj: Path, asm: Path,
                 keep: set[str] | None = None, main: str | None = None) -> list[dict]:
    lines = stabs_lines(asm.read_text(errors="replace"))
    segs = line_segments(src_root / main, main) if main else []
    if len(segs) > 1:
        lines = {fn: [(physical(segs, main, *x) or x) if x else None for x in ln]
                 for fn, ln in lines.items()}
    entries = []
    for name, insns in disasm(obj).items():
        if keep is not None and name not in keep:
            continue
        if not insns:
            continue
        ln = lines.get(name)
        if ln is None or len(ln) != len(insns):
            ln = [None] * len(insns)       # line records do not line up: show asm only
        ln = [(source_name(f, src_root), n) if f else None for f, n in
              ((x if x else (None, None)) for x in ln)]
        files = sorted({f for f, _ in filter(None, ln)})
        entries.append({
            "id": f"{corpus}:{unit}:{name}", "corpus": corpus, "unit": unit, "name": name,
            "root": str(src_root), "insns": [t for _, t in insns],
            "files": files, "lines": [[files.index(x[0]), x[1]] if x else None for x in ln],
        })
    return entries


def re4_units(re4: Path) -> list[tuple[str, str, list[str]]]:
    """(source under src/, language, extra flags) for every matched RE4 unit, read as text."""
    cfg = re4 / "config" / "G4BE08"
    objects = (cfg / "objects.py").read_text()
    snd = set(re.findall(r'"(game/snd_[^"]+)"', objects.split("SND_UNITS", 1)[1])) \
        if "SND_UNITS" in objects else set()
    units: list[tuple[str, str, list[str]]] = []
    seen = set()
    for u in re.findall(r'^\s+"(game/[^"]+\.(?:cpp|c))",', objects, re.M):
        if u in seen or u in snd or not (re4 / "src" / u).exists():
            continue
        seen.add(u)
        if u.endswith(".c"):
            units.append((u, "c", [] if u == "game/errno.c" else ["-fno-common"]))
        else:
            units.append((u, "c++", []))
    modules = (cfg / "modules.py").read_text() if (cfg / "modules.py").exists() else ""
    mod_flags: dict[str, list[str]] = {}
    block = re.search(r"^CFLAGS = \{(.*?)^\}", modules, re.M | re.S)
    if block:
        for mod, flags in re.findall(r'"(\w+)":\s*\[([^\]]*)\]', block.group(1)):
            mod_flags[mod] = re.findall(r'"([^"]+)"', flags)
    for path in sorted((re4 / "src").rglob("*.cpp")):
        rel = path.relative_to(re4 / "src").as_posix()
        top = rel.split("/", 1)[0]
        if top in ("game", "lib") or rel in seen:
            continue
        seen.add(rel)
        units.append((rel, "c++", ["-G", "0", f"-DREL_MODULE={top}", *mod_flags.get(top, [])]))
    return units


def build_re4(jobs: int) -> list[dict]:
    re4 = re4_root()
    if not (re4 / "config" / "G4BE08" / "objects.py").exists():
        sys.exit(f"RE4 donor not found at {re4}; clone {RE4_URL} there or set LSW1_RE4")
    rev = subprocess.run(["git", "-C", str(re4), "rev-parse", "HEAD"],
                         capture_output=True, text=True).stdout.strip()
    work = OUT / "re4"
    work.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ, LANG="C", LC_ALL="C",
               C_INCLUDE_PATH=str(re4 / "include" / "prodg"),
               CPLUS_INCLUDE_PATH=str(re4 / "include" / "prodg"))
    units = re4_units(re4)
    failed: list[str] = []

    def one(item: tuple[str, str, list[str]]) -> list[dict]:
        unit, lang, extra = item
        stem = work / unit.replace("/", "__")
        pre, asm, obj = stem.with_suffix(".i"), stem.with_suffix(".s"), stem.with_suffix(".o")
        cpp_extra = [f for f in extra if f.startswith(("-D", "-U", "-I"))]
        cc1_extra = [f for f in extra if f not in cpp_extra]
        if "-G" in cc1_extra:                    # cc1 takes -G0 joined, as ngccc passes it
            i = cc1_extra.index("-G")
            cc1_extra[i:i + 2] = ["-G" + cc1_extra[i + 1]]
        cc1 = "cc1plus.exe" if lang == "c++" else "cc1.exe"
        err = (run([str(WIBO), str(PRODG / "CPP.exe"), *TARGET_DEFS, *RE4_CPP_FLAGS, *cpp_extra,
                    *LANG_DEFS[lang], f"src/{unit}", "-o", str(pre)], re4, env)
               or run([str(WIBO), str(PRODG / cc1), "-quiet", *RE4_CC1_FLAGS, *cc1_extra,
                       str(pre), "-o", str(asm)], re4, env)
               or run([str(AS), "-mgekko", "-o", str(obj), str(asm)], re4))
        pre.unlink(missing_ok=True)
        if err or not obj.exists():
            failed.append(f"{unit}: {err.strip().splitlines()[-1] if err.strip() else 'no object'}")
            return []
        return index_object("re4", unit, re4, obj, asm, main=f"src/{unit}")

    entries: list[dict] = []
    with ThreadPoolExecutor(jobs) as pool:
        for i, res in enumerate(pool.map(one, units), 1):
            entries += res
            if i % 50 == 0:
                print(f"  re4: {i}/{len(units)} units, {len(entries)} functions", file=sys.stderr)
    note = {"corpus": "re4", "source": RE4_URL, "revision": rev, "units": len(units),
            "failed": failed}
    (OUT / "re4.meta.json").write_text(json.dumps(note, indent=1))
    print(f"re4: {len(units) - len(failed)}/{len(units)} units, {len(entries)} functions"
          f" ({len(failed)} failed, see {rel(OUT / 're4.meta.json')})")
    return entries


def matched_lsw1() -> dict[str, set[str]]:
    """source path -> names of functions objdiff reports at 100%."""
    if not REPORT.exists():
        sys.exit(f"{rel(REPORT)} missing: run bash build.sh first")
    out: dict[str, set[str]] = defaultdict(set)
    for u in json.loads(REPORT.read_text())["units"]:
        src = (u.get("metadata") or {}).get("source_path")
        # src/dolphin is MWCC-built SDK code: a different compiler's idioms
        if not src or not src.endswith(".c") or src.startswith("src/dolphin/"):
            continue
        for f in u.get("functions") or []:
            if f.get("fuzzy_match_percent") == 100.0:
                out[src].add(f["name"])
    return out


def build_lsw1(jobs: int) -> list[dict]:
    sys.path.insert(0, str(ROOT / "tools"))
    from score_fn import compile_source  # noqa: E402
    work = OUT / "lsw1"
    work.mkdir(parents=True, exist_ok=True)
    units = sorted(matched_lsw1().items())
    failed: list[str] = []

    def one(item: tuple[str, set[str]]) -> list[dict]:
        src, names = item
        obj = work / (src.replace("/", "__")[:-2] + ".o")
        err = compile_source(ROOT / src, obj, "prodg", ["-gstabs"])
        asm = obj.with_suffix(".prodg.s")
        obj.with_suffix(".i").unlink(missing_ok=True)
        if err or not asm.exists():
            failed.append(f"{src}: {err.splitlines()[-1] if err else 'no asm'}")
            return []
        return index_object("lsw1", src, ROOT, obj, asm, names, main=src)

    entries: list[dict] = []
    with ThreadPoolExecutor(jobs) as pool:
        for res in pool.map(one, units):
            entries += res
    print(f"lsw1: {len(units) - len(failed)}/{len(units)} units, {len(entries)} functions")
    for f in failed:
        print(f"  failed {f[:200]}")
    return entries


def save(corpus: str, entries: list[dict]) -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    with gzip.open(OUT / f"{corpus}.json.gz", "wt") as f:
        json.dump(entries, f, separators=(",", ":"))


def load(corpora: list[str]) -> list[dict]:
    out = []
    for c in corpora:
        p = OUT / f"{c}.json.gz"
        if p.exists():
            with gzip.open(p, "rt") as f:
                out += json.load(f)
    if not out:
        sys.exit(f"no index under {rel(OUT)}: run python3 tools/pattern_library.py build")
    return out


# ---------------------------------------------------------------- normalising

def reg_class(r: str) -> str:
    n = int(r[1:])
    if r[0] == "f":
        return "f" if n < 14 else "fs"
    if n == 0:
        return "r0"
    if n == 1:
        return "sp"
    if n in (2, 13):
        return "sda"
    return "a" if n <= 10 else "v" if n <= 12 else "s"


_REG = re.compile(r"^[rf]\d{1,2}$")
_MEM = re.compile(r"^(.*)\((r\d+)\)$")


def operand_kind(op: str) -> str:
    op = op.strip()
    if _REG.match(op):
        return reg_class(op)
    if re.match(r"^cr\d$", op):
        return "cr"
    if re.match(r"^q?r?qr\d$|^qr\d$", op):
        return "qr"
    m = _MEM.match(op)
    if m:
        disp = m.group(1)
        k = "sym" if "@" in disp or re.search(r"[A-Za-z_]{2,}", disp.replace("0x", "")) else "d"
        return f"{k}({reg_class(m.group(2))})"
    if "@" in op:
        return "sym@" + op.rsplit("@", 1)[1]
    if re.match(r"^-?(0x[0-9a-fA-F]+|\d+)$", op):
        return "0" if int(op, 0) == 0 else "i"
    return "lbl"


def mnemonic(text: str) -> str:
    return text.split(None, 1)[0].rstrip("+-")


def shape(text: str) -> str:
    parts = text.split(None, 1)
    ops = [operand_kind(o) for o in parts[1].split(",")] if len(parts) > 1 else []
    return parts[0].rstrip("+-") + " " + ",".join(ops)


# ---------------------------------------------------------------- target side

def target_function(name: str) -> list[tuple[int, str]]:
    if not TARGET_ASM.exists():
        sys.exit(f"{rel(TARGET_ASM)} missing: run bash build.sh first")
    needle = f".fn {name},"
    for path in TARGET_ASM.rglob("*.s"):
        text = path.read_text(errors="replace")
        if needle in text:
            fn = dtk_functions(text).get(name)
            if fn:
                return fn
    sys.exit(f"no .fn {name} under {rel(TARGET_ASM)}")


# ---------------------------------------------------------------- reporting

def source_excerpt(e: dict, lo: int, hi: int, width: int = 14) -> list[str]:
    """Source lines behind instructions lo..hi of corpus entry e (main file first)."""
    picked = [e["lines"][i] for i in range(lo, hi) if e["lines"][i]]
    if not picked:
        return ["    (no line records)"]
    by_file = Counter(fi for fi, _ in picked)
    fi = by_file.most_common(1)[0][0]
    nums = [ln for f, ln in picked if f == fi]
    a, b = min(nums), max(nums)
    if b - a >= width:
        b = a + width - 1
    path = Path(e["root"]) / e["files"][fi]
    if not path.exists():
        path = Path(e["root"]) / "src" / e["files"][fi]
    if not path.exists():                            # a header named by its include form
        hits = [p for d in ("include", "src") for p in (Path(e["root"]) / d).rglob(Path(e["files"][fi]).name)]
        path = hits[0] if len(hits) == 1 else path
    out = [f"    {e['files'][fi]}:{a}-{b}"]
    try:
        src = path.read_text(errors="replace").splitlines()
    except OSError:
        return out + ["    (source not readable)"]
    for n in range(a, b + 1):
        if 0 < n <= len(src):
            mark = ">" if n in nums else " "
            out.append(f"    {mark}{n:5} {src[n - 1].rstrip()[:110]}")
    return out


def where(e: dict) -> str:
    first = next((x for x in e["lines"] if x), None)
    loc = f"{e['files'][first[0]]}:{first[1]}" if first else e["unit"]
    return f"{e['name']}  [{e['corpus']} {loc}, {len(e['insns'])} insns]"


# ---------------------------------------------------------------- commands

def cmd_build(args) -> int:
    for tool in (WIBO, DTK, AS, PRODG / "cc1plus.exe"):
        if not tool.exists():
            sys.exit(f"{rel(tool)} missing: run bash tools/bootstrap.sh")
    corpora = ["re4", "lsw1"] if args.corpus == "all" else [args.corpus]
    for c in corpora:
        save(c, build_re4(args.jobs) if c == "re4" else build_lsw1(args.jobs))
    return 0


def query_entries(args) -> list[dict]:
    corpora = ["re4", "lsw1"] if args.corpus == "all" else [args.corpus]
    entries = load(corpora)
    skip = getattr(args, "name", None)
    return [e for e in entries if not (e["corpus"] == "lsw1" and e["name"] == skip)]


def cmd_similar(args) -> int:
    target = target_function(args.name)
    q_shape = [shape(t) for _, t in target]
    q_mn = [mnemonic(t) for _, t in target]
    qgrams = Counter(zip(q_mn, q_mn[1:], q_mn[2:]))
    entries = query_entries(args)
    n = len(q_mn)
    cands = []
    for e in entries:
        m = len(e["insns"])
        if not (n * 0.5 <= m <= n * 2 + 4):
            continue
        mn = [mnemonic(t) for t in e["insns"]]
        g = Counter(zip(mn, mn[1:], mn[2:]))
        inter = sum((qgrams & g).values())
        union = sum((qgrams | g).values()) or 1
        cands.append((inter / union, e))
    cands.sort(key=lambda x: -x[0])
    scored = []
    for _, e in cands[:max(60, args.top * 6)]:
        sm = difflib.SequenceMatcher(None, q_shape, [shape(t) for t in e["insns"]], autojunk=False)
        scored.append((sm.ratio(), e))
    scored.sort(key=lambda x: -x[0])
    print(f"{args.name}: {n} instructions; closest {args.corpus} functions by instruction shape")
    for ratio, e in scored[:args.top]:
        print(f"\n{ratio * 100:5.1f}%  {where(e)}")
        if args.source:
            print("\n".join(source_excerpt(e, 0, len(e["insns"]), args.source)))
    return 0


def parse_seq(seq: str) -> list[str]:
    return [s.strip() for s in seq.split(";") if s.strip()]


def cmd_window(args) -> int:
    if args.seq:
        q_text = parse_seq(args.seq)
        exact_shape = any(" " in s for s in q_text)
    else:
        if not args.name:
            sys.exit("give a function name with --at, or --seq")
        target = target_function(args.name)
        start = 0
        if args.at:
            addr = int(args.at, 16)
            idx = [i for i, (a, _) in enumerate(target) if a == addr]
            if not idx:
                sys.exit(f"{args.at} is not an instruction of {args.name}")
            start = max(0, idx[0] - args.before)
        q_text = [t for _, t in target[start:start + args.len]]
        exact_shape = True
    q_mn = [mnemonic(t) for t in q_text]
    q_sh = [shape(t) for t in q_text] if exact_shape else None
    k = min(3, len(q_mn))
    entries = query_entries(args)
    grams: dict[tuple, list[tuple[int, int]]] = defaultdict(list)
    q_keys = {tuple(q_mn[i:i + k]) for i in range(len(q_mn) - k + 1)}
    for ei, e in enumerate(entries):
        mn = [mnemonic(t) for t in e["insns"]]
        e["_mn"] = mn
        for i in range(len(mn) - k + 1):
            key = tuple(mn[i:i + k])
            if key in q_keys:
                grams[key].append((ei, i))
    best: dict[int, tuple] = {}
    for qi in range(len(q_mn) - k + 1):
        for ei, pos in grams.get(tuple(q_mn[qi:qi + k]), ()):
            mn = entries[ei]["_mn"]
            a, b = qi, pos
            while a > 0 and b > 0 and q_mn[a - 1] == mn[b - 1]:
                a, b = a - 1, b - 1
            length = 0
            while a + length < len(q_mn) and b + length < len(mn) and q_mn[a + length] == mn[b + length]:
                length += 1
            sh = 0
            if q_sh:
                ins = entries[ei]["insns"]
                sh = sum(q_sh[a + j] == shape(ins[b + j]) for j in range(length))
            key = (length, sh)
            if ei not in best or key > best[ei][0]:
                best[ei] = (key, a, b)
    ranked = sorted(best.items(), key=lambda kv: (-kv[1][0][0], -kv[1][0][1], len(entries[kv[0]]["insns"])))
    print(f"query: {len(q_mn)} instructions" + (f" from {args.name}" if args.name and not args.seq else ""))
    for t in q_text:
        print(f"    {t}")
    shown = 0
    for ei, ((length, sh), a, b) in ranked:
        if length < args.min:
            break
        e = entries[ei]
        print(f"\n{length}/{len(q_mn)} in a row" + (f", {sh} same operand shape" if q_sh else "")
              + f" (query {a}..{a + length - 1})  {where(e)}")
        for j in range(length):
            print(f"    {e['insns'][b + j]}")
        print("\n".join(source_excerpt(e, b, b + length)))
        shown += 1
        if shown >= args.top:
            break
    if not shown:
        print(f"\nno corpus run of {args.min}+ matching mnemonics")
    return 0


def cmd_show(args) -> int:
    corpus = args.id.split(":", 1)[0]
    for e in load([corpus]):
        if e["id"] == args.id or (e["name"] == args.id.split(":")[-1] and args.id.count(":") < 2):
            print(where(e))
            for t in e["insns"]:
                print(f"    {t}")
            print("\n".join(source_excerpt(e, 0, len(e["insns"]), 200)))
            return 0
    sys.exit(f"{args.id} not in the {corpus} index")


def rel(p: Path) -> str:
    try:
        return str(p.relative_to(ROOT))
    except ValueError:
        return str(p)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    b = sub.add_parser("build", help="compile and index the corpora")
    b.add_argument("--corpus", choices=["re4", "lsw1", "all"], default="all")
    b.add_argument("-j", "--jobs", type=int, default=os.cpu_count() or 4)
    s = sub.add_parser("similar", help="whole functions with the closest instruction shape")
    s.add_argument("name")
    s.add_argument("--top", type=int, default=8)
    s.add_argument("--source", type=int, nargs="?", const=14, default=0,
                   help="print up to N source lines of each hit (default 14)")
    w = sub.add_parser("window", help="corpus code containing an instruction run")
    w.add_argument("name", nargs="?")
    w.add_argument("--at", help="instruction address in the target (e.g. the first mismatch)")
    w.add_argument("--before", type=int, default=2, help="context instructions before --at")
    w.add_argument("--len", type=int, default=10, help="query length in instructions")
    w.add_argument("--seq", help='mnemonics separated by ";" (add operands for shape matching)')
    w.add_argument("--min", type=int, default=4, help="shortest run to report")
    w.add_argument("--top", type=int, default=6)
    for p in (s, w):
        p.add_argument("--corpus", choices=["re4", "lsw1", "all"], default="all")
    sh = sub.add_parser("show", help="print one corpus function with its source")
    sh.add_argument("id", help="corpus:unit:name, or corpus:name")
    args = ap.parse_args(argv)
    return {"build": cmd_build, "similar": cmd_similar, "window": cmd_window, "show": cmd_show}[args.cmd](args)


if __name__ == "__main__":
    sys.exit(main())
