# Matching quickstart

The shortest path from a prompt to one recorded C-matching attempt. Reference
details live in [C matching](code_matching_workflow.md) and the plan in
[strategy](strategy.md).

## 1. Bootstrap once per checkout

```sh
bash tools/bootstrap.sh
```

This fetches wibo, ProDG, binutils and objdiff, and builds the patched DTK. It
does not split the DOL. Worktrees share one toolchain: `build/tools`,
`build/compilers`, `build/binutils` and the retail DOL are linked from
`~/.cache/lsw1` (or the primary checkout), so a new worktree is ready in about a
second. The pinned dolsdk2004 and OpenSaga donors are cloned into the same cache
(`LSW1_CACHE` sets the shared cache; the existing toolchain/SDK overrides remain
unchanged). Verification also needs `orig/GL5E4F/sys/main.dol`; see the [README](../README.md#prerequisites).
Run `bash build.sh` when you change splits, configuration or compiler wiring.

OpenSaga uses the pin in `config/donors/opensaga.json`; `OPENSAGA` overrides its
checkout and `LSW1_NO_OPENSAGA=1` skips the fetch. See the
[donor workflow](symbol_donors/opensaga_matching.md#end-to-end-workflow) for offline
setup and checkout validation.

If a tool says the toolchain is missing, run the bootstrap again. For an SDK unit
rather than a game function, skip to `tools/sdk_import_unit.py` in
[SDK import](sdk_import.md#importing-a-unit-quickly).

## 2. Pick a target

`python3 tools/kickoff.py` prints the shortlist: sweep state, near misses, the top
`next_target.py --by-unit` units with ownership filtering, and a recommended lane;
`--start UNIT` records the queue and baseline. Its yield priorities come from
[strategy](strategy.md#choosing-a-task); the [sprint reference](sprint_workflow.md#scope)
describes the sweep and ownership detection. Inspect related code shapes before choosing;
the ranking does not estimate matching time. Naming and donor mining serve that target.
For SDK units, `python3 tools/sdk_probe.py --pending` ranks the queue and
`python3 tools/sdk_import_unit.py UNIT --apply` imports and proves a unit.

```sh
python3 tools/kickoff.py                # state, lanes, recommendation
python3 tools/sweep.py --apply          # then bash build.sh and verify_fn.py on each match
python3 tools/sweep_permute.py          # permuter on the biggest near-misses (background it)
python3 tools/sweep_permute.py --apply-saved  # later: write its confirmed matches
python3 tools/next_target.py
python3 tools/next_target.py --min-size 0x80 --max-size 0x400 --unit auto/game/ --limit 50
python3 tools/next_target.py --jp-paired --max-size 0x400   # readable JP PC x86 body
```

`next_target.py` lists undone functions inside split units, sorted by direct callee count and
then size, with their unit, source, OpenSaga lookup state and Japanese PC pair
(`jp`, see [PC builds](research.md#pc-builds)). Donor name leads, then PC pairs,
break otherwise equal ranking ties. Choose a `ready` row that is not `claimed`. An empty list does not mean the work is finished:
most game code lies outside split units (Phase 2 in [strategy](strategy.md)).
The default shows only the 15 smallest rows, so a lane of mid-size targets needs
`--min-size`, `--unit` (repeatable prefix) and a larger `--limit`. A `(new file)`
source means the unit has no C yet: create the `source_path` it names
(`src/<unit>.c`) and draft into it. If `configure.py` has no entry for the unit
yet (for example `auto/game/pickups`), add one beside the other `auto/` entries:
`Object(NonMatching, "auto/game/pickups.c", compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg)`.
A unit missing from `src/` is work to start, not a blocker.

For game/Nu2 candidates, read the [OpenSaga lane](symbol_donors/opensaga_matching.md)
and its [remaining queue](symbol_donors/opensaga_audit.md#remaining-matching-queue).
Name leads need current identity, unit and ownership checks.

## 3. Read the target

```sh
python3 tools/ls1_task_pack.py NAME --out-dir work
```

This writes `work/NAME/asm.s`, `context.md` and `donor_evidence.md` (ignored by git).
`asm.s` comes from the units the current build lists, and `draft_fn.py` and `sweep.py`
rewrite it (logging `refreshed ...`) whenever a rebuild changes it; other pack files stay.
Read the donor evidence, inspect listed source and relevant callees/keywords
against GC, and record what transfers or the no-hit/unavailable limit. Check
`symbol_donors/rejected_renames.tsv` before using a name.

For a first draft, `python3 tools/draft_fn.py NAME` writes `work/NAME/draft_m2c.c` from the
pinned [m2c](https://github.com/matt-kempster/m2c) (`tools/fetch_m2c.sh`, run by the
bootstrap). It does not compile until the arguments have real struct types; use it for the
control flow and the offsets.

It also writes `work/NAME/ghidra.c`, which shows named small-data globals, loops and
switches that m2c leaves raw. Read it for loops, switches and nested conditions; skip it
for straight-line leaves. The bootstrap creates the Ghidra project; without
it, run `tools/ghidra_decompile.py setup` once. For "who else reads or writes this global", run
`tools/ghidra_decompile.py xrefs NAME`; REA covers callers, strings and tables interactively
([Ghidra and REA](research.md#ghidra-and-rea)).

When the target has a Japanese PC pair, the task pack and `draft_fn.py` also write
`work/NAME/jp_x86.s`: the MSVC x86 body with paired names, aligned call lists, switch
tables and an audit verdict. After the opt-in `tools/ghidra_decompile.py setup-pc`,
`draft_fn.py` adds `jp_ghidra.c`. `next_target.py --jp-paired` lists paired targets
([PC builds](research.md#pc-builds)).

## 4. Score candidates (fast loop)

```sh
python3 tools/score_fn.py NAME src/UNIT.c
```

It takes well under a second and prints the verdict, score, exact instructions
and the first difference. Change one source choice per attempt, guided by that
first difference. `--diff` adds the disassembled TARGET/MINE listing (mismatched rows
marked `!!`) instead of hex words. `--body try.c` scores `try.c`, one full definition
of NAME plus any new externs, in place of NAME in the unit source without editing it.
Externs and `struct` definitions the unit already declares are dropped from the body, so
m2c's guessed prototypes cannot conflict. `--apply` writes it in only on a strict MATCH:

```sh
python3 tools/score_fn.py NAME src/UNIT.c --body /tmp/try.c --diff --apply
```

When the draft is close, try the known cheap rewrites in one run (declaration order of
locals, ternary against if/else, `s32`/`u32` for each integer, at most two changes at once):

```sh
python3 tools/variant_search.py NAME src/UNIT.c -j 8
```

If only the order of independent stores or loads differs, mark those statements
with `/* order: begin */` and `/* order: end */` in a scratch copy and search every
ordering ([order search](code_matching_workflow.md#store-and-load-order-search)):

```sh
python3 tools/order_search.py NAME work/NAME/order.c --const-locals -j 8
```

For any other close plateau, run decomp-permuter scored by the strict verifier
([details](code_matching_workflow.md#decomp-permuter)):

```sh
python3 tools/permute_setup.py NAME --run -- -j 8 --stop-on-zero --best-only
```

## 5. Record and accept

```sh
python3 tools/hillclimb.py try NAME --unit UNIT --source src/UNIT.c \
  --hypothesis 'one falsifiable change' \
  --donor-note '<inspected paths; GC-confirmed facts; differences or lookup limit>'
python3 tools/hillclimb.py status NAME --unit UNIT
python3 tools/verify_fn.py NAME --unit UNIT --build --strict-symbols
```

The first game/Nu2 attempt requires `--donor-note`; later attempts inherit it
for the same donor revision. When the pinned checkout has no name hit, `hillclimb.py`
records that limit as the note itself. Keep the review in the PR or plateau handoff. SDK
units use the Dolphin lane and are exempt.

Record a candidate with `try` when it improves the best score or settles a
hypothesis. `hillclimb.py` reports `PARK` when its configured search budget is exhausted.
Choose whether to adjust `--patience`/`--max-attempts` on the same ledger or park the
target with its best score, remaining mismatch and next hypothesis. Keep the history;
a default search budget is not a limit on the agent's task.

## 6. Rules that do not bend

- Never weaken the verifier, change pins or move a split to raise a score.
- A strict match is VERIFIED, not PROMOTED. Promotion needs the checks in
  [C matching](code_matching_workflow.md#promotion).
- Donor C is evidence. A function counts only after it matches with this
  game's compiler.
