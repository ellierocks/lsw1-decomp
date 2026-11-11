"""Keep a task pack's asm.s in step with the current build asm.

`ls1_task_pack.py` writes asm.s once and never overwrites a pack, so after `bash build.sh`
changes relocations or symbol names an old pack drafts and scores against stale assembly.
`ensure_asm` creates the pack when it is missing and otherwise rewrites asm.s whenever it
differs from `ls1_task_pack.extract_asm` on the current build, dropping files derived purely
from asm.s (DERIVED). Hand-written files (candidate.c, status.md, notes) are never touched;
drafts (draft_m2c.c, sweep_draft.c) are left for the next draft or sweep to rewrite.

asm.stamp records the build asm file asm.s came from and the stats of that file, of
build/GL5E4F/config.json and of asm.s itself. While all of them still hold, the check is a
few stat calls; otherwise (and for packs without a stamp) asm.s is compared with a fresh
extraction once. Without build asm, asm.s is kept as it is.
"""
from __future__ import annotations

import json
import os
import subprocess
import sys
import threading
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
STAMP = "asm.stamp"
DERIVED = ("asm_m2c.s",)  # rewritten from asm.s on every draft; removed so none outlives a refresh

_SYMBOLS: dict[tuple, dict[str, tuple[int, int]]] = {}
_SYMBOLS_LOCK = threading.Lock()


def _task_pack():
    """ls1_task_pack, imported on first use (it loads the donor tables)."""
    try:
        from . import ls1_task_pack
    except ImportError:
        here = str(Path(__file__).resolve().parent)
        if here not in sys.path:
            sys.path.insert(0, here)
        import ls1_task_pack
    return ls1_task_pack


def _stat(path: Path) -> list[int] | None:
    try:
        st = path.stat()
    except OSError:
        return None
    return [st.st_mtime_ns, st.st_size]


def _config(tp) -> Path:
    return tp.ASM_FILE.parent.parent / "config.json"


def stamp_current(task: Path) -> bool:
    """True while asm.stamp still describes asm.s, its build asm file and the build config."""
    try:
        stamp = json.loads((task / STAMP).read_text())
        source = Path(stamp["source"])
    except (OSError, ValueError, KeyError, TypeError):
        return False
    return (stamp.get("asm") == _stat(task / "asm.s") and _stat(source) is not None
            and stamp.get("source_stat") == _stat(source)
            and stamp.get("config") == _stat(_config(_task_pack())))


def _write(path: Path, text: str) -> None:
    tmp = path.with_name(f".{path.name}.{os.getpid()}.{threading.get_ident()}")
    tmp.write_text(text)
    tmp.replace(path)


def _symbol(name: str):
    """(address, size) of NAME in symbols.txt, or None."""
    tp = _task_pack()
    key = (tp.SYMTAB, *(_stat(tp.SYMTAB) or ()))
    with _SYMBOLS_LOCK:
        if key not in _SYMBOLS:
            table: dict[str, tuple[int, int]] = {}
            if tp.SYMTAB.is_file():
                for addr, sym, size, _ in tp.load_symtab():
                    table.setdefault(sym, (addr, size))
            _SYMBOLS.clear()
            _SYMBOLS[key] = table
        return _SYMBOLS[key].get(name)


def refresh(name: str, task: Path) -> bool:
    """Bring TASK/asm.s in line with the current build asm; True when it was rewritten.

    Keeps asm.s (and writes no stamp) when NAME is not in symbols.txt or the build asm
    has no body for it, e.g. before the first `bash build.sh`."""
    tp = _task_pack()
    sym = _symbol(name)
    if sym is None:
        return False
    text, source = tp.locate_asm(sym[0], sym[1], name)
    if text is None:
        return False
    asm = task / "asm.s"
    changed = asm.read_text() != text
    if changed:
        _write(asm, text)
        for derived in DERIVED:
            (task / derived).unlink(missing_ok=True)
        shown = lambda p: p.relative_to(ROOT) if p.is_relative_to(ROOT) else p  # noqa: E731
        print(f"refreshed {shown(asm)} from {shown(source)}", file=sys.stderr)
    _write(task / STAMP, json.dumps({"source": str(source), "source_stat": _stat(source),
                                     "config": _stat(_config(tp)), "asm": _stat(asm)}) + "\n")
    return changed


def create(name: str, out_root: Path) -> None:
    """Write OUT_ROOT/NAME with ls1_task_pack.py (quietly; a failure leaves no asm.s)."""
    subprocess.run([sys.executable, str(ROOT / "tools" / "ls1_task_pack.py"), name,
                    "--out-dir", str(out_root)], cwd=ROOT, capture_output=True)


def ensure_asm(name: str, out_root: Path, make: bool = True) -> Path:
    """OUT_ROOT/NAME/asm.s, created (MAKE) when missing and refreshed when stale."""
    task = Path(out_root) / name
    asm = task / "asm.s"
    if not asm.is_file():
        if not make:
            return asm
        create(name, Path(out_root))
        if not asm.is_file():
            return asm
    if not stamp_current(task):
        refresh(name, task)
    return asm
