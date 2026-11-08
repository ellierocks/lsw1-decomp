# Dolphin runtime evidence

A running game answers questions that static analysis only guesses at: which
caller reaches a function, what a field holds in a known state, which global
changes when something happens on screen, or how often a function runs.
`tools/dolphin.py` reads the user's running Dolphin directly, can start one on
the desktop, take screenshots, move the game to a chosen level, and
`tools/ramdump.py` accepts `live` wherever it takes a dump.

Runtime evidence supports a type, name or field offset. It does not replace the
oracle: confirm every layout against the target assembly, and never report it as
matching progress.

## When to use it

Reach for Dolphin when a static answer stays ambiguous after the assembly,
Ghidra/REA and donors, for example:

- a struct field's meaning or enum values (read it in two states and diff);
- an indirect call target, vtable or callback table entry;
- which callers actually reach a function, and with which arguments;
- whether a global is a frame counter, timer, flag word or pointer;
- string, script or asset tables as they look once loaded.

Skip it for questions that the assembly, symbols or a dump already answer.

## Rules

- **Reads by default.** Agents read through `/proc/<pid>/mem`. The only writes
  are the [drivers](#driving-the-game) below, such as `warp --write`, and only
  after the user has said in this session that agents may drive the game. Never
  drive while the user is playing or practising. Code reaches the game only
  through a Gecko code that the user pastes and enables.
- **Launch only when needed.** Agents may start a graphical Dolphin with
  `launch` when nothing is running, and close only the one they started. The
  user's own Dolphin is theirs: leave it open, and don't press its hotkeys or
  take screenshots while they are playing or have its settings open.
- **Ask, then verify.** Ask the user for a game state (see below). Then confirm
  it with `dolphin.py wait` or `read`; don't rely on the reply alone.
- **Clean up.** When a trace is finished, ask the user to disable the logger
  code. `dolphin.py status` warns while a ring is still active.
- **Keep artifacts local.** Snapshots (`dumps/*.raw`) and trace JSON contain
  copyrighted game memory. They stay out of git, PRs and CI; record only the
  facts you derived from them.
- **Name the state.** Each finding records the level, menu or event, and the
  frame, so that someone else can reproduce it.

## Check what is running

```sh
python3 tools/dolphin.py status
```

The output reports the Dolphin pid and the game, whether frames are advancing
(running or paused) and whether a trace ring is active. If no game is attached,
it says why: Dolphin isn't running, no game is booted, or a different game is
loaded. Set `DOLPHIN_PID` or pass `--pid` when several Dolphins are running.
This needs Linux, and ptrace access to Dolphin (`kernel.yama.ptrace_scope=0`
here).

## Desktop: launch, screenshots and hotkeys

```sh
python3 tools/dolphin.py launch                    # boots GL5E4F, waits for frames
python3 tools/dolphin.py launch --state ~/x.sav    # boot into a save state
python3 tools/dolphin.py screenshot --out /tmp/claude-1000/shots/now.png
python3 tools/dolphin.py hotkey                    # list hotkey ids
python3 tools/dolphin.py hotkey load-state-load-state-slot-1
python3 tools/dolphin.py close
```

- **Setup.** Machine settings live outside git in
  `~/.config/lsw1-decomp/dolphin.json` (`{"launcher": ..., "iso": ...}`), or
  `DOLPHIN_LAUNCHER` / `DOLPHIN_ISO`. The desktop is Hyprland: since 0.57,
  `hyprctl dispatch` takes Lua (`hl.dsp.exec_cmd(...)`, `hl.dsp.global(...)`).
  The tool finds the Hyprland instance itself, since agent shells don't carry
  the desktop environment.
- **`launch`** has Hyprland start Dolphin, so it belongs to the desktop and
  outlives the agent's shell, then waits until frames advance. It passes two
  `-C` overrides for that run only: `HotkeysRequireFocus=False` (needed for
  global shortcuts) and `ConfirmStop=False` (so `close` can finish).
- **`screenshot`** presses Dolphin's own *Take Screenshot* hotkey through the
  portal global shortcut, waits for the new PNG under
  `<user dir>/ScreenShots/GL5E4F/`, and prints its path. It captures the
  emulated frame at internal resolution without focusing the window. Read the
  PNG to look at it; copies go outside the repo.
- **`hotkey`** presses any registered Dolphin hotkey, such as save and load
  state slots, pause or frame stepping. This needs the Wayland build and
  *Hotkeys Require Window Focus* unticked (Config → Interface); otherwise
  Dolphin registers no global shortcuts and the tool says so. Loading a state
  or pausing is driving the game: the [rules](#rules) apply.
- **`close`** only closes a Dolphin that `launch` recorded. It sends SIGTERM,
  which Dolphin treats as closing the main window. Dolphin's handler fires once,
  so if a confirmation dialog appears, ask the user; `--force` sends SIGKILL.

## Asking the user

When a question needs a state that the game isn't in, first check whether a
[driver](#driving-the-game) reaches it and whether the user has allowed driving
this session. Otherwise, or when a Gecko code is needed, ask the user with
**one self-contained request**:

1. **State:** the level, area, menu or event. Give the route if it isn't obvious,
   and say what to avoid, such as "stand still, don't enter range of Dooku".
2. **Codes** (optional): the exact Gecko lines from `dolphin.py trace-code`. Ask
   for a new code under Tools → Cheats Manager → Gecko Codes, with cheats
   enabled in the config. Remind them that most codes take effect at boot or on
   the next frame, and that they must disable the code afterwards.
3. **Signal:** what to say back ("ready"), or a value the agent will watch for.
4. **Why:** one sentence naming the question this answers.

Example:

> Could you load the Count Dooku chapter (Episode II) and reach his second phase
> (after the first lightsaber duel)? Please stand still near the entrance. Also enable this
> Gecko code, named "trace NuRandInt":
>
> ```
> 04D00004 54524345
> C200C0C0 00000008
> ...
> ```
>
> Say "ready" when you're there. I'm checking which callers consume the RNG in
> that fight. Disable the code when I say I'm done.

After the reply, verify the state, for example with
`dolphin.py wait <global> --equals <value>` or `ramdump.py read live <addr>`.
Then continue.

**Unattended runs** (delegated children, sprints and loops) don't block on the
user. Record the request in the handoff under "Dolphin request", worded exactly
as above, and carry on with the static evidence.

## Live reads, waits and snapshots

```sh
python3 tools/dolphin.py read retraceCount
python3 tools/dolphin.py read 0x803DCF70 --type ptr --count 8
python3 tools/dolphin.py wait lbl_80409410 --type ptr          # any change
python3 tools/dolphin.py wait 0x80407B40 --equals 0 --timeout 120
python3 tools/dolphin.py watch 0x8040AA14 lbl_80407B40 --seconds 5
python3 tools/dolphin.py snapshot dooku_stage2 --note "Ep2 Dooku, phase 2, idle"

python3 tools/ramdump.py read live 0x80407B40 --type u32
python3 tools/ramdump.py diff dooku_stage2 live --section sbss
```

- **Targets** are addresses or `symbols.txt` names.
- **`watch`** prints a line each time a value changes, with the frame from
  `retraceCount`. Values that change many times within a frame alias, so trace
  them instead.
- **`snapshot`** writes `dumps/<label>.raw` and adds the label to
  `dumps/manifest.txt`, so later commands accept the label.
- **`live`** in `ramdump.py` reads all of MEM1 once per command. For a
  before-and-after comparison, snapshot both states and diff the labels.

Memory Engine locks and code writes are unreliable for experiments. A lock
can't hold a value that the game rewrites many times per frame, and Dolphin's
JIT can keep running code it has already compiled. Use Gecko codes instead.

## Driving the game

Drivers put the game into a state by making the same store that the game
itself makes. They are dry runs unless given `--write`. They refuse to act while
emulation is paused or another change is pending, and they wait until the game
confirms the new state.

```sh
python3 tools/dolphin.py levels                 # index, name and path
python3 tools/dolphin.py levels dooku
python3 tools/dolphin.py level                  # live current and pending level
python3 tools/dolphin.py warp Dooku_B           # dry run
python3 tools/dolphin.py warp Dooku_B --write   # load it and wait
```

`warp` takes an index, an exact name or a unique substring.

`debug-menu --write` sets `gLegoOptionsMenuEnabled` (`0x804090F0`) to 1. The game
only reads that word, so "LEGO Options" stays in the pause menu until reboot.
The menu itself (scene select, game mode, unlocks, toggles) is documented in
[menus](menu_system.md#debug-menu--lego-options). Prefer it when a state needs
several of those options; the user can operate it, and a driver can later make
the same stores directly.

**How the warp works:**
- The level table is `lbl_801D68E0`: 127 entries of `0xA8` bytes, with the path
  at `+8` and the name at `+0xC`. Indices 0–122 are story and bonus levels,
  then 123 `Map_E` (the Dex's Diner hub), 124 `titles`, 125 `credits` and 126
  `status`.
- `lbl_804093AC` holds the current level and `lbl_804093B0` the pending level
  (-1 for none). The main loop in `game/main.s` loads the pending level.
- Doors, cutscenes, menus and `SceneSelect_HandleSelection` change the level by
  storing an index there, so `warp` does exactly the same thing.

**Not covered:** a warp loads the level with the current party and story
state. It doesn't select characters, restore a save or skip into a later phase
of a fight. Ask the user for those, or make them reproducible with a Dolphin
save state that the user names.

**Adding a driver:** as research finds more of the game's own state switches
(character select, story flags, phase triggers, free play), turn each one into
a driver in `tools/dolphin.py` with the same shape:
1. Cite the game code that makes the same store, in the doc and the code.
2. Make it a dry run unless given `--write`, and check for paused emulation and
   pending changes.
3. Wait until the game confirms the new state, and report a timeout.
4. Test it against the fake Dolphin in `tests/test_dolphin.py`.

Never patch code or freeze values to force a state. Drivers make one-shot data
stores; anything ongoing is a Gecko code the user enables.

## Call tracing with a Gecko logger

`trace-code` builds a Gecko C2 hook at the entry of up to four functions:

```sh
python3 tools/dolphin.py trace-code NuRandInt fn_8000C038 --capture sda:-0x7f00
python3 tools/dolphin.py trace-code FindForceObject --capture r4 --capture "[r3+0x20]"
```

Each call appends a 16-byte entry to a 64 KiB ring at `--ring` (default
`0x80D00000`):

- the caller's return address, with the hook's position (0–3) as a tag in its
  low bits;
- `retraceCount`;
- two captured words.

Captures:

| Capture | Meaning |
|---------|---------|
| `r3`–`r10` | an argument register at entry (`r0`, `r11` and `r12` are the hook's scratch registers) |
| `[r3+0x40]` | a word loaded through a register |
| `mem:SYMBOL` or `mem:0xADDR` | an absolute word |
| `sda:-0x7f00` | an `r13`-relative small-data word |

The tool takes the displaced instruction from the retail DOL. It refuses
instructions that are PC-relative or read the scratch registers. In that case,
hook a later instruction with `FUNC+0x4`, but only where `r11` and `r12` are
dead. A `04` line keeps a magic word at `ring+4`, so the reader knows the code
is active.

```sh
python3 tools/dolphin.py trace-tail --seconds 30
python3 tools/dolphin.py trace-tail --seconds 60 --lr 0x80134688 --out /tmp/trace.json
```

`trace-tail` polls the ring about every millisecond. It prints the call count
for each caller and tag, with the most common first captured values, and can
save every event as JSON outside the repo.

Limits:

- **Ring location.** The default ring was unused in the levels tested so far.
  `trace-code` warns when it isn't zero in the current state; pick another free
  `--ring` if so.
- **Overruns.** At thousands of calls per frame the reader can fall behind.
  Trace fewer functions, or find a quieter state.
- **Bad captures.** Loading a capture through a bad pointer makes Dolphin raise
  an invalid-read panic. Capture pointers only where they are known to be valid.

## Recorded runtime evidence

These come from live sessions and are leads to confirm against assembly before
they are used as names or types.

- **RNG.** `NuRandInt` (`0x8000C0C0`) and `fn_8000C038` (probably
  `NuRandFloat`) share the LCG seed `lbl_80407B40` (`seed * 1664525 +
  1013904223`). `NuRand` uses `lbl_80407B48`. The seed is set to zero only at
  power-on. In the Dooku fight, consumers are:
  - audio, about 6 calls per frame;
  - particles: `fn_80072410` (spawn) and `fn_8007373C` (update);
  - camera shake: `fn_800957E0`.
- **Force objects.** `lbl_80409410` points to the force system:
  - `+0x57C`: disabled flag;
  - `+0x58C`: entry array, 0xF4 bytes per entry;
  - `+0x590`: entry count.

  Each entry holds:
  - `+0x00`: name;
  - `+0x04`: object reference;
  - `+0x20`: u16 flags;
  - `+0x74`: range;
  - `+0x78`: position.
- **AI script actions.** A loaded action is `{char **params, int count,
  table entry *}`, and table entries are `{name, fn}`. In
  `Action_UseForce`, `+0xB2`, `+0xB3` and `+0xB4` hold the throwable count,
  in-range count and chosen entry. The chosen object is
  `found[NuRandInt() % count]` (`0x80134684`).

Add new findings here in the same form: the state, the observation, and what
still needs assembly confirmation.
