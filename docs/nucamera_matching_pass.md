# nucamera matching pass, 2026-11-12

Baseline at `f691955`: 459,240 / 1,612,280 matched code bytes and 2,826 / 4,318
functions. The ProDG 3.9.3 comparison unit is `auto/nu3d/ngc/nucamera`.

**Result:** all four previously unattempted bodies are now strict
`verify_fn.py --strict-symbols` MATCHes (VERIFIED, not PROMOTED):
`fn_8002303C` (1,132 B, 283/283), `fn_80022A88` (864 B, 216/216),
`fn_800225C4` (1,220 B, 305/305) and `fn_80021784` (3,648 B, 912/912), 6,864 B
in total. The unit stays NonMatching because `fn_80022DE8` is still a parked
NONMATCHING draft, so the linked code count is unchanged. The candidate bodies,
task packs and hillclimb ledgers are in this checkout's ignored `work/`
directory. The first-attempt table below records the starting scores.

## Strict matches

What fixed each body, confirmed against target assembly:

- `fn_8002303C`: see the typed continuation below. The record layout there is
  the one now in source.
- `fn_80022A88`: write the aligned scratch pointer twice from non-self sources
  (`v = (struct nuvec4_s *)buffer; v = aligned(v);`). This drops its frame
  base, so the global stores alias the scratch and force the target's
  reloads. A 79-variant permutation of the buffer-write order found
  `v2, v3, w2, w3, b0, b2, b3, b1`. The inner loop advances `p++` in the
  for-increment and reads `p[1]` in the else branch, which avoids `lwzu`.
  The prototype is `void (struct camera_record *, struct nuvec4_s *faces,
  struct nuvec4_s *verts, struct nuvec_s *dir)`.
- `fn_800225C4`: the locals are `struct nuvec_s e[8]` and
  `struct nuvec4_s v[33]`, which gives the `0x300` frame. Keeping `t = &v[1]`
  as a call argument holds that address in `r29`. Indexing the ring as
  `lbl_80337860[i]` and `[(i + 1) & 7]`, rather than through a pointer, gives
  the separate `+4`/`+8` base registers. It returns `void` and takes the
  direction by value.
- `fn_80021784`: the cylinder flag is bit 0 of `box->p0.x` read as an
  integer, and `struct camera_child40` gains its `unk24` pad so that `c`
  sits at `+0x28`. Sin and cos of `0x2000` and `0x6000` are computed first.
  Both branches build `v[0]` (origin), `v[1]` (first edge) and `v[2]` (unit
  normal of edge x dir). The key fix is a single float `l` reused for
  `|v[2]|^2`, its inverse length, the `e[1]` dot and the radius
  `NuFsqrt(l * l + d1 * d1)`, with `d1` the `e[2]` dot computed second. One
  pseudo puts all four in `f2` and frees a callee-saved FPR, which moved the
  sin/cos registers to `f21..f24` (601 to 900 of 912). Buffer writes index
  `lbl_80407CE4 + k` and finish with `lbl_80407CE4 += 4`.

All four use the donor note "OpenSaga 7bb2402 nucamera.c: no name hit; no JP
PC pair listed" in their `hillclimb.py try` records.

## First attempts

| Function and range | Size | Best strict score | Exact instructions | First difference | Next hypothesis |
| --- | ---: | --- | ---: | --- | --- |
| `fn_800225C4`, `800225C4..80022A88` | 1,220 B | `[1,316,68,-56,0]` | 56/305 | extra target `f17` save after matching `0x300` frame | Replace the scratch frame padding with typed vec4 and edge locals; then reproduce the first transform and FPR lifetimes before tuning the loop. |
| `fn_80022A88`, `80022A88..80022DE8` | 864 B | `[1,258,65,-49,0]` | 49/216 | extra `f28` save; first body difference follows the matching `0x460` frame | The scratch 0x3E0-byte backing allocation reproduces the frame but is not a proven type. Model the aligned matrix and vec4 locals, then remove the extra `f28` lifetime and compare the matrix copy. |
| `fn_8002303C`, `8002303C..800234A8` | 1,132 B | `[1,287,28,-57,0]` | 57/283 | frame `0xE0` versus `0x50` | Type the 0x68-byte record and its three child arrays; preserve the 0x40-byte copied matrix and vector locals, then compare the group transition loop. |
| `fn_80021784`, `80021784..800225C4` | 3,648 B | `[1,1315,105,-67,2]` | 67/912 | frame `0x318` versus `0xE8` | Start from the record types established by the smaller functions and replace m2c's unrelated scalar temporaries with the target's stack-resident aggregate locals. |

For `fn_800225C4`, a second scratch candidate enlarged the stack backing to 153 floats. This reproduced the `0x300` frame and improved 24 to 56 exact instructions, but target saves `f17` and the draft does not. The padding is not an accepted type.

For `fn_80022A88`, a second scratch candidate enlarged the aligned local backing to `0x3E0` bytes. This reproduced the target `0x460` frame and improved 39 to 49 exact instructions, but introduced an extra `f28` save and still has 65 extra instructions. The allocation is an experiment, not an accepted layout.

The recovered sweep drafts came from the main checkout's
`work/sweep/fn_*/sweep_draft.c` without changing that checkout. The
`fn_8002303C` draft needed its stale `fn_80022DE8` declaration removed and
camera matrix accesses corrected to `lbl_80313268.mtx._30` through `_32`
before it compiled. These are comparison drafts, not accepted source.

## GC record evidence

The `fn_8002303C` target assembly strides `lbl_803378CC` by `0x68` and
reads the group byte at `+0x67`. It tests a mask at `+0x58` against its two
arguments, uses position floats at `+0x30..+0x38`, and reads a squared
distance threshold at `+0x60`. It dispatches three child arrays from
`+0x44`, `+0x40` and `+0x48`; their counts are bytes at `+0x65`, `+0x64`
and `+0x66`, respectively. The first child advances by `0x40` bytes, the
second by `0x30`. These offsets and strides are from the GC assembly at
`0x800230E0..0x80023470`; names and higher-level behavior remain unproved.
The same body reads camera position from `lbl_80313268.mtx +0x30..+0x38`.

`fn_800225C4` transforms four points derived from its input's four 12-byte
vectors and writes through `lbl_803145E0`, incrementing `lbl_80407CE4`.
`fn_80022A88` manually aligns a stack pointer to 16 bytes before copying
its matrix and transforming point pairs. These observations come from the
target assembly and justify the next type and frame experiments.

OpenSaga revision `7bb2402aa902d08f0979892ca23ee80e809e5ee1` has no
name hit for any of the four functions. Its `src/nu2api/nu3d/nucamera.c`
was reviewed: it provides projection and clipping routines, but no
corresponding record traversal or triangle construction body. No Japanese
PC pair is recorded for these targets. No donor layout or gameplay name was
transferred.

## Typed `fn_8002303C` continuation

The provisional comparison source is now in `src/auto/nu3d/ngc/nucamera.c`.
Its `camera_record` keeps the GC-supported `0x68` stride: a `0x40` matrix,
child pointers at `0x40`, `0x44` and `0x48`, vector floats at `0x4C..0x54`,
a mask at `0x58`, radii at `0x5C` and `0x60`, then count bytes and a group
byte at `0x64..0x67`. Matrix translation at `0x30..0x38` supplies the distance
calculation. The first child gets a local copy of the whole matrix; later
children get a local copy of the vector. Offsets, copy widths and strides are
target facts; `camera_*` names and meanings of the vector/radii are provisional.

The typed draft initially scored `[1,272,42,-54,0]` (54/283 exact). Hoisting
the constant scale and retaining the mask's `lbl_80337924` relocation scored
`[1,256,38,-64,0]` (64/283). Spilling both arguments scored
`[1,254,36,-70,0]` (70/283) and reproduced the target stores at stack
`+0x68` and `+0x6C`. The last candidate is checked in. `hillclimb.py` attempt
2 records the improvement and donor conclusion. Strict verification remains
MISMATCH: frame `0xD0` versus `0xE0`, saves `r19..r31` versus `r14..r31`, and
247 versus 283 instructions. Explicitly carrying `i * 0x68` as a record
offset for the mask and record pointer compiled to the same 247 instructions;
ProDG canonicalized that expression. An explicit 24-byte chunk loop plus
16-byte tail for the matrix copy also compiled to 247 instructions, but fell
to 61 exact and score `[1,270,36,-61,0]`. The remaining copy mismatch is
therefore a register-lifetime and pointer-origin issue, not simply the copy
width. No linked match bytes result from this source.

Verification commands used for each target:

```sh
python3 tools/score_fn.py NAME src/auto/nu3d/ngc/nucamera.c --body work/NAME/try.c
python3 tools/hillclimb.py try NAME --unit auto/nu3d/ngc/nucamera --source src/auto/nu3d/ngc/nucamera.c --body work/NAME/try.c --hypothesis 'Test recovered m2c sweep draft and establish register/frame baseline' --donor-note 'OpenSaga reviewed; no name hit; GC assembly owns offsets and call order'
```

The first-attempt scores above were all mismatches. The strict matches
section records the later acceptance. The unit remains NonMatching;
promotion requires the separate section, relocation and linked-range gates.
