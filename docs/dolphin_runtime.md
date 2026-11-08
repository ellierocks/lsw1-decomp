# Dolphin runtime evidence

A running game answers questions that static analysis only guesses at: which
caller reaches a function, what a field holds in a known state, which global
changes when something happens on screen, or how often a function runs.
`tools/dolphin.py` reads the user's running Dolphin directly, and
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

- **Read-only.** Agents read through `/proc/<pid>/mem` and never write to
  Dolphin's memory. Code reaches the game only through a Gecko code that the
  user pastes and enables.
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

## Asking the user

When a question needs a state that the game isn't in, or needs a Gecko code
enabled, ask the user with **one self-contained request**:

1. **State:** the level, area, menu or event. Give the route if it isn't obvious,
   and say what to avoid, such as "stand still, don't enter range of Dooku".
2. **Codes** (optional): the exact Gecko lines from `dolphin.py trace-code`. Ask
   for a new code under Tools → Cheats Manager → Gecko Codes, with cheats
   enabled in the config. Remind them that most codes take effect at boot or on
   the next frame, and that they must disable the code afterwards.
3. **Signal:** what to say back ("ready"), or a value the agent will watch for.
4. **Why:** one sentence naming the question this answers.

Example:

> Could you load Episode II, Chapter 6 and reach Dooku's second phase (after the
> first lightsaber duel)? Please stand still near the entrance. Also enable this
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
