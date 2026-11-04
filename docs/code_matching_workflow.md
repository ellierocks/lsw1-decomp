# C matching

Use ProDG to reproduce retail `GL5E4F`, then replace fallback assembly only
when object sections, relocations and linked ranges are proven. A function
can be **VERIFIED** without its unit being **PROMOTED**. Symbol names and
partial instruction scores do not measure promoted code.

## Toolchain

LSW1 GC game and Nu2 code use **SN Systems ProDG (GCC)**. The linked Dolphin
SDK libraries are Nintendo's CodeWarrior **GC/1.2.5n** builds; compile them
with the dolsdk2004 release flags (`tools/sdk_probe.py`, `score_fn.py --compiler
sdk`). CrashWOC uses CodeWarrior; its algorithms and types are donor evidence,
but its compiler flags and register-allocation behavior do not transfer.
[Strategy](strategy.md) decides which lane a task belongs to.

`tools/prodg_cc.py` drives preprocessing, ProDG `cc1`, `powerpc-eabi-as`
and DTK ELF fixup. Use the pinned `PRODG_VERSION` and `cflags_prodg` from
`configure.py` for a comparison object:

```py
Object(NonMatching, "numath/foo.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg)
```

`NonMatching` compiles the candidate for comparison while the retail target
object remains linked. A source file under `work/` is not compiled automatically.
`Matching` selects the compiled object for the final link. DTK's inherited
Metrowerks linker settings do not identify the game's compiler.

### Compiler audit and RTL diagnostics

The compiler audit, RTL dump instructions and the SN GCC v1.79 evaluation moved to
[compiler evaluation](compiler_evaluation.md). The pin stays ProDG 3.9.3 (`cc1.exe`, v1.76).

### Per-TU flag investigations

Treat a flag difference as a hypothesis until equivalent drafts strictly match
and existing matches in the same unit remain matches. Scheduling changes order;
optimization changes instruction selection and frames; frame-pointer use needs
a stack-derived base, not merely accesses through r31. Calls returning state in
CR0 or an inherited private register may instead identify assembly. Common-data
and SDA hypotheses also require declarations, sections and relocations.

Use `tools/compare_prodg_cc.py --source path/to/probe.c --out work/flags-run
--variant 'nosched:flag=-fno-schedule-insns;flag=-fno-schedule-insns2'` (one shell
line) for an isolated search. It uses the pinned compiler and strict verifier;
retain drafts, commands and rejected sets locally. Its baseline is shared
`-O2`, and `score_fn.py` also hardcodes that baseline: neither automatically
inherits a future per-unit override. Pass the candidate options explicitly in
experiments and use the configured ninja object for acceptance. Inspect the
wrapper's actual cc1 arguments, particularly `-G` routing.

Once confirmed, install a replacement per-object `cflags` list in
`configure.py`, regenerate, and run `verify_fn.py NAME --unit auto/path/unit
--build --strict-symbols` for every changed and previously matched function.
Compare other units' compile commands/objects against baseline, then run the
full build, linked-range/DOL comparison and progress report. Keep NonMatching
until the separate promotion gates pass. Rejected searches belong in an evidence
note with the remaining source/declaration/assembly hypothesis, not in a
speculative permanent override. See [podrace results and override wiring](build_config.md#per-tu-prodg-flags).

## Coding conventions

- Function names: `Nu{Subsystem}{Verb}` (engine), `Action_*`/`Condition_*` (dispatch).
- Struct tags: `nu{name}_s` (engine), `NU{NAME}_s` (game).
- Fields: `camelCase`; error strings: `str_FunctionName_Description`.
- Four-space C indentation; follow surrounding code.
- No comments in decomp source unless explaining a subtle match constraint.

## Select and prepare

1. Read current symbol evidence and rejected renames. Prefer a small function with
   a known prototype, useful types, few callees and a comparison unit already
   configured. Use `ls1_task_pack.py` and donor queues as discovery aids, not proof.
2. Check `tools/matching_progress.py`, then run `bash tools/bootstrap.sh` (or `bash build.sh`) to establish
   a working baseline. Keep the ProDG version and flags pinned. Do not start
   source tuning when the compiler, target split or retail input is broken.
3. Generate a task under ignored `work/`. Generation refuses to overwrite a
   populated task. Configure the source as a **NonMatching** ProDG comparison
   object; `work/Foo/source.c` is not compiled automatically. Prefer editing an
   existing comparison unit. Keep fallback assembly linked until promotion.
4. Write the signature and a plausible C body from the retail assembly and
   verified types. Treat donor implementations as evidence about algorithms;
   their MWCC code generation is not this game's ProDG code generation.

Use the [OpenSaga lane](symbol_donors/opensaga_matching.md) as a routine Nu2
donor lookup alongside CrashWOC and Mac evidence. It links GC body evidence,
orders small parser/helpers first, and identifies shared layouts and rejected
transfers. Generated task packs contain `donor_evidence.md`; inspect its source
leads and relevant callees/keyword strings against GC before drafting. The first
game/Nu2 experiment requires `--donor-note` recording inspected paths, confirmed
facts and rejected differences, or an explicit no-hit/unavailable limit. Notes
are inherited for that donor revision and automatically retained with lookup
provenance in the ledger. SDK units are exempt. Carry the conclusion into the PR
or plateau handoff; record reusable GC facts in shared headers/evidence pages.
See the [end-to-end workflow](symbol_donors/opensaga_matching.md#end-to-end-workflow).
Its expansion gate requires strict matches, not partial scores.

## Measure and iterate

From the repository root (replace the example function, unit and source):

```sh
python3 configure.py
python3 tools/hillclimb.py try NuVecScale --unit numath/nuvec \
  --source src/numath/nuvec.c --source src/numath/numath.h \
  --hypothesis 'baseline with existing vector types' \
  --donor-note '<inspected paths; GC-confirmed facts; differences or lookup limit>'
python3 tools/hillclimb.py status NuVecScale --unit numath/nuvec
python3 tools/verify_fn.py NuVecScale --unit numath/nuvec --build --strict-symbols --full
```

`--source` snapshots edited files; it still builds the configured unit. For an
unapplied experiment, use `--candidate scratch.c` for a complete unit or
`--body work/NAME/body.c` for a full function definition inserted into a temporary
copy of the unit. These modes run the strict scorer and record the exact compiled
unit under its configured source path, so the permuter can reuse the ledger's best
candidate. They do not edit the configured source. Include edited headers with
`--source` as before.

Interactive drafts and sweeps share assembly preprocessing and macro repairs.
Default condition-bit aliases are qualified for m2c; floating comparison `cror`
instructions and branches are retained, including unordered behavior. Arrayizing
absolute globals keeps explicit address offsets in bytes. Raw m2c pointer strides,
access widths and call arguments still require comparison with target assembly:
a guessed pointer type can scale `p += 4`, and a guessed prototype can invent an
argument. Prefer established unit/header prototypes. Indirect call declarations
use unspecified arguments only when every indirect target call has a nearby
`crclr/crset cr1eq`; mixed conventions require manual review.

For the search itself, use the fast scorer. It compiles one file straight to a
temporary object, without ninja or a ledger, in well under a second:

```sh
python3 tools/score_fn.py NuVecScale src/numath/nuvec.c
python3 tools/score_fn.py NuVecScale scratch.c --compile-only -o out.o   # permuter compile.sh
```

Its score has the same order as `hillclimb.py`.

ProDG shapes that recur in near-misses (the sweep tries the first two as variants):

- `v = A; if (f()) v = B; g = v;` hoists `A` into a callee-saved register before the
  call. Retail usually stored in both arms: `if (f()) g = B; else g = A;`
  (the `fn_800CCDE4` spline family in `auto/game/levels`).
- A `.rodata` float read inside a loop that stores floats is reloaded every iteration
  unless the extern is `const`; declared `const`, it is hoisted into `f31` after the
  loop's entry test (`fn_800C9490`).
- A constant kept in a callee-saved register across a call is a local: assigned after
  the call for a float (`reset = 0.4f;`, `fn_80100D5C`) or set before it for an int
  (`one = 1;`). Initialising at the declaration also works (`dt = lbl_80408E9C`,
  `fn_800EA4B4`).
- Loads scheduled early come from operands preloaded into locals before a branch
  (`fn_800EA4B4`'s lerp operands before the clamp).
- Retail store order is sometimes reversed: `v[1]` before `v[0]` (`fn_801042D4`),
  `s[2], s[1], s[0]` (`fn_80103F84`). Write the stores in target order. Two adjacent
  independent stores can also come out in the opposite order to the source
  (`fn_800ABC50`, `fn_800D5530`); the sweep's `swap_stores` variant tries each swap.
- An empty function whose target still opens a frame (`stwu r1, -N(r1)`, `addi`, `blr`)
  has an unused volatile local: `volatile s32 pad[(N - 8) / 4];` (`fn_800BA740`; the
  sweep's `empty_frame` repair).
- `mulli` + `add` + `lha 0x14(rN)` (or `addi` + `lbzx`) is a struct-array or member-array
  access, `((Frame *)base)[i].field`; raw `base + i*K + off` arithmetic folds the offset
  into the address instead (`fn_800B2ED4`, `fn_80108DA8`).
- A global accessed directly, not through a local pointer, gives CSE'd base+offset
  address constants (`fn_8010439C`). An indexed `p[0xD8 + i*0x34]` instead of a walking
  pointer fixes the induction setup (`fn_80106128`).
- A separate local for a call result (`m = f(obj);`) adds the extra callee-saved
  register (`fn_80103F84`). Splitting `if (a && b && d < r*r)` into a nested
  `r = ...; if (d < r*r)` matches compare placement (`fn_800F77A4`).
- A wrong callee return type changes the caller's schedule: `fn_80095558` returns
  `void` (r3 is never set), and declaring it `s32` cost `fn_800FC5C0` one `stb`/`li`
  pair. Check the callee's epilogue before keeping an `s32` prototype.
- m2c drafts list only the arguments a function reads, so `f(void *arg2)` compiles with
  the value in `r3` instead of `r5`. Write the full handler signature, e.g.
  `(sys, process, packet, params, count, firstTime, time)` for script actions or
  `(sys, process, packet, name, data)` for conditions. In `auto/game/gameaiscript`
  this alone matched 51 drafts. `tools/sweep.py` now pads every draft's signature this
  way (`s32 padI` for missing `argI`, `f32 fpadI` for missing `fargI`; GPR and FPR
  arguments are numbered separately) and keeps the better score. Rename the placeholders
  when the calling convention is known.
- `crclr cr1eq` (or `crset`) before a `bl` means the callee is variadic or unprototyped at
  that call; a prototyped declaration drops the instruction. Declare it as the target
  calls it (`sprintf(char *, const char *, ...)`, `memset();` in game code). m2c also
  guesses one prototype per callee, so take the real one from matched code instead;
  `tools/sweep.py` does both (`fix_prototypes`, and `adopt_own_prototype` when the unit
  already declares the target with a different signature). For a callee the repo has no
  prototype for it also tries the Mac demo's parameter kinds (`tools/mac_protos.py`; return
  type from m2c) and the target's own full Mac parameter list, keeping either only if it
  scores strictly better, so they cannot lower a result. Mac types are kinds, not structs. m2c's compare-in-GPR idioms
  (`mfcr` + `rlwinm`, `cror`, carry sequences) need the patched m2c from `tools/fetch_m2c.sh`;
  an unpatched checkout leaves them as `M2C_CARRY`/`M2C_ERROR` and the sweep reports UNHANDLED.
- Paired-single add/sub in game code is an inline `asm volatile` block, written with the
  `NuVecAddPS`/`NuVecSubPS` macros from `numath/nuvec_ps.h` (numeric register syntax,
  because the assembler rejects `f1` inside `asm()`). Use `(dst, a, b)`; the sweep emits
  them for you. Other paired-single operations have no macro yet.
- An out-parameter whose address is passed to several calls is kept in a callee-saved
  register when it is a 4- or 8-byte scalar. Declared at its real size (the 12-byte
  `NuSpecialFind` result), `addi rN, r1, 8` is recomputed before each call instead
  (`Condition_OnObjectInit`).
- A float `bge`/`ble` skip with no `cror` comes from a negated less-than,
  `if (!(a < b)) continue;`; `if (a >= b)` emits `cror` + `bso`
  (`Condition_LocatedCollectable`).
- When the target loads a pointer into a volatile register and copies it to a callee-saved
  one only on a later path (`mr r30, r4`), give the first load its own local and assign the
  long-lived variable inside that branch (`Condition_LocatedCollectable`).
- Array-indexed loops over `gPlayer1Pointer[i]` give the signed `cmpw` pointer compare
  that a pointer-increment loop does not; `goto` to a label after `return 1;` places a
  `return 0` block out of line (`Action_RaceOpponent`, `Action_SetDoomedEscapeLocator`).

### Store and load order search

When the only remaining difference is the order of independent stores or loads,
search the orderings instead of hand-tuning them. Copy the candidate to a scratch
file, mark the free statements and run `order_search.py`:

```c
void fn_800127D8(struct fn_800127D8_outer* o)
{
    struct fn_800127D8_inner* p = o->inner;
    /* order: decl */
    /* order: begin */
    p->f8 = 0;
    p->f14 = -1;
    p->f20 = -1;
    /* order: end */
}
```

```sh
python3 tools/order_search.py fn_800127D8 work/fn_800127D8/order.c --const-locals -j 8
```

Each `begin`/`end` block is split into top-level statements (a braced statement
counts as one); several blocks are permuted independently. `--const-locals` also
tries one shared `s32` local for each constant that is the whole right-hand side of
two or more assignments in a block, declared at `/* order: decl */` (or in a brace
around the block without that marker). Every variant is scored by `score_fn.py`;
the search stops at the first strict `MATCH` and refuses more than `--limit`
variants (default 7! = 5040, about two minutes with `-j 8`). `--dry-run` prints the
count. The best variant goes to `work/order_search/NAME/best.c`; port it into the
unit source and accept it with `verify_fn.py --build --strict-symbols`.

Blocks over 7 statements use `--climb` instead (`--const-locals` is exhaustive-only):

```sh
python3 tools/order_search.py fn_800262BC work/fn_800262BC/climb.c --climb -j 8 --budget 1500
```

It starts from the source order and scores every single-statement move (n·(n−1) per
block), takes the best strict improvement and repeats. At a plateau it restarts from
the best order with 2–4 random moves. It never rescores an order and stops at a strict
`MATCH` or after `--budget` variants (default 2000; `--seed` varies the restarts).
Both 12-store descriptor blocks in `nurndr` (479M orders each) matched this way in
under 3 minutes at `-j 8`: `fn_800262BC` after 1366 variants and `fn_80025FA0` after
495. The permuter had spent hours on them without a match.

### Variant search

For a draft within about ten instructions, `variant_search.py` tries the rewrites that solved
most plateaus without markers: the declaration order of the leading locals (stack layout and
register order), `x = c ? a : b;` against an explicit if/else, and `s32`/`u32` (also 16 and 8
bit) for each parameter and local. It varies only the named function in the unit source:

```sh
python3 tools/variant_search.py NAME src/UNIT.c -j 8 [--dry-run] [--max-changes 2]
```

Variants are tried fewest-changes first and the search stops at the first strict `MATCH`.
A space larger than `--limit` (default 2000) is truncated to its first `--limit` variants
instead of being refused. A
whole declaration permutation, or all ternaries together, count as one change. The best
variant goes to `work/variant_search/NAME/best.c`; port it and accept it with
`verify_fn.py --build --strict-symbols`. It reproduced both the `Curve2` local-order fix and the
ternary/if-else fix from the `nuanim_2` sprint within the first 32 compiles.

### Pattern library

When you do not know which C produces a target's instruction shape, look it up in code
that already matches with the same compiler. `pattern_library.py` indexes two corpora:

- **re4**: [RE4's decomp](https://github.com/adonis-singh/re4), where every DOL and REL
  unit matches retail. It is about 15,700 functions: C++ game code plus SN newlib C, with
  the `-O0` sound units skipped.
- **lsw1**: this project's ProDG functions that objdiff reports at 100%. The MWCC SDK
  units are excluded.

`build` compiles both corpora with the pinned v1.76 `cc1`/`cc1plus` plus `-gstabs`,
which adds line records without changing a single instruction (checked on
`cam_ctrl.cpp`). It disassembles the objects with dtk in the same syntax as
`build/GL5E4F/asm`. With a warm toolchain it takes about 25 s. It reads the RE4 clone
from `~/.cache/lsw1/donors/re4` (override with `LSW1_RE4`); clone it there with
`git clone https://github.com/adonis-singh/re4`. Rebuild the lsw1 corpus after a sprint
adds matches.

```sh
python3 tools/pattern_library.py build                       # work/pattern_library/
python3 tools/pattern_library.py similar NAME --source       # closest whole functions + their C
python3 tools/pattern_library.py window NAME --at 0x80138F04 # code containing the run at an address
python3 tools/pattern_library.py window --seq "lwz;cmpwi;beq;lbz;extsb"
python3 tools/pattern_library.py show re4:game/cam_ctrl.cpp:IsChangeCamera__13CameraControl
```

- `similar` ranks corpus functions by mnemonic 3-gram overlap, then by sequence
  similarity of operand shapes. It helps you choose a control-flow skeleton for a draft.
- `window` takes `--len` instructions around `--at`, which is usually the first mismatch
  `verify_fn.py` or `score_fn.py --diff` reports. It returns the longest corpus runs
  with the same mnemonics, ranked by how many operands also have the same shape. Each
  hit prints its instructions and the source lines that produced them.

RE4's `#line` renumbering is mapped back to the real lines of its sources. A hit is an
idiom to try, not evidence of identity, and a candidate still has to match through
`score_fn.py` and `verify_fn.py`. RE4 matched with a native v1.79 `cc1plus`, so a few
of its `-mfast-cast` conversions differ from v1.76 by a dead `mr`. The donor source and
the index stay in ignored local paths.

### decomp-permuter

For a close plateau that is not just an ordering problem, run the pinned
[decomp-permuter](https://github.com/simonlindholm/decomp-permuter). `bootstrap.sh`
fetches it (`tools/fetch_permuter.sh`; `DECOMP_PERMUTER` overrides the checkout).

```sh
python3 tools/permute_setup.py fn_8000A64C                 # writes work/permute/fn_8000A64C/
python3 tools/permute_setup.py fn_8000A64C --run -- -j 8 --stop-on-zero --best-only
```

`permute_setup.py` writes `base.c`, the best scored `hillclimb.py` snapshot of the
unit source (else the unit source; `--source` overrides), preprocessed with ProDG's
CPP. It also writes `target.o` (the retail DTK split object), `compile.sh` and
`settings.toml`, and prints the base score. `compile.sh` runs cc1, `as` and the dtk
fixup directly on the preprocessed input, which is about 3x faster than the
`score_fn.py NAME "$1" --compile-only -o "$3"` wrapper. Setup compiles the base both
ways and falls back to the wrapper, with a warning, if the objects differ. `--run`
reuses an existing directory unless `--force` is given.

**Focus the search.** Whole-function random mutation rarely lands a one-slot schedule
fix. Mark the region that differs in a copy of the candidate and pass it with
`--source`:

```c
/* permute: begin */      /* only randomize these lines: PERM_RANDOMIZE(...) */
...
/* permute: end */
/* permute: lineswap */   /* only reorder these whole lines: PERM_LINESWAP(...) */
...
/* permute: end */
```

The markers become PERM macros in `base.c`. Setup writes a marker-free `base_plain.c`
for scoring the base. With a focus region, the 180 s burst on `fn_80025FA0` ran 8.5k
iterations, against about 550 with the old compile step and no focus. Use `--climb`
above for pure statement order: the permuter samples line orders at random, and for
12 lines it found nothing that climbing found in 495 compiles.

**Read setup's warnings before running.** `permute_setup.py` warns when:

- the base differs structurally (instruction count, or under 90% exact). The permuter
  seldom fixes frame size, instruction count or missing code, so fix the C first.
- the function still has m2c names (`temp_r3`, `var_f31`). Rename or fold the
  temporaries and drop redundant casts first, so mutations act on real structure
  rather than on m2c noise.
- there is no focus region.

Scoring uses a verifier-backed dumper rather than normalised objects. The
permuter's objdump scorer compares relocation symbol names (`.rodata+0x8` never
equals `lbl_8018F150`) and diffs every function in the object. `settings.toml`
therefore points `objdump_command` at `tools/permute_objdump.py`. It runs the
`score_fn.py` comparison and prints each row the strict verifier accepted with the
target's text, and each other row with the candidate's text. Unless the verdict is a
strict `MATCH`, it adds a `verify_fn <verdict>` line. Permuter score 0 therefore
means `score_fn.py` MATCH, and the permuter's diff is the verifier's diff.

Output lands in `work/permute/NAME/output-*`. Minimise the change by hand, port it
into the unit source and accept it only with `verify_fn.py NAME --build
--strict-symbols`.

Run anything that outlives the session with `tools/permute_long.sh [--unit NAME] [--total
HOURS] SLOTS HOURS_PER_TARGET TARGET...`; never `setsid` or `nohup`. It starts a transient
`systemd-run --user` unit (default `lsw1-permute`) that holds a `sleep:idle` block
inhibitor, so the session ending neither kills the queue nor lets the idle policy suspend
the PC. The targets go into a backlog (`work/permute/backlog.txt`, deduped, order kept) and
the unit keeps `SLOTS` jobs running: whenever one ends (an early `--stop-on-zero` match, a
timeout, a crash) it pops the next target, until the backlog is empty or `--total` hours
have passed (each timeout is then capped by the time left). Size the backlog so early
matches cannot drain it: `python3 tools/permute_backlog.py --slots S --hours H --per P`
ranks near-miss candidates from `work/sweep/results.json` (optionally `--index` from
`worktree_cleanup.py`; matched and output-0 functions are dropped) and prints the
recommended size (at least 2 x S x H / P) on stderr. A missing `work/permute/NAME` is
created by the queue; a failed setup or job is logged (`skip NAME setup-failed`, `finish
NAME rc=N base=B best=S secs=T` in `queue.log`) and the queue moves on. Targets that
already have an `output-0*` are skipped as matched unless `--rerun`. `--add TARGET...`
appends to the running unit's backlog (exit 1 if it is not running). `--dry-run` prints the
command and queue. `systemctl --user stop <unit>` stops the jobs, writes
`queue_summary.txt` (matched, improved, unchanged, failed, not started) and keeps the
unstarted targets in the backlog; launching again with no TARGET continues from it.
Watch with `tools/permwatch.py` (`--once` prints a snapshot; `--targets` overrides the
queue). Review every improved output with
`tools/permute_review.py NAME [OUTPUT_DIR]` (default: best score). It diffs `source.c`
against `base.c`: the normalised target body, every changed top-level declaration
(prototype return types, extern types, structs, typedefs) and flags for `void *`
arithmetic, removed `volatile` and locals read before assignment. A changed
prototype with an identical body is a real result: the `fn_800FC5C0` match came from
`fn_80095558` returning `void`.

Record a candidate with
`hillclimb.py try` when it improves the incumbent or settles a hypothesis;
exploratory attempts do not need ledger entries.

The local compiler and strict verifier judge candidates. `hillclimb.py try` records
`--hypothesis` and every edited source/header supplied through repeated `--source`.
It runs the strict verifier with `--build`; stale objects and compiler errors cannot
become improvements. `--candidate` and `--body` select unapplied source to compile;
`--source` alone snapshots files and does not select the compiled candidate.

Its lexicographic score favors fully verified matches, fewer differences, smaller
instruction-count differences, more exact instructions and fewer unverified references.
That ranking guides search; it does not prove semantics or permit promotion. Losing
experiments remain useful evidence but do not replace the recorded best candidate.

Records are immutable numbered JSON files under `work/hillclimb/<identity>/`.
They include hypotheses, source text and hashes, Git head, build configuration /
verifier hashes, command, errors and the complete report. Identity includes the
function, unit and optional compiled symbol: repeat `--compiled-symbol` for status.
Only one writer can record a target at a time. An interrupted writer may leave
`.lock`; confirm no writer remains before removing that lock. Logs stay local;
retain a concise evidence summary and final C in the PR, not raw game artifacts.

Defaults are `--max-attempts 40`, `--patience 8` consecutive non-improving attempts,
and `--timeout 120` seconds per verifier invocation. Errors and timeouts count.
`PARK` describes exhaustion under those settings, not a command to abandon the target.
Adjust the budgets on the same ledger when further search is useful; retain prior
attempts. If parking, save the best candidate, remaining mismatch and next hypothesis.
Changes to configuration, symbol/split or verifier hashes block new attempts in that
ledger. Explain changed verification context and use a new `--ledger` so incompatible
scores are not compared. Source/header context changes still require judging whether
prior scores remain comparable.

## Verifying a candidate

Prerequisite: `bash tools/bootstrap.sh` (fetched toolchain) and `bash build.sh` once (patched DTK, split target objects under
`build/GL5E4F/obj`, retail DOL at `orig/GL5E4F/sys/main.dol`).

```sh
python3 tools/verify_fn.py NuRandInt --strict-symbols      # check an already-built object
python3 tools/verify_fn.py NuVecScale --unit numath/nuvec --build --strict-symbols # rebuild first
python3 tools/verify_fn.py Foo --object work/Foo/source.o  # arbitrary candidate object
python3 tools/verify_fn.py Foo --json                      # machine-readable
```

Pass `--compiled-symbol <name>` when the candidate emits a different function
name from the target. Use `--strict-symbols` for all acceptance checks.

What is compared, per instruction: the 32-bit word with relocation-patched
fields masked out, plus each relocation's type, symbol and addend. Register,
immediate, opcode and instruction-count differences are all failures;
mnemonic-order similarity is printed for orientation only and never counts.
Branches that leave the function without a relocation are resolved to symbols
before comparing. The target object bytes are also checked against the retail
DOL, and an `.rodata`/`.sdata2` constant referenced through a relocation is
compared with the retail bytes at the target symbol's address (width from the
load/store instruction).

Exit codes: `0` match, `1` mismatch, `2` error (unknown symbol, missing or stale
object, build/tool failure, inconsistent target), `3` code matches but some
references are only mapped by consistency (`--strict-symbols`). Verdicts:

| Verdict | Meaning |
|---------|---------|
| `MATCH` | Code identical; every reference verified by name, same `symbols.txt` address, or data bytes. |
| `MATCH (unverified references)` | Code identical, but some referenced symbols differ by name (for example `fseed` vs `lbl_80407B40`) and could only be mapped consistently. Lines under `references:` say why. |
| `MISMATCH` | Any word, relocation, length or constant differs. |

Staleness: without `--object`, the compiled object must be up to date
according to `ninja -n` (falls back to source mtime); otherwise the tool exits
2 instead of reporting on an old object. `--build` runs ninja and exits 2 with
the compiler output if it fails; `--allow-stale` skips the check.

Limitations (what a `MATCH` does not prove):

- Only the named function's code and the data it references through
  relocations are compared; data layout, symbol placement, section choice
  (mine `.sbss` vs retail `.sdata`), alignment and linker ordering are not.
- Referenced data is compared only for the width of the first load or store
  that uses it (at most 8 bytes); the rest of a table or struct is not checked.
- Two separate literals in mine that retail shares through one symbol are
  reported as a mismatch (the name mapping is strictly one-to-one).
- Exit 0 includes `MATCH (unverified references)`; scripts that must reject
  names mapped only by consistency should pass `--strict-symbols` (exit 3).
- Address-taken data (`addi rX, sym@l`, SDA21 `li`) and bss have no width or
  retail bytes to compare and stay "unverified".
- Debug-only differences and anything after linking (SDA base, final addresses)
  are out of scope; objdiff and a relinked DOL remain the final authority.
- The side-by-side text uses `dtk elf disasm`; if it fails the verdict is
  unaffected and raw words are shown.

Regression tests: `tests/test_verify_fn.py` (synthetic objects everywhere; the
genuine ProDG/retail cases run after `bash build.sh` and are required in the
GitHub retail CI job).

## Promotion

1. Restore the best source, rebuild, and run `verify_fn.py --build
   --strict-symbols` for **every function in the affected unit**. Resolve
   unverified names with address, type and data evidence.
2. Compare raw bytes of every owned section and all relocation offsets,
   types, addends and resolved section-relative targets against the DTK target
   object. Match data placement, initializers, alignment and symbol visibility.
3. For a partial assembly replacement, run `tools/asm_function_slice.py <name>`
   to plan the before/C/after split. `--write` emits slices only; review them
   and wire the source and `splits.txt` deliberately. Keep current boundaries
   unless binary evidence supports changing them.
4. Mark the verified unit `Matching` in `configure.py` and run `bash build.sh`.
   It compares promoted units with retail using `dol_compare.py --linked-units`.
   Compare the report and whole-image differences with the baseline too:
   previously matched code and unrelated linked ranges must not regress.
5. Retain target addresses, baseline/best attempt, exact verification commands,
   promoted bytes/functions, report delta and remaining limits in the PR.
   An improved `NonMatching` candidate is useful, but promotes zero bytes.

A strict function match proves only that function and the checked references.
Do not weaken the verifier, toolchain pins or split boundaries to improve a score.
The whole DOL remains the final goal; an existing link difference does not excuse
new differences in a promoted range.

### SDK complete-section gate

For a Dolphin unit, run `python3 tools/verify_unit.py dolphin/gx/GXTexture --build`
in addition to strict verification of every retained function. The gate compares
all allocated section extents, nonrelocated bytes and canonical relocations with
the DTK target, resolving local targets through the declared split bases and
external targets through unique symbols. It also checks retail bytes and encoded relocation destinations,
including branches, SDA references and initialized data pointers. SDA21 word/halfword offsets are normalized; unsupported
data relocation kinds, incomplete ownership fail closed. `.bss`/`.sbss` is proved by layout, not contents
(see [BSS layout policy](build_config.md#bss-layout-policy)); an unexplained gap or extra symbol fails.

`verify_fn.py` uses this same complete-unit proof to verify an address-taken
local table only when its section-relative address agrees with the reference.
An identical instruction sequence or consistent reference mapping alone still
cannot verify a table. This does not promote a unit automatically; the final
linked-range gate remains required. See [SDK import evidence](sdk_import.md)
for every promoted unit and repeatable verification commands.

### Object and linked-range checks

For the already promoted `numath/nurand` unit, after `bash build.sh`:

```sh
build/binutils/powerpc-eabi-objcopy -O binary -j .text build/GL5E4F/obj/numath/nurand.o /tmp/nurand-target.bin
build/binutils/powerpc-eabi-objcopy -O binary -j .text build/GL5E4F/src/numath/nurand.o /tmp/nurand-source.bin
cmp /tmp/nurand-target.bin /tmp/nurand-source.bin
build/binutils/powerpc-eabi-readelf -r build/GL5E4F/obj/numath/nurand.o
build/binutils/powerpc-eabi-readelf -r build/GL5E4F/src/numath/nurand.o
python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
```

Repeat the byte comparison for `.sdata` and any other owned sections. Relocation
symbol spelling may differ; compare the actual section-relative targets.
`--linked-units` reads complete units from `objdiff.json` and their section ranges
from `splits.txt`. `tests/test_dol_compare.py` covers this check; retail verifier
regressions live in `tests/test_verify_fn.py`.

## nurand promotion evidence

The initial promoted unit owns `.text` `0x8000C0C0..0x8000C1B8` (five functions,
248 bytes) and `.sdata` `0x80407B40..0x80407B64` (36 bytes). At promotion, both
sections and all five `R_PPC_EMB_SDA21` relocations matched the target object;
all 284 linked bytes matched retail. Changing the LCG increment from
`0x3c6ef35f` to `0x3c6ef35e` made the linked-range check fail at `0x8000C0D7`.

| `.text` offset | Use | Target |
|---|---|---|
| `0x00`, `0x1C` | `NuRandInt` load/store | `.sdata+0` (`lbl_80407B40`) |
| `0x48` | `NuRandSeed` store | `.sdata+0` |
| `0x58` | `NuRandSetSeed` address | `.sdata+8` (`lbl_80407B48`) |
| `0x84` | `NuRand` address | `.sdata+8` |

### Source changes

The pre-existing source matched `.text` but not data: it placed `fseed` and
`global_rand` in `.sbss`, whereas retail has 36 initialised bytes in `.sdata`.
ProDG 3.9.3 puts an explicit `= 0` initialiser in `.sdata` (it has no
zero-initialised-in-bss mode for this), so the source now declares:

| Retail address | Contents | Source |
|----------------|----------|--------|
| `0x80407B40` | `0x00000000` | `fseed` |
| `0x80407B44` | `0x00001365` | `unk_80407B44` (no reference found in code) |
| `0x80407B48` | `0x00000001` | `global_rand` |
| `0x80407B4C`-`0x80407B63` | `0 0 0 0 0 1` | `unk_80407B4C`..`unk_80407B60` |

ProDG's GCC keeps unreferenced statics and emits variables in definition order.
Objects larger than 8 bytes go to `.data`, so the 24-byte tail must be several
small objects; an array would land in `.data` and fail the layout.

Two things here are **not** established by evidence:

- Whether `unk_80407B44` and the tail belong to this translation unit. The tail
  has no direct references; it is carried verbatim because the current split
  includes it, and narrowing the split would leave `lbl_80407B48`
  (`size:0x1C` in `symbols.txt`) straddling the boundary. The Mac demo has a
  4-byte `global_rand`, so the real owner of the tail is probably a neighbour.
  Fix by splitting `symbols.txt` and `splits.txt` together (the `symbols.txt`
  change belongs to the symbol-naming workstream).
- The names `unk_*` are placeholders.

### Symbol names

Other retail units reference the dtk names. `fn_8000C038` (float-generator candidate) uses
`lbl_80407B40`, and a link with only local statics fails with
`undefined: 'lbl_80407B40'`. Rather than editing `symbols.txt`, `nurand.c` ends with
a top-level `asm` exporting `lbl_80407B40` and `lbl_80407B48` as aliases of
`fseed` and `global_rand`. The linked ELF shows `80407b40 d fseed` and
`80407b40 D lbl_80407B40` at the same address. Delete the `asm` block once
`symbols.txt` names those two symbols.

## Remaining link differences

The [SDK import](sdk_import.md) resolves the historical differences below by
using the existing retail ProDG linker with explicit section addresses and SDA
bases. The current linked image has **zero differing bytes**. These observations
are retained to explain why MWLD must not silently replace the SN linker.

The initial nurand promotion left the same 1,192 differing bytes as an all-target
object relink. This is historical evidence, not a substitute for measuring the
current build:

- 396 `.sdata2` references (1,188 differing bytes) used `r2` after linking with
  `mwldeppc`; retail uses `r13`. Both reached the same effective addresses.
  Retail `r13` is `0x8040FA40`; the linker's `.sdata2` base is `0x80413300`.
  Folding sections in the linker script was rejected by `mwld`. Investigate
  relocation handling or linker behavior before source-level tuning here.
- Two `__start` words differed: the `r2` initializer at `0x8000310C` and the
  `.sbss` clear end at `0x80003160`. Retail uses `0x8041FA40` and `0x8040B2E4`;
  the relink used `0x80413300` and `0x8040BD40`.

`build.sh` requests `main.dol` explicitly because the default Ninja checksum
check still expects a whole-image match. The configured CFA exclusion and
blocked startup relocation are documented in [build configuration](build_config.md).
Do not hide these link differences with a broader exclusion.

The preceding functions at `0x8000C038` and `0x8000C07C` use nurand's state
and are grouped with it in the Mac demo. Matching them and establishing the
owner of the `.sdata` tail are separate evidence tasks before extending this unit.

## Discovery tools

- `next_target.py`: ranks undone functions inside split units by callee count
  and size, skipping those with C, a verified ledger record or a claim.
- `bootstrap.sh`: fetches the toolchain that `score_fn.py` needs (fast, no DOL split).
- `ls1_task_pack.py <name-or-address> --out-dir work`: assembly, context and
  executable strict checks. Archived packs under
  [docs/history/task_packs](history/task_packs/index.md) use superseded names and
  toolchain steps; resolve addresses against current `symbols.txt`.
- `crashwoc_code_queue.py /path/to/crashwoc-decomp --include-norm`: donor
  candidates from `symbol_donors/nu2_fn_matches.tsv`. The default exact queue
  is mostly trivial stubs; normalized candidates need full verification.
- `sdk_probe.py --sdk /path/to/dolsdk2004 --all --tsv work/sdk_probe.tsv`:
  compiles donor SDK units with MWCC 1.2.5n, anchors functions to retail by
  name or layout (skipping linker-stripped functions) and classifies each one
  as `MATCH`, `UNVERIFIED`, `CODE_SAME` (code identical, names missing),
  `MISMATCH`, `NO_ANCHOR` or `ERROR`. Layout anchors can coincide by size; the
  verifier rejects those.
- `matching_progress.py`: objdiff report, configured with
  `functionRelocDiffs=all`. Lenient relocation mode previously produced a
  false `NuVecRotateAxis` match against the wrong float constant.
- [Research](research.md): symbol lookup, donor provenance, Ghidra and REA, and RAM dumps.
