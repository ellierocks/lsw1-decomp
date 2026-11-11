#!/usr/bin/env python3
"""Read the running Dolphin's GL5E4F RAM and build Gecko call tracers.

Agents use this to turn static guesses into runtime evidence without a
debugger. Everything here reads Dolphin except the drivers (`warp`, `debug-menu`),
which with --write make one data store that the game itself also makes or
reads, and the desktop commands (`launch`, `close`, `screenshot`, `hotkey`),
which go through Hyprland and Dolphin's own hotkeys. Code reaches the game only
through a Gecko code that the user pastes and enables. See
docs/dolphin_runtime.md for the workflow and the rules.

Dolphin keeps emulated RAM in an unlinked shared-memory file
(`/dev/shm/dolphin-emu.<pid>`) mapped into its own address space. MEM1 is the
mapping that is at least 24 MiB and starts with the game ID, read through
`/proc/<pid>/mem`. Linux only; needs ptrace access (yama ptrace_scope 0, or
run as the same user with scope 1 and CAP_SYS_PTRACE).

Commands:
    status                     is Dolphin running GL5E4F, and is it paused?
    launch [--state FILE]      start a graphical Dolphin on the desktop
    close                      close a Dolphin that `launch` started
    screenshot [--out PNG]     capture Dolphin's game window (grim -T)
    hotkey [ID]                press a Dolphin hotkey (once per session)
    read ADDR|SYM [--type]     typed live read (see ramdump.py for more types)
    wait ADDR|SYM --equals V   block until a value is reached (state gates)
    watch ADDR|SYM [...]       print values as they change
    snapshot LABEL             save MEM1 to dumps/LABEL.raw + manifest entry
    levels                     list level indices and names (retail DOL)
    level                      current and pending level of the live game
    warp LEVEL --write         request a level change
    debug-menu --write         unhide "LEGO Options" in the pause menu
    trace-code FUNC[...]       emit a Gecko C2 logger for function entries
    trace-tail                 read the logger ring, summarise callers/values
"""
import argparse
import bisect
import collections
import json
import os
import re
import shlex
import signal
import shutil
import tempfile
import struct
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
REPORT = ROOT / "build/GL5E4F/report.json"
SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
DUMPS_DIR = ROOT / "dumps"
GAME_ID = b"GL5E4F"

MEM1_BASE = 0x80000000
MEM1_SIZE = 0x1800000
SDA_BASE = 0x8040FA40   # r13
FRAME_COUNTER = 0x8040AA14  # retraceCount

LEVEL_TABLE = 0x801D68E0      # lbl_801D68E0, 0xA8-byte entries
LEVEL_ENTRY = 0xA8             # +8 path, +0xC name
LEVEL_COUNT = 127             # 124 titles, 125 credits, 126 status
CUR_LEVEL = 0x804093AC        # lbl_804093AC, current level index
NEXT_LEVEL = 0x804093B0       # lbl_804093B0, pending level index or -1

DEBUG_MENU = 0x804090F0       # gLegoOptionsMenuEnabled; read twice, never written

DEFAULT_RING = 0x80D00000
RING_BYTES = 0x10000          # entry area; header is 16 bytes before it
ENTRY = 16                    # [lr|tag, frame, cap0, cap1]
RING_MAGIC = 0x54524345       # 'TRCE', kept alive by a 04 line

SCALARS = {
    "u8": (">B", 1), "s8": (">b", 1), "u16": (">H", 2), "s16": (">h", 2),
    "u32": (">I", 4), "s32": (">i", 4), "f32": (">f", 4), "ptr": (">I", 4),
}
STR_MAX = 256                 # read --type str: bytes scanned for the NUL


CONFIG = Path(os.environ.get("XDG_CONFIG_HOME", Path.home() / ".config")) \
    / "lsw1-decomp/dolphin.json"
LAUNCHED = Path(os.environ.get("XDG_CACHE_HOME", Path.home() / ".cache")) \
    / "lsw1-decomp/dolphin-launched.json"
SHOTS_DIR = Path(tempfile.gettempdir()) / "lsw1-dolphin-shots"
LAUNCH_OVERRIDES = (
    # Dolphin registers portal global shortcuts (screenshot, hotkey) only
    # when hotkeys don't need window focus.
    "Dolphin.General.HotkeysRequireFocus=False",
    # SIGTERM closes the main window; without this it asks to confirm, and
    # Dolphin's handler is one-shot, so `close` could not finish.
    "Dolphin.Interface.ConfirmStop=False",
)


class DolphinError(RuntimeError):
    pass


# --------------------------------------------------------------------------- #
# Live process access
# --------------------------------------------------------------------------- #
def parse_maps(text):
    """Yield (start, end, path) for Dolphin shared-memory mappings."""
    for line in text.splitlines():
        parts = line.split(None, 5)
        if len(parts) < 6 or "dolphin-emu." not in parts[5]:
            continue
        lo, hi = (int(x, 16) for x in parts[0].split("-"))
        yield lo, hi, parts[5].strip()


def candidate_pids():
    env = os.environ.get("DOLPHIN_PID")
    if env:
        return [int(env)]
    pids = []
    for d in Path("/proc").iterdir():
        if not d.name.isdigit():
            continue
        try:
            comm = (d / "comm").read_text().strip()
        except OSError:
            continue
        if comm.startswith("dolphin") and "memory" not in comm:
            pids.append(int(d.name))
    return sorted(pids)


class LiveMem:
    """MEM1 of a running Dolphin, addressed by GameCube address."""

    def __init__(self, pid, host_base, game_id):
        self.pid = pid
        self.host_base = host_base
        self.game_id = game_id
        self._fh = open(f"/proc/{pid}/mem", "rb", buffering=0)

    @classmethod
    def attach(cls, pid=None, game_id=GAME_ID):
        pids = [pid] if pid else candidate_pids()
        if not pids:
            raise DolphinError("no Dolphin process found (set DOLPHIN_PID "
                               "or pass --pid)")
        seen = []
        for p in pids:
            try:
                maps = Path(f"/proc/{p}/maps").read_text()
                fh = open(f"/proc/{p}/mem", "rb", buffering=0)
            except OSError as e:
                seen.append(f"pid {p}: {e.strerror}")
                continue
            heads = []
            with fh:
                for lo, hi, _path in parse_maps(maps):
                    if hi - lo < MEM1_SIZE:
                        continue
                    try:
                        fh.seek(lo)
                        head = fh.read(6)
                    except OSError:
                        continue
                    if head == game_id:
                        return cls(p, lo, head)
                    heads.append(head)
            seen.append(f"pid {p}: RAM holds {heads[0]!r}" if heads
                        else f"pid {p}: no game booted")
        raise DolphinError("GL5E4F not running in Dolphin (" +
                           "; ".join(seen) + ")")

    def close(self):
        self._fh.close()

    def __del__(self):
        fh = getattr(self, "_fh", None)
        if fh is not None:
            fh.close()

    def read(self, addr, n):
        if addr >= 0xC0000000:
            addr = (addr & 0x01FFFFFF) | MEM1_BASE
        off = addr - MEM1_BASE
        if not (0 <= off and off + n <= MEM1_SIZE):
            raise DolphinError(f"0x{addr:08X}+{n} is outside MEM1")
        self._fh.seek(self.host_base + off)
        return self._fh.read(n)

    def scalar(self, addr, kind):
        fmt, width = SCALARS[kind]
        return struct.unpack(fmt, self.read(addr, width))[0]

    def write_u32(self, addr, value):
        """The only write path; callers gate it behind --write."""
        if not (MEM1_BASE <= addr and addr + 4 <= MEM1_BASE + MEM1_SIZE):
            raise DolphinError(f"0x{addr:08X} is outside MEM1")
        with open(f"/proc/{self.pid}/mem", "r+b", buffering=0) as fh:
            fh.seek(self.host_base + addr - MEM1_BASE)
            fh.write(struct.pack(">I", value & 0xFFFFFFFF))

    def read_all(self):
        out = bytearray()
        for off in range(0, MEM1_SIZE, 0x100000):
            out += self.read(MEM1_BASE + off, 0x100000)
        return bytes(out)


def attach_or_exit(args):
    try:
        return LiveMem.attach(getattr(args, "pid", None))
    except DolphinError as e:
        sys.exit(f"error: {e}\nAsk the user to boot GL5E4F in Dolphin "
                 f"(docs/dolphin_runtime.md#asking-the-user).")


# --------------------------------------------------------------------------- #
# Desktop: launching, closing and screenshots (Hyprland)
# --------------------------------------------------------------------------- #
def machine_config():
    """Per-machine settings, shared by all worktrees; never in git."""
    cfg = json.loads(CONFIG.read_text()) if CONFIG.is_file() else {}
    cfg.setdefault("launcher", os.environ.get("DOLPHIN_LAUNCHER")
                   or shutil.which("dolphin-emu"))
    cfg.setdefault("iso", os.environ.get("DOLPHIN_ISO"))
    return cfg


def hypr_env():
    """The desktop's environment; agent shells don't inherit it."""
    env = dict(os.environ)
    if "HYPRLAND_INSTANCE_SIGNATURE" not in env:
        run = Path(env.get("XDG_RUNTIME_DIR", f"/run/user/{os.getuid()}")) / "hypr"
        sigs = sorted(run.iterdir(), key=lambda d: d.stat().st_mtime) \
            if run.is_dir() else []
        if not sigs:
            raise DolphinError("Hyprland is not running (no $XDG_RUNTIME_DIR/hypr)")
        env["HYPRLAND_INSTANCE_SIGNATURE"] = sigs[-1].name
    return env


def hyprctl(*args, json_out=False):
    env = hypr_env()
    cmd = ["hyprctl"] + (["-j"] if json_out else []) + list(args)
    try:
        out = subprocess.run(cmd, env=env, capture_output=True, text=True,
                             check=True, timeout=10).stdout
    except (OSError, subprocess.SubprocessError) as e:
        raise DolphinError(f"hyprctl {' '.join(args)} failed: {e}")
    if args[0] == "dispatch" and out.strip() != "ok":
        raise DolphinError(f"hyprctl {' '.join(args)}: {out.strip()}")
    return json.loads(out) if json_out else out


def lua_str(text):
    """A Lua string literal (JSON escapes are valid Lua for this use)."""
    return json.dumps(text, ensure_ascii=True)


def dispatch(lua):
    """Hyprland 0.57+ takes dispatchers as Lua, e.g. hl.dsp.exec_cmd("...")."""
    return hyprctl("dispatch", lua)


def user_dir(pid):
    argv = Path(f"/proc/{pid}/cmdline").read_bytes().split(b"\0")
    return user_dir_from_argv([a.decode(errors="replace") for a in argv])


def user_dir_from_argv(argv):
    for i, a in enumerate(argv):
        if a in ("-u", "--user") and i + 1 < len(argv):
            return Path(argv[i + 1])
        if a.startswith("--user="):
            return Path(a.partition("=")[2])
    return Path.home() / ".local/share/dolphin-emu"


def cmd_launch(args, syms):
    try:
        mem = LiveMem.attach(args.pid)
        print(f"already running: pid {mem.pid}")
        return 0
    except DolphinError:
        pass
    cfg = machine_config()
    if not cfg["launcher"] or not cfg["iso"]:
        raise DolphinError(f"set launcher and iso in {CONFIG} (or "
                           f"DOLPHIN_LAUNCHER / DOLPHIN_ISO)")
    cmd = [cfg["launcher"]]
    for override in LAUNCH_OVERRIDES:      # this run only; ini is untouched
        cmd += ["-C", override]
    cmd += ["-e", cfg["iso"]]
    if args.state:
        cmd += ["-s", str(Path(args.state).expanduser().resolve())]
    # Hyprland starts it, so it belongs to the desktop, not this shell.
    dispatch(f"hl.dsp.exec_cmd({lua_str(shlex.join(cmd))})")
    print(f"launching: {shlex.join(cmd)}")
    deadline = time.time() + args.timeout
    while time.time() < deadline:
        try:
            mem = LiveMem.attach()
        except DolphinError:
            time.sleep(0.5)
            continue
        if frame_rate(mem, 0.5):
            LAUNCHED.parent.mkdir(parents=True, exist_ok=True)
            LAUNCHED.write_text(json.dumps({"pid": mem.pid,
                                            "started": time.time()}))
            print(f"running: pid {mem.pid}, retraceCount "
                  f"{mem.scalar(FRAME_COUNTER, 'u32')}")
            return 0
        time.sleep(0.5)
    print(f"timeout after {args.timeout:g} s waiting for GL5E4F to run")
    return 2


def cmd_close(args, syms):
    if not LAUNCHED.is_file():
        raise DolphinError("no agent-launched Dolphin recorded; the user's own "
                           "Dolphin is theirs to close")
    pid = json.loads(LAUNCHED.read_text())["pid"]
    if not Path(f"/proc/{pid}").exists():
        LAUNCHED.unlink()
        print(f"pid {pid} already gone")
        return 0
    # SIGTERM closes the main window; Dolphin stops emulation and exits.
    os.kill(pid, signal.SIGKILL if args.force else signal.SIGTERM)
    for _ in range(100):
        if not Path(f"/proc/{pid}").exists():
            break
        time.sleep(0.1)
    else:
        print(f"pid {pid} still running: Dolphin may be asking to confirm "
              f"the stop. Ask the user, or use --force (SIGKILL; the "
              f"handler is one-shot, so a second SIGTERM also kills)")
        return 2
    LAUNCHED.unlink()
    print(f"closed pid {pid}")
    return 0


def dolphin_shortcuts():
    """Dolphin's portal global shortcuts, as {short id: full name}."""
    found = {}
    for line in hyprctl("globalshortcuts").splitlines():
        name = line.split(" -> ")[0].strip()
        app, _, sid = name.partition(":")
        if app.startswith("dolphin") and sid:
            found[sid] = name
    return found


def press_hotkey(sid):
    """Press one of Dolphin's hotkeys through the desktop, without focus."""
    shortcuts = dolphin_shortcuts()
    if not shortcuts:
        raise DolphinError(
            "Dolphin registered no global shortcuts: untick Config -> "
            "Interface -> Hotkeys Require Window Focus (Wayland build only)")
    if sid not in shortcuts:
        near = ", ".join(k for k in shortcuts if sid.split("-")[0] in k)
        raise DolphinError(f"no Dolphin hotkey {sid!r}"
                           + (f"; similar: {near}" if near else ""))
    dispatch(f"hl.dsp.global({lua_str(shortcuts[sid])})")
    return shortcuts[sid]


def cmd_hotkey(args, syms):
    if not args.id:
        for sid, name in dolphin_shortcuts().items():
            print(sid)
        return 0
    print(f"pressed {press_hotkey(args.id)}")
    return 0


def render_window(clients, pid, game_id):
    """The Hyprland client showing the game: Dolphin's window titled with
    the game ID, or its only window."""
    mine = [c for c in clients if c.get("pid") == pid]
    titled = [c for c in mine if f"({game_id})" in c.get("title", "")]
    if len(titled) == 1:
        return titled[0]
    if len(mine) == 1:
        return mine[0]
    raise DolphinError(f"can't tell which of Dolphin's {len(mine)} windows "
                       f"shows the game; enable Config -> Interface -> Show "
                       f"Active Title in Window Title")


def cmd_screenshot(args, syms):
    """Capture Dolphin's render window with `grim -T` (repeatable, works on
    hidden workspaces). Dolphin's own screenshot hotkey works only once per
    session through hyprctl; see docs/dolphin_runtime.md."""
    mem = attach_or_exit(args)
    game = mem.game_id.decode()
    win = render_window(hyprctl("clients", json_out=True), mem.pid, game)
    frame = mem.scalar(FRAME_COUNTER, "u32")
    out = Path(args.out) if args.out else \
        SHOTS_DIR / f"{game}-{time.strftime('%Y%m%d-%H%M%S')}-f{frame}.png"
    out.parent.mkdir(parents=True, exist_ok=True)
    env = hypr_env()
    if "WAYLAND_DISPLAY" not in env:
        inst = [i for i in hyprctl("instances", json_out=True)
                if i["instance"] == env["HYPRLAND_INSTANCE_SIGNATURE"]]
        env["WAYLAND_DISPLAY"] = inst[0]["wl_socket"] if inst else "wayland-1"
    try:
        subprocess.run(["grim", "-T", str(win["stableId"]), str(out)],
                       env=env, check=True, capture_output=True, text=True,
                       timeout=15)
    except subprocess.CalledProcessError as e:
        raise DolphinError(f"grim failed: {e.stderr.strip()}")
    except (OSError, subprocess.SubprocessError) as e:
        raise DolphinError(f"grim failed: {e}")
    print(out)
    return 0


# --------------------------------------------------------------------------- #
# Symbols
# --------------------------------------------------------------------------- #
SYM_LINE = re.compile(r"^([A-Za-z_$][\w$]*)\s*=\s*\.(\w+):0x([0-9A-Fa-f]+);"
                      r"(?:.*?size:0x([0-9A-Fa-f]+))?")


class Symbols:
    def __init__(self, path=SYMBOLS):
        self.by_name, rows = {}, []
        if path.is_file():
            for line in path.read_text(errors="replace").splitlines():
                m = SYM_LINE.match(line.strip())
                if m:
                    addr, size = int(m[3], 16), int(m[4] or "0", 16)
                    self.by_name[m[1]] = (addr, m[2], size)
                    rows.append((addr, size, m[1], m[2]))
        rows.sort()
        self._rows, self._addrs = rows, [r[0] for r in rows]

    def addr(self, text):
        try:
            return int(text, 0)
        except ValueError:
            pass
        if text not in self.by_name:
            raise DolphinError(f"unknown symbol or address '{text}'")
        return self.by_name[text][0]

    def name(self, addr):
        i = bisect.bisect_right(self._addrs, addr) - 1
        if i >= 0:
            a, size, name, _sec = self._rows[i]
            if addr < a + max(size, 4):
                return name if addr == a else f"{name}+0x{addr - a:X}"
        return f"0x{addr:08X}"


# --------------------------------------------------------------------------- #
# Status / read / wait / watch / snapshot
# --------------------------------------------------------------------------- #
ADDR_STEP = re.compile(r"(->|[+-])\s*(0[xX][0-9a-fA-F]+|\d+)")


def resolve_addr(text, syms, mem=None):
    """Address of `text`: an address or symbol, then any +OFF/-OFF and ->OFF
    steps. `->OFF` loads the 32-bit pointer at the address so far and adds OFF,
    so `gPlayer1Pointer->0x600` is field 0x600 of the player-1 object. Null or
    out-of-MEM1 pointers raise instead of reading garbage."""
    text = text.replace(" ", "")
    m = re.match(r"[^-+]+", text)
    if not m:
        raise DolphinError(f"bad address expression '{text}'")
    addr, rest = syms.addr(m.group(0)), text[m.end():]
    while rest:
        step = ADDR_STEP.match(rest)
        if not step:
            raise DolphinError(f"bad address expression '{text}' at '{rest}'")
        op, off = step.group(1), int(step.group(2), 0)
        if op == "->":
            if mem is None:
                raise DolphinError("'->' needs a running Dolphin")
            ptr = mem.scalar(addr, "u32")
            if not MEM1_BASE <= ptr < MEM1_BASE + MEM1_SIZE:
                raise DolphinError(f"{syms.name(addr)} holds 0x{ptr:08X}, "
                                   f"not a MEM1 pointer, in '{text}'")
            addr = ptr + off
        else:
            addr += off if op == "+" else -off
        rest = rest[step.end():]
    return addr


def frame_rate(mem, secs=0.25):
    a = mem.scalar(FRAME_COUNTER, "u32")
    time.sleep(secs)
    return (mem.scalar(FRAME_COUNTER, "u32") - a) & 0xFFFFFFFF


def cmd_status(args, syms):
    try:
        mem = LiveMem.attach(args.pid)
    except DolphinError as e:
        print(f"dolphin: not available -- {e}")
        return 1
    frames = frame_rate(mem)
    state = "running" if frames else "paused (or not yet retracing)"
    print(f"dolphin: pid {mem.pid}, game {mem.game_id.decode()}, {state}")
    print(f"retraceCount = {mem.scalar(FRAME_COUNTER, 'u32')} "
          f"(+{frames} in 0.25 s)")
    ring = ring_state(mem, args.ring)
    if ring["magic"]:
        print(f"trace ring at 0x{args.ring:08X}: active "
              f"(offset 0x{ring['offset']:X}) -- remind the user to disable "
              f"the logger code when done")
    return 0


def fmt(kind, v):
    if kind == "f32":
        return f"{v:.6g}"
    if kind in ("u32", "ptr", "u16", "u8"):
        return f"0x{v:0{SCALARS[kind][1] * 2}X}"
    return str(v)


def c_string(raw):
    """Decode a NUL-terminated byte string; the bool is False when it was cut off."""
    end = raw.find(b"\0")
    text = raw if end < 0 else raw[:end]
    return text.decode("latin-1"), end >= 0


def cmd_read(args, syms):
    mem = attach_or_exit(args)
    addr = resolve_addr(args.target, syms, mem)
    if args.type == "str":
        for i in range(args.count):
            text, ended = c_string(mem.read(addr, STR_MAX))
            more = "" if ended else " ..."
            print(f"0x{addr:08X} {syms.name(addr):<32} {text!r}{more}")
            addr += len(text.encode("latin-1")) + 1
        return
    width = SCALARS[args.type][1]
    for i in range(args.count):
        a = addr + i * width
        v = mem.scalar(a, args.type)
        note = ""
        if args.type == "ptr" and MEM1_BASE <= v < MEM1_BASE + MEM1_SIZE:
            note = f"  -> {syms.name(v)}"
        print(f"0x{a:08X} {syms.name(a):<32} {fmt(args.type, v)}{note}")


def parse_value(text, kind):
    return float(text) if kind == "f32" else int(text, 0)


def cmd_wait(args, syms):
    mem = attach_or_exit(args)
    addr = resolve_addr(args.target, syms, mem)
    start = mem.scalar(addr, args.type)
    want = None if args.equals is None else parse_value(args.equals, args.type)
    deadline = time.time() + args.timeout
    while time.time() < deadline:
        v = mem.scalar(addr, args.type)
        if (want is not None and v == want) or (want is None and v != start):
            print(f"reached: {syms.name(addr)} = {fmt(args.type, v)}")
            return 0
        time.sleep(args.interval)
    print(f"timeout after {args.timeout:g} s: {syms.name(addr)} = "
          f"{fmt(args.type, mem.scalar(addr, args.type))}")
    return 2


def cmd_watch(args, syms):
    mem = attach_or_exit(args)
    targets = [(resolve_addr(t, syms, mem), t) for t in args.targets]
    last = {}
    end = time.time() + args.seconds
    while time.time() < end:
        frame = mem.scalar(FRAME_COUNTER, "u32")
        for addr, label in targets:
            v = mem.scalar(addr, args.type)
            if last.get(addr) != v:
                print(f"frame {frame:>8}  {label:<24} {fmt(args.type, v)}")
                last[addr] = v
        time.sleep(args.interval)


def cmd_snapshot(args, syms):
    if not re.fullmatch(r"[\w.-]+", args.label):
        sys.exit("error: label must be letters, digits, '.', '_' or '-'")
    mem = attach_or_exit(args)
    DUMPS_DIR.mkdir(exist_ok=True)
    path = DUMPS_DIR / f"{args.label}.raw"
    if path.exists() and not args.force:
        sys.exit(f"error: {path} exists (pass --force to replace it)")
    data = mem.read_all()
    path.write_bytes(data)
    manifest = DUMPS_DIR / "manifest.txt"
    lines = manifest.read_text().splitlines() if manifest.exists() else []
    lines = [l for l in lines if l.partition("=")[0].strip() != args.label]
    note = f"  # {args.note}" if args.note else ""
    lines.append(f"{args.label} = {path.name}{note}")
    manifest.write_text("\n".join(lines) + "\n")
    frame = struct.unpack(">I", data[FRAME_COUNTER - MEM1_BASE:][:4])[0]
    print(f"saved {path} (frame {frame}); use `ramdump.py ... {args.label}`")


# --------------------------------------------------------------------------- #
# Minimal PowerPC encoder (enough for the logger hook)
# --------------------------------------------------------------------------- #
def _d(op, rt, ra, imm):
    return (op << 26) | (rt << 21) | (ra << 16) | (imm & 0xFFFF)


def lis(rt, imm):        return _d(15, rt, 0, imm)
def addi(rt, ra, imm):   return _d(14, rt, ra, imm)
def ori(ra, rs, imm):    return _d(24, rs, ra, imm)
def lwz(rt, d, ra):      return _d(32, rt, ra, d)
def stw(rs, d, ra):      return _d(36, rs, ra, d)
def add(rt, ra, rb):     return (31 << 26) | (rt << 21) | (ra << 16) | (rb << 11) | (266 << 1)
def mflr(rt):            return (31 << 26) | (rt << 21) | (0x100 << 11) | (339 << 1)


def rlwinm(ra, rs, sh, mb, me):
    return (21 << 26) | (rs << 21) | (ra << 16) | (sh << 11) | (mb << 6) | (me << 1)


SCRATCH = {0, 11, 12}
# D-form opcodes: (reads rD/rS?, reads rA as a base register?)
_DFORM = {
    14: (False, True), 15: (False, True),           # addi, addis
    24: (True, False), 25: (True, False), 26: (True, False), 27: (True, False),
    28: (True, False), 29: (True, False),           # ori/oris/xori/xoris/andi./andis.
    32: (False, True), 33: (False, True), 34: (False, True), 35: (False, True),
    40: (False, True), 41: (False, True), 42: (False, True), 43: (False, True),
    48: (False, True), 49: (False, True), 50: (False, True), 51: (False, True),
    36: (True, True), 37: (True, True), 38: (True, True), 39: (True, True),
    44: (True, True), 45: (True, True), 52: (True, True), 53: (True, True),
    54: (True, True), 55: (True, True), 46: (True, True), 47: (True, True),
    10: (False, True), 11: (False, True), 7: (False, True), 8: (False, True),
    12: (False, True), 13: (False, True),
}


def check_relocatable(word):
    """Return why `word` can't run inside a C2 hook, or None if it can.

    The hook body clobbers r0, r11 and r12 before running the displaced
    instruction, so that instruction must not read them, and it must not be
    PC-relative.
    """
    op = word >> 26
    rd, ra, rb = (word >> 21) & 31, (word >> 16) & 31, (word >> 11) & 31
    if op in (16, 18, 19):
        return "branch or condition-register instruction (PC-relative)"
    if op in _DFORM:
        reads_rd, reads_ra = _DFORM[op]
        if op in (14, 15) and ra == 0:
            reads_ra = False   # addi/lis with rA=0 means literal zero
        used = ({rd} if reads_rd else set()) | ({ra} if reads_ra else set())
        bad = used & SCRATCH
        return f"reads scratch register r{min(bad)}" if bad else None
    if op == 31:
        xo = (word >> 1) & 0x3FF
        if xo in (339, 371):       # mfspr/mftb read no GPR
            return None
        bad = {rd, ra, rb} & SCRATCH
        return f"may read scratch register r{min(bad)}" if bad else None
    if op == 21 or op == 20 or op == 23:
        bad = {rd, ra} & SCRATCH
        return f"may read scratch register r{min(bad)}" if bad else None
    return f"opcode {op} is not on the known-safe list"


CAPTURE = re.compile(r"^(?:r(\d+)|\[r(\d+)([+-]0x[0-9a-fA-F]+|[+-]\d+)?\]|"
                     r"mem:(\S+)|sda:(-?0x[0-9a-fA-F]+|-?\d+))$")


def capture_insns(spec, syms, slot):
    """Instructions that store one captured word to slot(r12)."""
    if spec in ("none", "0"):
        return []
    m = CAPTURE.match(spec)
    if not m:
        raise DolphinError(f"bad capture '{spec}' (use r3, [r3+0x40], "
                           f"mem:SYMBOL_OR_ADDR or sda:-0x7f00)")
    reg, base, off, mem_, sda = m.groups()
    if reg is not None:
        r = int(reg)
        if r in SCRATCH or r > 31:
            raise DolphinError(f"r{r} is clobbered by the hook; capture "
                               f"another register")
        return [stw(r, slot, 12)]
    if base is not None:
        r = int(base)
        if r in SCRATCH or r > 31:
            raise DolphinError(f"r{r} is clobbered by the hook")
        return [lwz(11, int(off or "0", 0), r), stw(11, slot, 12)]
    if sda is not None:
        return [lwz(11, int(sda, 0), 13), stw(11, slot, 12)]
    addr = syms.addr(mem_)
    hi, lo = (addr + 0x8000) >> 16, addr & 0xFFFF
    return [lis(11, hi), lwz(11, lo, 11), stw(11, slot, 12)]


def hook_body(ring, tag, captures, syms):
    if not 0 <= tag <= 3:
        raise DolphinError("tag must be 0..3 (stored in the return address's "
                           "low bits)")
    if len(captures) > 2:
        raise DolphinError("at most two captures per hook")
    caps = list(captures) + ["none"] * (2 - len(captures))
    frame_off = FRAME_COUNTER - SDA_BASE
    body = [
        lis(12, (ring + 0x8000) >> 16),
        addi(12, 12, ring & 0xFFFF),
        lwz(11, 0, 12),
        addi(11, 11, ENTRY),
        rlwinm(11, 11, 0, 16, 27),      # wrap within 64 KiB, 16-byte entries
        stw(11, 0, 12),
        add(12, 12, 11),                # r12 = ring + offset; entry at +16
        mflr(0),
        ori(0, 0, tag),
        stw(0, 16, 12),
        lwz(11, frame_off, 13),
        stw(11, 20, 12),
    ]
    for i, spec in enumerate(caps):
        body += capture_insns(spec, syms, 24 + 4 * i)
    return body


def gecko_c2(addr, body, original):
    words = body + [original]
    if len(words) % 2 == 0:
        words.append(0x60000000)
    words.append(0)
    lines = [f"C2{addr & 0x1FFFFFF:06X} {len(words) // 2:08X}"]
    for i in range(0, len(words), 2):
        lines.append(f"{words[i]:08X} {words[i + 1]:08X}")
    return lines


# --------------------------------------------------------------------------- #
# Original instructions (from the retail DOL)
# --------------------------------------------------------------------------- #
def find_dol():
    cands = [ROOT / "orig/GL5E4F/sys/main.dol"]
    try:
        common = subprocess.run(
            ["git", "-C", str(ROOT), "rev-parse", "--path-format=absolute",
             "--git-common-dir"], capture_output=True, text=True, check=True
        ).stdout.strip()
        cands.append(Path(common).parent / "orig/GL5E4F/sys/main.dol")
    except (OSError, subprocess.CalledProcessError):
        pass
    for c in cands:
        if c.is_file():
            return c
    return None


def dol_word(dol_bytes, addr):
    offs = struct.unpack(">18I", dol_bytes[0:0x48])
    addrs = struct.unpack(">18I", dol_bytes[0x48:0x90])
    sizes = struct.unpack(">18I", dol_bytes[0x90:0xD8])
    for o, a, s in zip(offs, addrs, sizes):
        if s and a <= addr < a + s:
            return struct.unpack(">I", dol_bytes[o + addr - a:][:4])[0]
    raise DolphinError(f"0x{addr:08X} is not in any DOL section")


def original_word(addr, args):
    dol = find_dol()
    if dol:
        return dol_word(dol.read_bytes(), addr)
    mem = attach_or_exit(args)
    word = mem.scalar(addr, "u32")
    if word >> 26 == 18:
        raise DolphinError("no retail DOL and the live word is a branch "
                           "(already hooked?)")
    return word


# --------------------------------------------------------------------------- #
# Levels
# --------------------------------------------------------------------------- #
def level_names(read):
    """[(index, path, name)] from the level table via read(addr, n)."""
    def cstr(ptr):
        if not MEM1_BASE <= ptr < MEM1_BASE + MEM1_SIZE:
            return ""
        return read(ptr, 64).split(b"\0")[0].decode("latin-1")
    out = []
    for i in range(LEVEL_COUNT):
        path_ptr, name_ptr = struct.unpack(
            ">II", read(LEVEL_TABLE + i * LEVEL_ENTRY + 8, 8))
        out.append((i, cstr(path_ptr), cstr(name_ptr)))
    return out


def dol_reader():
    dol = find_dol()
    if not dol:
        return None
    data = dol.read_bytes()

    def read(addr, n):
        offs = struct.unpack(">18I", data[0:0x48])
        addrs = struct.unpack(">18I", data[0x48:0x90])
        sizes = struct.unpack(">18I", data[0x90:0xD8])
        for o, a, s in zip(offs, addrs, sizes):
            if s and a <= addr < a + s:
                return data[o + addr - a:o + addr - a + n]
        return b"\0" * n
    return read


def load_levels(args):
    read = dol_reader()
    if read is None:
        read = attach_or_exit(args).read
    return level_names(read)


def resolve_level(levels, text):
    if re.fullmatch(r"\d+", text):
        i = int(text)
        if 0 <= i < len(levels):
            return levels[i]
        raise DolphinError(f"level index {i} out of range 0..{len(levels) - 1}")
    exact = [l for l in levels if l[2].lower() == text.lower()]
    if exact:
        return exact[0]
    part = [l for l in levels if text.lower() in (l[1] + " " + l[2]).lower()]
    if len(part) == 1:
        return part[0]
    hint = ", ".join(l[2] for l in part[:8]) or "none"
    raise DolphinError(f"level '{text}' is ambiguous or unknown (matches: {hint})")


def level_label(levels, i):
    if i == 0xFFFFFFFF or i == -1:
        return "-1 (none)"
    return f"{i} {levels[i][2]}" if 0 <= i < len(levels) else f"{i} (?)"


# Code pointers in each level-table entry. +0x18/+0x1C/+0x20 hold the named
# *Init (29), *Reset (10) and *Update (4) functions in symbols.txt; +0x24 is
# the exit router, which stores the next level into lbl_804093B8 (OpenSaga
# other_level_override) from the players' sock position or spline side.
# +0x28 and +0x2C are unidentified.
LEVEL_CALLBACKS = (0x18, 0x1C, 0x20, 0x24, 0x28, 0x2C)
LEVEL_CALLBACK_NOTES = {0x18: "init", 0x1C: "reset", 0x20: "update", 0x24: "exit"}


def report_functions(path=None):
    """{name: (fuzzy percent, size)} from the objdiff report, or None without one."""
    try:
        report = json.loads(Path(path or REPORT).read_text())
    except (OSError, ValueError):
        return None
    return {fn["name"]: (fn.get("fuzzy_match_percent", 0.0), int(fn.get("size", 0)))
            for unit in report.get("units", []) for fn in unit.get("functions", [])}


def callback_label(tag, name, status):
    if status is None:
        return f"{tag}={name}"
    pct, size = status
    return f"{tag}={name} [{pct:.1f}% {size} B]"


def cmd_levels(args, syms):
    if args.unmatched:
        args.callbacks = True
    status = report_functions() if args.callbacks else None
    if args.unmatched and status is None:
        raise SystemExit(f"--unmatched needs {REPORT} (run bash build.sh)")
    read = dol_reader() if args.callbacks else None
    if args.callbacks and read is None:
        read = attach_or_exit(args).read
    for i, path, name in load_levels(args):
        if args.filter and args.filter.lower() not in (path + name).lower():
            continue
        line = f"{i:4d}  {name:<24} {path}"
        if args.callbacks:
            entry = read(LEVEL_TABLE + i * LEVEL_ENTRY, LEVEL_ENTRY)
            fns = []
            for off in LEVEL_CALLBACKS:
                ptr = struct.unpack(">I", entry[off:off + 4])[0]
                if ptr:
                    tag = LEVEL_CALLBACK_NOTES.get(off, f"+0x{off:X}")
                    name = syms.name(ptr)
                    st = status.get(name) if status else None
                    if args.unmatched and (st is None or st[0] >= 100.0):
                        continue
                    fns.append(callback_label(tag, name, st))
            if not fns and (args.unmatched or not args.filter):
                continue
            line += "\n        " + ("  ".join(fns) or "(no callbacks)")
        print(line)


def cmd_level(args, syms):
    levels = load_levels(args)
    mem = attach_or_exit(args)
    print(f"current: {level_label(levels, mem.scalar(CUR_LEVEL, 'u32'))}")
    print(f"pending: {level_label(levels, mem.scalar(NEXT_LEVEL, 'u32'))}")


def cmd_warp(args, syms):
    levels = load_levels(args)
    index, path, name = resolve_level(levels, args.level)
    if not args.write:
        print(f"target {index} {name} ({path})")
        print("dry run: pass --write to request the change (only with the "
              "user's go-ahead for this session)")
        return 0
    mem = attach_or_exit(args)
    cur, pending = mem.scalar(CUR_LEVEL, "s32"), mem.scalar(NEXT_LEVEL, "s32")
    print(f"current {level_label(levels, cur)}; target {index} {name} ({path})")
    if pending != -1:
        raise DolphinError(f"a level change to {level_label(levels, pending)} "
                           f"is already pending; wait for it to finish")
    if not frame_rate(mem, 0.1):
        raise DolphinError("emulation is paused; ask the user to resume")
    mem.write_u32(NEXT_LEVEL, index)
    print(f"requested: lbl_804093B0 = {index}")
    # Boot and the intro movie take the request (pending goes back to -1)
    # without loading anything; ask again until the level actually changes.
    # Waiting at least a second between requests keeps this to one store per
    # attempt, like the game's own.
    requests, last = 1, time.time()
    deadline = time.time() + args.timeout
    while time.time() < deadline:
        cur, pending = mem.scalar(CUR_LEVEL, "s32"), mem.scalar(NEXT_LEVEL, "s32")
        if cur == index and pending == -1:
            print(f"loaded: {index} {name}"
                  + (f" ({requests} requests)" if requests > 1 else ""))
            return 0
        if pending == -1 and cur != index and time.time() - last >= 1.0:
            mem.write_u32(NEXT_LEVEL, index)
            requests, last = requests + 1, time.time()
            print(f"dropped while on {level_label(levels, cur)} (boot or a "
                  f"movie); requested again")
        time.sleep(0.1)
    print(f"timeout: current {level_label(levels, cur)}, pending "
          f"{level_label(levels, pending)} after {requests} requests")
    return 2


def cmd_debug_menu(args, syms):
    mem = attach_or_exit(args)
    state = mem.scalar(DEBUG_MENU, "u32")
    print(f"gLegoOptionsMenuEnabled = {state}")
    want = 0 if args.off else 1
    if state == want:
        return 0
    if not args.write:
        print(f"dry run: pass --write to set it to {want} (only with the "
              f"user's go-ahead for this session)")
        return 0
    mem.write_u32(DEBUG_MENU, want)
    # The game never writes this word, but a Gecko 04 code (speedrun
    # practice: 044090F0 00000001) rewrites it every frame.
    start = mem.scalar(FRAME_COUNTER, "u32")
    deadline = time.time() + 1
    while mem.scalar(FRAME_COUNTER, "u32") - start < 3 and time.time() < deadline:
        time.sleep(0.01)
    now = mem.scalar(DEBUG_MENU, "u32")
    if now != want:
        print(f"reverted to {now}: an enabled Gecko code writes 0x804090F0 "
              f"every frame; ask the user to toggle that code instead")
        return 2
    print(f"set to {now}: LEGO Options is {'shown' if want else 'hidden'} "
          f"in the pause menu until reboot")
    return 0


def cmd_trace_code(args, syms):
    lines = [f"$Trace ring 0x{args.ring:08X} [decomp agent]",
             f"04{(args.ring + 4) & 0x1FFFFFF:06X} {RING_MAGIC:08X}"]
    tags = []
    for tag, spec in enumerate(args.functions):
        target, _, off = spec.partition("+")
        addr = syms.addr(target) + (int(off, 0) if off else 0)
        word = original_word(addr, args)
        why = check_relocatable(word)
        if why:
            sys.exit(f"error: {syms.name(addr)} first instruction "
                     f"0x{word:08X}: {why}; pick another offset with "
                     f"FUNC+0x4")
        if off and int(off, 0):
            print(f"warning: hooking {syms.name(addr)} mid-function; r11/r12 "
                  f"must be dead there", file=sys.stderr)
        body = hook_body(args.ring, tag, args.capture, syms)
        lines += gecko_c2(addr, body, word)
        tags.append(f"tag {tag} = {syms.name(addr)}")
    if args.ring == DEFAULT_RING:
        try:
            mem = LiveMem.attach(args.pid)
            area = mem.read(args.ring, RING_BYTES + 16)
            if any(area[8:]) and not mem.scalar(args.ring + 4, "u32") == RING_MAGIC:
                print("warning: ring area is not zero in the current state; "
                      "pick a free --ring", file=sys.stderr)
        except DolphinError:
            print("note: Dolphin not attached, ring area not checked",
                  file=sys.stderr)
    print("\n".join(lines))
    print("# " + "; ".join(tags) + f"; captures: {', '.join(args.capture) or 'none'}",
          file=sys.stderr)


# --------------------------------------------------------------------------- #
# Ring reader
# --------------------------------------------------------------------------- #
def ring_state(mem, ring):
    off, magic = struct.unpack(">II", mem.read(ring, 8))
    return {"offset": off, "magic": magic == RING_MAGIC}


def decode_entries(raw, start_off, end_off):
    """Entries written after start_off up to and including end_off."""
    n = ((end_off - start_off) & (RING_BYTES - 1)) // ENTRY
    off, out = start_off, []
    for _ in range(n):
        off = (off + ENTRY) & (RING_BYTES - 1)
        lr, frame, c0, c1 = struct.unpack(">4I", raw[off:off + ENTRY])
        out.append({"lr": lr & ~3, "tag": lr & 3, "frame": frame,
                    "c0": c0, "c1": c1})
    return out


def summarise(events, syms, top=25):
    callers = collections.Counter((e["tag"], syms.name(e["lr"]))
                                  for e in events)
    frames = {e["frame"] for e in events}
    span = (max(frames) - min(frames) + 1) if frames else 0
    print(f"{len(events)} calls over {span} frames")
    for (tag, name), n in callers.most_common(top):
        vals = collections.Counter(e["c0"] for e in events
                                   if e["tag"] == tag and syms.name(e["lr"]) == name)
        common = ", ".join(f"0x{v:X}x{c}" for v, c in vals.most_common(3))
        print(f"  {n:7d}  tag {tag}  {name:<40} c0: {common}")


def cmd_trace_tail(args, syms):
    mem = attach_or_exit(args)
    if not ring_state(mem, args.ring)["magic"]:
        sys.exit("error: logger not active (no magic at ring+4); ask the "
                 "user to enable the trace code")
    entries_base = args.ring + 16
    last = ring_state(mem, args.ring)["offset"]
    events, overruns, end = [], 0, time.time() + args.seconds
    while time.time() < end:
        cur = ring_state(mem, args.ring)["offset"]
        if cur != last:
            raw = mem.read(entries_base, RING_BYTES)
            got = decode_entries(raw, last, cur)
            if events and got and got[0]["frame"] - events[-1]["frame"] > 2:
                overruns += 1
            events += got
            last = cur
        time.sleep(0.001)
    if overruns:
        print(f"warning: {overruns} possible ring overruns; trace fewer "
              f"functions or poll a quieter state")
    if args.lr:
        want = syms.addr(args.lr)
        events = [e for e in events if e["lr"] == want]
    summarise(events, syms)
    if args.out:
        Path(args.out).write_text(json.dumps(events))
        print(f"wrote {len(events)} events to {args.out}")


# --------------------------------------------------------------------------- #
def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--pid", type=int, help="Dolphin pid (default: detect)")
    ap.add_argument("--ring", type=lambda s: int(s, 0), default=DEFAULT_RING,
                    help="trace ring address (default 0x80D00000)")
    sub = ap.add_subparsers(dest="cmd", required=True)

    p = sub.add_parser("status", help="report whether GL5E4F is running")
    p.set_defaults(func=cmd_status)

    p = sub.add_parser("launch", help="start a graphical Dolphin")
    p.add_argument("--state", help="save state file to load at boot")
    p.add_argument("--timeout", type=float, default=90)
    p.set_defaults(func=cmd_launch)

    p = sub.add_parser("close", help="close the Dolphin that launch started")
    p.add_argument("--force", action="store_true",
                   help="SIGKILL: unsaved emulator state is lost")
    p.set_defaults(func=cmd_close)

    p = sub.add_parser("screenshot", help="take a screenshot with Dolphin")
    p.add_argument("--out", help="PNG path (default: a temp dir; keep it "
                   "out of git)")
    p.set_defaults(func=cmd_screenshot)

    p = sub.add_parser("hotkey", help="press a Dolphin hotkey (list if none)")
    p.add_argument("id", nargs="?", help="e.g. load-state-load-state-slot-1")
    p.set_defaults(func=cmd_hotkey)

    p = sub.add_parser("read", help="read live values")
    p.add_argument("target", help="address or symbols.txt name, then +OFF "
                   "or ->OFF steps (gPlayer1Pointer->0x600)")
    p.add_argument("--type", default="u32", choices=[*SCALARS, "str"],
                   help="str: NUL-terminated string (up to %d bytes)" % STR_MAX)
    p.add_argument("--count", type=int, default=1,
                   help="values to read; for str, consecutive strings")
    p.set_defaults(func=cmd_read)

    p = sub.add_parser("wait", help="block until a value is reached")
    p.add_argument("target")
    p.add_argument("--type", default="u32", choices=SCALARS)
    p.add_argument("--equals", help="value to wait for (default: any change)")
    p.add_argument("--timeout", type=float, default=600)
    p.add_argument("--interval", type=float, default=0.05)
    p.set_defaults(func=cmd_wait)

    p = sub.add_parser("watch", help="print values as they change")
    p.add_argument("targets", nargs="+")
    p.add_argument("--type", default="u32", choices=SCALARS)
    p.add_argument("--seconds", type=float, default=10)
    p.add_argument("--interval", type=float, default=0.005)
    p.set_defaults(func=cmd_watch)

    p = sub.add_parser("snapshot", help="save MEM1 under dumps/")
    p.add_argument("label")
    p.add_argument("--note", help="game state, kept in dumps/manifest.txt")
    p.add_argument("--force", action="store_true")
    p.set_defaults(func=cmd_snapshot)

    p = sub.add_parser("levels", help="list level indices and names")
    p.add_argument("filter", nargs="?")
    p.add_argument("--callbacks", action="store_true",
                   help="also show each entry's code pointers (+0x24 = exit "
                   "router) by symbol; omits levels without any")
    p.add_argument("--unmatched", action="store_true",
                   help="with report.json: show only callbacks below 100%%, "
                   "with match percent and size (implies --callbacks)")
    p.set_defaults(func=cmd_levels)

    p = sub.add_parser("level", help="show the live current/pending level")
    p.set_defaults(func=cmd_level)

    p = sub.add_parser("warp", help="request a level change")
    p.add_argument("level", help="index, exact name (Dooku_B) or unique substring")
    p.add_argument("--write", action="store_true",
                   help="actually write; without it this is a dry run")
    p.add_argument("--timeout", type=float, default=60)
    p.set_defaults(func=cmd_warp)

    p = sub.add_parser("debug-menu", help="unhide the LEGO Options debug menu")
    p.add_argument("--off", action="store_true", help="hide it again")
    p.add_argument("--write", action="store_true")
    p.set_defaults(func=cmd_debug_menu)

    p = sub.add_parser("trace-code", help="print a Gecko logger for functions")
    p.add_argument("functions", nargs="+",
                   help="up to 4 FUNC or FUNC+0xOFF; tag = position")
    p.add_argument("--capture", action="append", default=[],
                   help="r3 | [r3+0x40] | mem:SYMBOL | sda:-0x7f00 (max 2)")
    p.set_defaults(func=cmd_trace_code)

    p = sub.add_parser("trace-tail", help="read the logger ring")
    p.add_argument("--seconds", type=float, default=10)
    p.add_argument("--lr", help="keep only calls returning to this address")
    p.add_argument("--out", help="write events as JSON (keep it local)")
    p.set_defaults(func=cmd_trace_tail)

    args = ap.parse_args(argv)
    if args.cmd == "trace-code" and len(args.functions) > 4:
        sys.exit("error: at most four hooked functions per ring (2-bit tag)")
    try:
        return args.func(args, Symbols()) or 0
    except DolphinError as e:
        sys.exit(f"error: {e}")


if __name__ == "__main__":
    sys.exit(main())
