# OpenSaga legacy curve animation evidence

Lane: legacy curve helpers in `nu3dx/nuanim` (GL5E4F `.text` `0x80016A98`–`0x80016FFF`).
Donor: OpenSaga `src/nu2api/nu3d/nu3d_includes.cpp` and
`src/nu2api/nucore/nuanim3.h` at pinned commit `7bb2402aa902d08f0979892ca23ee80e809e5ee1`,
read only. Donor code is Android x86 TCS without DWARF. Its struct layout, names
and matching percentages are evidence about semantics, not proof of LSW1 identity.
No donor source was copied into the candidate; every expression below was checked
against the retail GC body.

## Target: `NuAnimCurveCalcVal2` (`0x80016C84`, size `0x1A4`, 105 insns)

Source name: `config/GL5E4F/symbols.txt` already carries `NuAnimCurveCalcVal2`.
It is not in `docs/symbol_donors/rejected_renames.tsv`. The donor has the same name,
and its `NuAnimCurveCalcVal2(nuanimcurve_s*, nuanimtime_s*)` takes the same
arguments with the same Hermite / step-flag semantics.

### GC evidence (retail asm, `build/GL5E4F/asm/nu3dx/nuanim.s`)

| Access | Offset | Field (LSW1 header) | Donor field |
|---|---|---|---|
| `r4` time | `0x0` | `nuanimtime_s.time` | `time` |
| `r4` time_mask | `0xC` | `nuanimtime_s.time_mask` | `time_mask` |
| `r4` time_byte | `0x10` | `nuanimtime_s.time_byte` | `time_byte` |
| `r3` mask bytes | `0..3` | `nuanimcurve_s.mask` read as bytes | `key_mask[4]` |
| `r3` keys | `0x4` | `nuanimcurve_s.animkeys` | `keys` |
| `r3` numkeys | `0x8` | `nuanimcurve_s.numkeys` | `key_count` |
| `r3` flags | `0xC` | `nuanimcurve_s.flags` | `flags` |
| key `+0` | `0x0` | `nuanimkey_s.time` | `time` |
| key `+4` | `0x4` | `nuanimkey_s.dtime` | `reciprocal_span` |
| key `+8` | `0x8` | `nuanimkey_s.c` | `tangent` |
| key `+C` | `0xC` | `nuanimkey_s.d` | `value` |

The donor's `nuanimkey_s { time, reciprocal_span, tangent, value }` is a direct
match for the reconstructed LSW1 names. That makes `dtime` the reciprocal span
and `c` / `d` tangent / value. The donor corroborates these names, and the GC
body matches this layout. The struct is not renamed.

Byte order: the GC body reads mask byte `0` through `3` from the struct as
bytes. Case 0 (`time_byte == 0`) uses byte `3` (`lbz 0x3(r3)`). That is the
reverse of the donor's `key_mask[0]` role, so the donor's index mapping does
**not** transfer. The mapping used in the candidate was derived from the GC body.

### Math identity

The GC Hermite body (`f32` ops, `fmadds` contracted) computes

```
span = next.time - prev.time
delta = prev.value - next.value
t = (time - prev.time) * prev.reciprocal
t0 = prev.tangent * span
t1 = next.tangent * span
r = t * (t * (2*delta + t0 + t1 - 3*delta/t) ... )   (see candidate)
```

The candidate's expression tree matches the target register dataflow exactly.
The constant `-3.0f` at `lbl_80190214` was checked against retail bytes
(`c0400000`). The step branch (`flags & 1`) uses the donor's `d1 > d2` rule,
with `numkeys` as the upper bound.

## Candidate and scores

### 2026-11-12 continuation

The current ProDG candidate improves `NuAnimCurveCalcVal2` to **78/105 exact
instructions** (106 emitted). `python3 tools/score_fn.py NuAnimCurveCalcVal2
src/nu3dx/nuanim.c` and `python3 tools/verify_fn.py NuAnimCurveCalcVal2 --unit
nu3dx/nuanim --build --strict-symbols` agree that it remains a mismatch. Its
four mask-count cases match the retail body through instruction 53. Explicit
accumulation in cases 2 and 3 reproduces the target's `r5` count register;
forming the previous key first improves the alignment after the switch. The
first remaining difference is an extra `slwi r9,r5,4` before the key pointer
load. Next, test source forms that schedule the key-array load before the shift
while retaining the `r5` count and the step-path pointer shape. The later
Hermite loads and float schedule still differ. No function or unit was
promoted; the built DOL remains byte-identical through fallback assembly.

The earlier 43/105 experiments below document the starting candidate.

All scores use `tools/score_fn.py` (ProDG 3.9.3, `-O2`, configure cflags) and
the official `verify_fn.py --unit nu3dx/nuanim --build --strict-symbols`. The
two agreed on the earlier in-tree candidate (43/105).

| Attempt | Shape | Exact | Insns (mine/target) | First difference |
|---|---|---|---|---|
| baseline (existing `fn_80016C84`) | if/else, hoisted `ObjectAnim` | 26 | 101/105 | `lis` before `li` |
| v2 | if/else, direct `ObjectAnim` | 27 | 107/105 | `beq` vs `bge` |
| v3 | `switch(time_byte)` | 32 | 108/105 | `beq` distance (case block length) |
| a_case1 | case-1 operand order | 37 | 107/105 | branch distance |
| **h_split_add / k_ptr_arith (kept)** | `idx = partial; idx += last` join, `curr`-based keys | **43** | 108/105 | `li r5,0` vs `li r7,0` |
| p / q / r | `curr`-first pointers; inline mask bytes | 43 | 107/105 | same `idx` register |

Recorded with `hillclimb.py try`, ledger `work/hillclimb/nuanim-prodg-wiring/`
(attempt 1, best `[1, 69, 3, -43, 0]`). Earlier ledgers `f21833db90b3…`
(MWCC wiring) and `1ea1430b8b4f…` are **superseded**. The MWCC object was not
the game-code oracle.

### Earlier mismatch and hypothesis

Target: `idx` lives in `r5` from the initial `li r5,0`. Candidate: `idx` is in
`r7`. Every case block then differs in register, not in instruction sequence.
The target also shares one `add r5,r5,r11` tail across cases 1–3, while case 0
skips it. The candidate's join is not merged that way.

Next hypothesis: introduce a temporary that is produced by the shared tail
(`last`) for cases 1–3 only, and force `idx` to be the first-written variable
after the switch. Test with a permutation search over declaration order and
over where `idx` is first assigned, not by adding asm.

## Earlier boundary: `NuAnimCurve2CalcVal` (`0x80016E28`, size `0x704`, 449 insns)

The earlier source had a stub `return 0.0f;` (score 3/449). The retail body is a
type-dispatch over `r5`:

- `type == 4` (`0x80016E4C`–`0x80016EA4`): frame bit lookup. It uses
  `NuFloor(time_offset)` (`fn_8000B264`), `mask[chunk]`, `(mask >> (frame-1)) & 1`
  and converts to float. The donor `NuAnimCurve2CalcValEx` has the same branch:
  `(key_mask >> ((frame - 1) & 0x1f)) & 1`.
- Other `type` values run Hermite-style evaluators whose key layout is
  `curvedata->key_ixs / key_array`. The donor's `key_offsets / key_data` (stride
  `0x10` with header) is a different layout, so the donor's type 1/2 paths
  **do not transfer**.

The type-4 branch is a useful first comparison block within the complete function.
Keep its split boundary unchanged; acceptance requires the entire function. Do not
transplant the donor's `CalcValEx` type 1/2 layout.

## Other functions in the TU

`fn_80016A98` (`NuAnimData2CalcTime`, `0x1EC`) was not attempted.
Its donor counterpart is the ANI3/ANI4 time setup; the donor branch
(`0x414E4934` magic) belongs to a different format. Those paths are not
used for the legacy old-format branch.

## Build wiring

`configure.py` previously built `nu3dx/nuanim.c` with MWCC GC/1.3.2 (no
`compiler=` argument), while every other game unit uses ProDG 3.9.3 (AGENTS.md,
code matching workflow). Change: `Object(NonMatching, "nu3dx/nuanim.c",
compiler="prodg", compiler_version=PRODG_VERSION, cflags=cflags_prodg)`.
This is `NonMatching` (comparison only); the retail object still links.
Compiler flags were not changed.

## Tool-side effect (disclosure)

To compile, the first build was run with `build/compilers`, `build/binutils`
and `build/tools/wibo` as symlinks into the chore worktree
(`chore-workflow-prodg-restart/build/`). Ninja's download rules then re-downloaded the
pinned tools into those target directories (`compilers` 20251118,
`binutils` 2.42-2, `wibo` 1.0.3), rewriting about 228 files there. The symlinks were removed.
The restored files are the same pinned release artifacts (same sizes as before
for `ProDG/3.9.3/cc1.exe`, `CPP.exe`, `ngccc.exe` and `wibo`), but the old bytes
were not hash-checked. The chore session should confirm its tools if it
depends on exact bytes.


## nuanim item follow-up, 2026-11-13

Bench run: `t3-64a20ec0-263fa37007ce45cf-1794598235`, baseline `591ab1a`.
PR 162 and its parked legacy notes were reviewed before this pass. The four
remaining functions stay `NonMatching`; **new strict matches 0, promoted bytes 0**.
The four preexisting strict-only functions are reverifications, not new work.

| Function / GC address | Bytes | Start → retained exact instructions | Distinct compiled candidates | Remaining mismatch / next hypothesis |
|---|---:|---:|---:|---|
| `NuAnimCurveCalcVal2` / `0x80016C84` | 420 | 78/105 → 85/105 | 29 | First 79 instructions agree; Hermite float live ranges and operation scheduling differ. Keep the recovered mask/step control flow; compare value/time load lifetimes in the Hermite block. |
| `NuAnimCurve2CalcVal` / `0x80016E28` | 1,796 | 3/449 stub → 340/449 | 54 | Candidate saves f31 and uses a 0x30 frame; target uses 0x28. Diagnostic callback tails merge in the candidate. Scope polynomial temporaries separately from the shared return value, then compare each typed case. |
| `NuAnimCurve2SetApplyBlendToJoint2TransLoc` / `0x8001752C` | 2,420 | no body → 441/605 | 579 | Frame 0x130 now matches. First differing block assigns the scale/matrix pointers to different saved registers; curve loop address formation and result/type-load scheduling still differ. Recover the second output's indexed store and shared return lifetimes before register tuning. |
| `NuAnimCurve2SetApplyToJointTransLoc` / `0x80017EA0` | 1,720 | no body → 350/430 | 107 | Frame 0xA0 and instruction count match. Flags/joint saved registers differ first; later the next type byte loads after the previous result store. Test a paired expression that retains the next signed type across that store and explicit zero-store sequencing. |

A sweep of the four functions applied no matches and found no near misses. Its
legacy draft scored 32/105, unblended joint draft 151/430; evaluator/blended drafts
failed compilation. Search results with a speculative `volatile` inverse vector
were rejected in favor of the recovered nonvolatile layout. No permuter was run.

### Recovered layout and donor limits

The evaluator's GC key formats are not the legacy key format:

| Format | Stride | GC fields |
|---|---:|---|
| Float key | 16 | time +0, reciprocal +4, value +8, tangent +12 |
| Packed key | 4 | signed value halfword +0, signed tangent byte +2, time byte +3 |
| Wide key | 6 | signed value halfword +0, signed tangent byte +2, time halfword +4 |

For `key = popcount(mask prefix)` and chunk index `index`, the first key is:
float `[index+key-1]`, packed type 2 `[index+key+1]`, wide type 5
`[index+key+2]`, packed type 6 `[index+key+3]`. Type 3 loads the float
at byte offset `(index+key-1)*8`. Type 2 has an 8-byte scale header; type 5
has 12 bytes; type 6 has 16 bytes with its first-key expression overlapping the
last header-sized slot as encoded by the target. Type 4 has no source `&31` on
the shift count; copying that donor expression changes code generation.

OpenSaga revision `7bb2402aa902d08f0979892ca23ee80e809e5ee1` was reviewed read-only:
legacy evaluator at `nu3d_includes.cpp:5941`, `CalcValEx` at 6048 and unblended
joint at 8620, plus `nuanim3.h`. Legacy tangent/value and joint override accesses
agree with GC. The donor mask byte order and packed-key layout do not transfer.
The evaluator has no exact donor name hit; its Japanese PC pair at `0x004F3A90`
confirms the 0..6 dispatch and Hermite family, not the GC layouts. The blended
joint has no donor name/search hit and no JP pair; the other two have no JP pair.
Each target has a `hillclimb.py try --donor-note` ledger entry for this revision.

The blended ABI carries weight in f1 after eight integer/pointer arguments.
Joint/scale/parent-scale/matrix/override and three optional outputs occupy caller
stack offsets +8 through +0x24. The corrected wrapper remains a strict 24/24
match. Joint pivot is +0x40, flags byte +0x51; override rotation/translation/scale
are +0/+12/+24, signed clamp halfwords +0x24..+0x2E, flags byte +0x31.
The nine blended vec/angle locals occupy +8..+0x94 in the 0x130 frame.
The target independently advances curve, float-constant and output cursors;
combining them initially produced a smaller 0x128 frame and 279/605 instructions.

### Search and tooling handoff

Legacy `PLATEAU` at 29 distinct candidates, evaluator at 54 and unblended joint
at 107 each led to parking and moving to the next function. Final source
reverification repeats those notices; no further matching search followed them.
The blended function printed `PLATEAU` at 579 distinct candidates, best
449/605 unchanged for the last 161; it was parked. That best search result uses
a speculative `volatile` inverse vector and was rejected; the retained
nonvolatile candidate is 441/605. The two bounded blended searches each had
one compile error.

The first evaluator variant search had 141/250 compile errors: declaration
permutations moved initialized dependent pointers ahead of their inputs. Reading
the warning led to separating declarations from assignments; the next search
had 1/200 errors. Other errors included one C89 declaration after a statement,
six joint signedness trials conflicting with the existing prototype, one malformed
scratch brace rewrite (fixed before searching), and two rejected blended search
variants. Raw diagnostics and experiments remain in ignored `work/`.

The scorer's dropped-later-extern blocker is fixed and measured in
[workflow gap 18](../workflow_gaps.md#18-body-splicing-dropped-declarations-needed-before-the-new-use).
The same candidate changes from ERROR to 254/449; no strict match came from the
fix. m2c's pointer-return inference still loses evaluator float expressions;
this pass recovered those by hand. Ghidra's local project was unavailable.
Neither runtime/gameplay naming nor an Ellie question was needed for these
engine-layout comparisons.
