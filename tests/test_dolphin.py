"""Live Dolphin access and the Gecko logger builder."""
import contextlib
import io
import os
import struct
import subprocess
import sys
import tempfile
import textwrap
import unittest
from pathlib import Path
from unittest import mock

from tools import dolphin, ramdump


class EncoderTests(unittest.TestCase):
    def test_matches_known_words(self):
        # Words from the hand-assembled NuRandInt logger (binutils -mgekko).
        self.assertEqual(dolphin.lis(12, 0x80D0), 0x3D8080D0)
        self.assertEqual(dolphin.lwz(11, 0, 12), 0x816C0000)
        self.assertEqual(dolphin.addi(11, 11, 8), 0x396B0008)
        self.assertEqual(dolphin.rlwinm(11, 11, 0, 16, 28), 0x556B0438)
        self.assertEqual(dolphin.stw(11, 0, 12), 0x916C0000)
        self.assertEqual(dolphin.add(10, 12, 11), 0x7D4C5A14)
        self.assertEqual(dolphin.mflr(0), 0x7C0802A6)
        self.assertEqual(dolphin.ori(0, 0, 1), 0x60000001)
        self.assertEqual(dolphin.lwz(9, -0x7F00, 13), 0x812D8100)

    def test_relocatable_check(self):
        ok = dolphin.check_relocatable
        self.assertIsNone(ok(0x9421FFE0))        # stwu r1,-32(r1)
        self.assertIsNone(ok(0x7C0802A6))        # mflr r0
        self.assertIsNone(ok(0x812D8100))        # lwz r9,-0x7f00(r13)
        self.assertIsNone(ok(0x38600000))        # li r3,0
        self.assertIn("PC-relative", ok(0x48000011))  # bl
        self.assertIn("r0", ok(0x90010024))      # stw r0,36(r1)
        self.assertIn("r11", ok(0x7D6B5A14))     # add r11,r11,r11

    def test_c2_pads_to_whole_lines(self):
        # 2 body words + original: odd count, no nop, then terminator.
        self.assertEqual(dolphin.gecko_c2(0x8000C0C0, [1, 2], 3),
                         ["C200C0C0 00000002", "00000001 00000002",
                          "00000003 00000000"])
        # 1 body word + original: even count gets a nop before the terminator.
        self.assertEqual(dolphin.gecko_c2(0x80134684, [1], 3),
                         ["C2134684 00000002", "00000001 00000003",
                          "60000000 00000000"])

    def test_hook_rejects_scratch_captures(self):
        syms = dolphin.Symbols(Path("/nonexistent"))
        with self.assertRaises(dolphin.DolphinError):
            dolphin.hook_body(0x80D00000, 0, ["r12"], syms)
        body = dolphin.hook_body(0x80D00000, 2, ["r3", "sda:-0x7f00"], syms)
        self.assertIn(dolphin.ori(0, 0, 2), body)
        self.assertIn(dolphin.stw(3, 24, 12), body)
        self.assertEqual(body[-2:], [dolphin.lwz(11, -0x7F00, 13),
                                     dolphin.stw(11, 28, 12)])


class LevelTests(unittest.TestCase):
    LEVELS = [(0, "a\\Negotiations_A", "Negotiations_A"),
              (1, "a\\Kamino_A", "Kamino_A"), (2, "a\\Kamino_C", "Kamino_C"),
              (3, "Map\\Map_E", "Map_E")]

    def test_resolve_level(self):
        r = dolphin.resolve_level
        self.assertEqual(r(self.LEVELS, "3")[2], "Map_E")
        self.assertEqual(r(self.LEVELS, "kamino_c")[0], 2)
        self.assertEqual(r(self.LEVELS, "negot")[0], 0)
        with self.assertRaisesRegex(dolphin.DolphinError, "Kamino_A"):
            r(self.LEVELS, "kamino")
        with self.assertRaises(dolphin.DolphinError):
            r(self.LEVELS, "99")


class RingAndDolTests(unittest.TestCase):
    def test_decode_entries_wraps(self):
        raw = bytearray(dolphin.RING_BYTES)
        last = dolphin.RING_BYTES - dolphin.ENTRY
        raw[last:last + 16] = struct.pack(">4I", 0x80134688 | 1, 7, 2, 0)
        raw[0:16] = struct.pack(">4I", 0x8000C700, 8, 5, 6)
        got = dolphin.decode_entries(bytes(raw), last - dolphin.ENTRY, 0)
        self.assertEqual([(e["lr"], e["tag"], e["frame"]) for e in got],
                         [(0x80134688, 1, 7), (0x8000C700, 0, 8)])

    def test_dol_word(self):
        dol = bytearray(0x200)
        struct.pack_into(">I", dol, 0x00, 0x100)        # text0 offset
        struct.pack_into(">I", dol, 0x48, 0x80003100)   # text0 address
        struct.pack_into(">I", dol, 0x90, 0x100)        # text0 size
        struct.pack_into(">I", dol, 0x108, 0x9421FFE0)
        self.assertEqual(dolphin.dol_word(bytes(dol), 0x80003108), 0x9421FFE0)
        with self.assertRaises(dolphin.DolphinError):
            dolphin.dol_word(bytes(dol), 0x80100000)

    def test_parse_maps(self):
        text = ("7f00-7f10 r--p 00000000 00:01 1 /usr/lib/libc.so\n"
                "7f0000000000-7f0001800000 rw-s 00000000 00:1a 9 "
                "/dev/shm/dolphin-emu.42 (deleted)\n")
        self.assertEqual(list(dolphin.parse_maps(text)),
                         [(0x7F0000000000, 0x7F0001800000,
                           "/dev/shm/dolphin-emu.42 (deleted)")])


SHORTCUTS = ("com.t3tools.T3Code:capture-window -> Capture a window\n"
             "dolphin-emu:general-take-screenshot -> General: Take Screenshot\n"
             "dolphin-emu:load-state-load-state-slot-1 -> Load State: Slot 1\n")


class DesktopTests(unittest.TestCase):
    def test_lua_string_escapes(self):
        self.assertEqual(dolphin.lua_str('a "b" \\c'), '"a \\"b\\" \\\\c"')

    def test_user_dir_from_argv(self):
        f = dolphin.user_dir_from_argv
        self.assertEqual(f(["dolphin-emu", "-u", "/x", "-e", "g"]), Path("/x"))
        self.assertEqual(f(["dolphin-emu", "--user=/y"]), Path("/y"))
        self.assertEqual(f(["dolphin-emu"]),
                         Path.home() / ".local/share/dolphin-emu")

    def test_hyprctl_finds_newest_instance(self):
        with tempfile.TemporaryDirectory() as run:
            for i, name in enumerate(["old", "new"]):
                d = Path(run) / "hypr" / name
                d.mkdir(parents=True)
                os.utime(d, (1000 + i, 1000 + i))
            done = subprocess.CompletedProcess([], 0, stdout="ok")
            env = {"XDG_RUNTIME_DIR": run}
            with mock.patch.dict(os.environ, env, clear=True), \
                    mock.patch.object(subprocess, "run",
                                      return_value=done) as run_:
                dolphin.dispatch('hl.dsp.global("x")')
        self.assertEqual(run_.call_args.args[0],
                         ["hyprctl", "dispatch", 'hl.dsp.global("x")'])
        self.assertEqual(run_.call_args.kwargs["env"]
                         ["HYPRLAND_INSTANCE_SIGNATURE"], "new")

    def test_dispatch_error_is_raised(self):
        bad = subprocess.CompletedProcess([], 0, stdout="error: nope")
        with mock.patch.dict(os.environ, {"HYPRLAND_INSTANCE_SIGNATURE": "s"}), \
                mock.patch.object(subprocess, "run", return_value=bad):
            with self.assertRaisesRegex(dolphin.DolphinError, "nope"):
                dolphin.dispatch("hl.dsp.exec_cmd(1)")

    def test_press_hotkey(self):
        calls = []

        def fake(*args, json_out=False):
            calls.append(args)
            return SHORTCUTS if args == ("globalshortcuts",) else "ok"
        with mock.patch.object(dolphin, "hyprctl", side_effect=fake):
            self.assertEqual(dolphin.press_hotkey("general-take-screenshot"),
                             "dolphin-emu:general-take-screenshot")
            with self.assertRaisesRegex(dolphin.DolphinError, "slot-1"):
                dolphin.press_hotkey("load-state-slot-9")
        self.assertEqual(calls[1], ("dispatch", 'hl.dsp.global('
                                    '"dolphin-emu:general-take-screenshot")'))

    def test_press_hotkey_without_shortcuts_explains(self):
        with mock.patch.object(dolphin, "hyprctl", return_value=SHORTCUTS[:51]):
            with self.assertRaisesRegex(dolphin.DolphinError, "Window Focus"):
                dolphin.press_hotkey("general-take-screenshot")

    def test_launch_command_line(self):
        cfg = {"launcher": "/bin/dolphin wl", "iso": "/roms/LSW (USA).rvz"}
        sent = []
        with tempfile.TemporaryDirectory() as tmp, \
                mock.patch.object(dolphin, "machine_config", return_value=cfg), \
                mock.patch.object(dolphin, "dispatch", side_effect=sent.append), \
                mock.patch.object(dolphin, "LAUNCHED", Path(tmp) / "l.json"), \
                mock.patch.object(dolphin.LiveMem, "attach",
                                  side_effect=dolphin.DolphinError("none")), \
                contextlib.redirect_stdout(io.StringIO()):
            rc = dolphin.main(["launch", "--state", "/s/x.sav",
                               "--timeout", "0"])
        self.assertEqual(rc, 2)
        self.assertEqual(sent, ['hl.dsp.exec_cmd("\'/bin/dolphin wl\' '
                                '-C Dolphin.General.HotkeysRequireFocus=False '
                                '-C Dolphin.Interface.ConfirmStop=False '
                                '-e \'/roms/LSW (USA).rvz\' -s /s/x.sav")'])

    def test_close_refuses_unlaunched(self):
        with tempfile.TemporaryDirectory() as tmp, \
                mock.patch.object(dolphin, "LAUNCHED", Path(tmp) / "l.json"):
            with self.assertRaisesRegex(SystemExit, "user's own"):
                dolphin.main(["close"])


CHILD = textwrap.dedent(r"""
    import mmap, os, struct, sys
    path = sys.argv[1]
    with open(path, "w+b") as f:
        f.truncate(0x1800000)
        m = mmap.mmap(f.fileno(), 0x1800000, mmap.MAP_SHARED)
    os.unlink(path)
    m[0:6] = b"GL5E4F"
    m[0x40AA14:0x40AA18] = struct.pack(">I", 1234)
    m[0x407B40:0x407B44] = struct.pack(">I", 0xDEADBEEF)
    # Level table: entry 80 named Dooku_B; current level 123, none pending.
    m[0x100000:0x100008] = b"Dooku_B\0"
    m[0x101000:0x101010] = b"ep2\\Dooku_B\0\0\0\0\0"
    entry = 0x1D68E0 + 80 * 0xA8
    m[entry + 8:entry + 16] = struct.pack(">II", 0x80101000, 0x80100000)
    m[0x4093AC:0x4093B4] = struct.pack(">Ii", 123, -1)
    print("ready", flush=True)
    # Play the game: advance frames and apply pending level changes.
    import select, time
    frame = 1234
    while not select.select([sys.stdin], [], [], 0.005)[0]:
        frame += 1
        m[0x40AA14:0x40AA18] = struct.pack(">I", frame)
        cur, nxt = struct.unpack(">Ii", m[0x4093AC:0x4093B4])
        if nxt != -1:
            m[0x4093AC:0x4093B4] = struct.pack(">Ii", nxt, -1)
""")


@unittest.skipUnless(sys.platform.startswith("linux"), "needs /proc")
class FakeDolphinTests(unittest.TestCase):
    """A child maps a shared file named like Dolphin's MEM1."""

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        shm = Path(self.tmp.name) / "dolphin-emu.test"
        self.child = subprocess.Popen(
            [sys.executable, "-c", CHILD, str(shm)],
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
        self.assertEqual(self.child.stdout.readline().strip(), "ready")
        try:
            dolphin.LiveMem.attach(self.child.pid).close()
        except (dolphin.DolphinError, OSError) as e:
            self.tearDown()
            self.skipTest(f"cannot read child memory: {e}")

    def tearDown(self):
        if self.child.poll() is None:
            self.child.stdin.close()
            self.child.wait(timeout=10)
        self.child.stdout.close()
        self.tmp.cleanup()

    def test_live_reads(self):
        mem = dolphin.LiveMem.attach(self.child.pid)
        self.addCleanup(mem.close)
        self.assertGreaterEqual(mem.scalar(dolphin.FRAME_COUNTER, "u32"), 1234)
        self.assertEqual(mem.scalar(0xC0407B40, "u32"), 0xDEADBEEF)
        with self.assertRaises(dolphin.DolphinError):
            mem.read(0x81800000, 4)

    def test_wrong_game_is_reported(self):
        with self.assertRaisesRegex(dolphin.DolphinError, "GL5E4F"):
            dolphin.LiveMem.attach(self.child.pid, game_id=b"GXXE01")

    def test_snapshot_and_ramdump_live(self):
        dumps = Path(self.tmp.name) / "dumps"
        out = io.StringIO()
        with mock.patch.object(dolphin, "DUMPS_DIR", dumps), \
                contextlib.redirect_stdout(out):
            dolphin.main(["--pid", str(self.child.pid), "snapshot", "boss",
                          "--note", "Dooku stage 2"])
        self.assertEqual((dumps / "boss.raw").stat().st_size, dolphin.MEM1_SIZE)
        self.assertIn("boss = boss.raw  # Dooku stage 2",
                      (dumps / "manifest.txt").read_text())
        live = ramdump.Dump(ramdump.resolve_dump_path(f"live:{self.child.pid}"))
        self.assertEqual(live.scalar(0x80407B40, "u32"), 0xDEADBEEF)
        self.assertEqual(live.path.name, f"live:{self.child.pid}")

    def test_warp_dry_run_does_not_write(self):
        out = io.StringIO()
        with mock.patch.object(dolphin, "find_dol", return_value=None), \
                contextlib.redirect_stdout(out):
            dolphin.main(["--pid", str(self.child.pid), "warp", "Dooku_B"])
        mem = dolphin.LiveMem.attach(self.child.pid)
        self.addCleanup(mem.close)
        self.assertIn("dry run", out.getvalue())
        self.assertEqual(mem.scalar(dolphin.CUR_LEVEL, "u32"), 123)

    def test_warp_write_waits_for_load(self):
        out = io.StringIO()
        with mock.patch.object(dolphin, "find_dol", return_value=None), \
                contextlib.redirect_stdout(out):
            rc = dolphin.main(["--pid", str(self.child.pid), "warp", "dooku_b",
                               "--write", "--timeout", "5"])
        self.assertEqual(rc, 0, out.getvalue())
        self.assertIn("loaded: 80 Dooku_B", out.getvalue())

    def test_debug_menu_write(self):
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            dolphin.main(["--pid", str(self.child.pid), "debug-menu"])
            dolphin.main(["--pid", str(self.child.pid), "debug-menu", "--write"])
        mem = dolphin.LiveMem.attach(self.child.pid)
        self.addCleanup(mem.close)
        self.assertIn("dry run", out.getvalue())
        self.assertEqual(mem.scalar(dolphin.DEBUG_MENU, "u32"), 1)

    def test_wait_reaches_value(self):
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            rc = dolphin.main(["--pid", str(self.child.pid), "wait",
                               "0x8040AA14", "--type", "u32",
                               "--timeout", "2"])
        self.assertEqual(rc, 0)
        self.assertIn("reached", out.getvalue())


if __name__ == "__main__":
    unittest.main()
