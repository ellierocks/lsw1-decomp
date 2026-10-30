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

All scores use `tools/score_fn.py` (ProDG 3.9.3, `-O2`, configure cflags) and
the official `verify_fn.py --unit nu3dx/nuanim --build --strict-symbols`. The
two agree on the in-tree candidate (43/105).

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

### Remaining mismatch and next hypothesis

Target: `idx` lives in `r5` from the initial `li r5,0`. Candidate: `idx` is in
`r7`. Every case block then differs in register, not in instruction sequence.
The target also shares one `add r5,r5,r11` tail across cases 1–3, while case 0
skips it. The candidate's join is not merged that way.

Next hypothesis: introduce a temporary that is produced by the shared tail
(`last`) for cases 1–3 only, and force `idx` to be the first-written variable
after the switch. Test with a permutation search over declaration order and
over where `idx` is first assigned, not by adding asm.

## Boundary: `NuAnimCurve2CalcVal` (`0x80016E28`, size `0x704`, 449 insns)

Current source has a stub `return 0.0f;` (score 3/449). The retail body is a
type-dispatch over `r5`:

- `type == 4` (`0x80016E4C`–`0x80016EA4`): frame bit lookup. It uses
  `NuFloor(time_offset)` (`fn_8000B264`), `mask[chunk]`, `(mask >> (frame-1)) & 1`
  and converts to float. The donor `NuAnimCurve2CalcValEx` has the same branch:
  `(key_mask >> ((frame - 1) & 0x1f)) & 1`.
- Other `type` values run Hermite-style evaluators whose key layout is
  `curvedata->key_ixs / key_array`. The donor's `key_offsets / key_data` (stride
  `0x10` with header) is a different layout, so the donor's type 1/2 paths
  **do not transfer**.

Recommendation for a future lane: match the type-4 branch as the first verifiable
piece, once the `0x80016E28` function body can be split, not by naming it. Do not
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
