# Matching Notes

Compiler quirks, naming conventions, and diff-fixing techniques discovered
while matching LEGO Star Wars: The Video Game (GameCube) functions.

## Compiler: Metrowerks CodeWarrior for GameCube

### Versions tried
- `GC/1.3.2` (build.ninja default) — used for all tests below
- `GC/1.2.5n` — identical output
- `GC/1.2.5` — identical output
- `GC/1.3.2r` — identical output

All produce identical instruction selection for fn_80023C70.

### Key flags (from configure.py)
```
-nodefaults -proc gekko -align powerpc -enum int -fp hardware
-Cpp_exceptions off -O4,p -inline auto -pragma "cats off"
-pragma "warn_notinlined off" -maxerrors 1 -nosyspath -RTTI off
-fp_contract on -str reuse -multibyte
-i src -i build/GL5E4F/include
```

### Register allocation

**Biggest matching challenge.** Register allocation is influenced by:
- Surrounding functions in the same compilation unit (MWCC allocates
  registers globally per-file, not per-function)
- Variable declaration order (weak influence)
- Optimization level

For fn_80023C70:
- Original: `r9` (src ptr), `r11` (counter), `r3` (dst ptr)
- Compiled standalone: `r4` (src ptr), `r5` (counter), `r3` (dst ptr)
- Compiled with immediate local neighbors
  (`fn_80023B50`, `fn_80023D80`, `fn_80023D8C`, `fn_80023DB0`,
  `fn_800240EC`, `fn_80024124`): still `r4`/`r5`

The instruction encodings are **identical except for the register number
bits** — same opcode, same immediate/offset values.

To fix: compile the function alongside its neighbors in the same `.c` file.
The register allocator sees the full-file live range and may choose r9/r11
over r4/r5.

In this case, the immediate address neighbors were not enough. The original
translation unit is likely larger than the `0x80023B50`-`0x80024124` local
cluster that was tested.

### `subic.` vs `addic.`

Both assemble to the same encoding (`34 xx FF E8`). The disassembly
mnemonic depends on the tool — not a real difference.

## Naming

### Pattern: `fn_ADDR` → `NuVerbNoun`

Apply verified names via `ls1_rename.py`. Priority order:
1. CrashWOC decomp (identical engine)
2. LSW1 Mac Demo (closest lineage)
3. LSW2 Mac
4. Struct offset analysis

### Struct evidence in ASM

| Pattern | Struct | Matching technique |
|---------|--------|-------------------|
| `lwz/stw` on same reg with offsets 0,4,8,12 | `nuanimcurve_s` | Copy loop with 0x18 stride |
| `lha` + `lfs` with offsets 0,4,8,12 | `nuanimkey_s` | Float lerp function |
| Halfword ops at +4/+6 | `nuanimdata2_s` | Short field access |
| Ptr deref with 5+ offsets | `nudathdr_s` | Fat struct access |

## Matching workflow (C)

1. **Generate task pack**: `ls1_task_pack.py <fn> --out-dir tasks`
2. **Write C**: in `tasks/<Fn>/source.c` or `src/<module>/<fn>.c`
3. **Compile**: `wibo mwcceppc.exe <flags> -c <file>.c -o <file>.o`
4. **Fixup**: `dtk elf fixup <file>.o <file>.o`
5. **Diff**: `objdiff-cli diff -1 <compiled>.o -2 <base>.o <symbol>`
6. **Iterate**: fix control flow first, then register allocation

### Quick manual compile
```sh
build/tools/wibo build/compilers/GC/1.3.2/mwcceppc.exe \
  -nodefaults -proc gekko -align powerpc -enum int -fp hardware \
  -Cpp_exceptions off -O4,p -inline auto -pragma "cats off" \
  -pragma "warn_notinlined off" -maxerrors 1 -nosyspath -RTTI off \
  -fp_contract on -str reuse -multibyte \
  -i src -i build/GL5E4F/include \
  -c src/nu3dx/<file>.c -o /tmp/<fn>.o
build/tools/dtk elf fixup /tmp/<fn>.o /tmp/<fn>.o
```

## Common issues

### Stack frame mismatch
- `stwu r1, -N(r1)` / `addi r1, r1, N` — check N matches
- MWCC always pairs these; any mismatch means wrong frame size

### Data symbol refs
- Original uses `lis r9, lbl_ADDR@ha; addi r9, r9, lbl_ADDR@l`
- Compiled with `-i src` (writes `lis rD, addr; ...`)
- The compiled output matches the original's encoding — the same
  address is computed.

### Object boundary recovery
- `lbl_80313268` is currently typed in `symbols.txt` as a `0x20` BSS object,
  but the camera-state cluster proves that boundary is too small:
  - `fn_80023C70` copies `0x68` bytes from `lbl_80313268`
  - `fn_80023CD0` allocates `0x68` bytes and initializes fields through `+0x64`
  - `fn_80023F80` reads floats from `lbl_80313268 + 0x48` and `+0x4C`
- Donor evidence in `docs/symbol_donors/mac_data_to_gc_verified.md`
  links the start of this region to `DoorExitCameraSplineName`.
- Working conclusion: `lbl_80313268` is the start of a larger
  camera-related state struct, with `DoorExitCameraSplineName` as an
  offset-0 field or prefix, not the whole object.
- Stronger boundary conclusion:
  - The live object spans `0x80313268..0x803132CF` (`0x68` bytes total)
  - The next independently referenced object begins at `0x803132D0`
  - Therefore `lbl_80313288`, `lbl_80313298`, `lbl_803132B0`, and
    `lbl_803132B4` are field-level artifacts inside the same larger state block
- Recovered shared type:
  - `struct nucamstate_s` in `src/nu3dx/nu3dx.h`
  - Layout:
    - `0x00..0x3F` `numtx_s mtx`
    - `0x40` `fov`
    - `0x44` `aspect`
    - `0x48` `nearclip`
    - `0x4C` `farclip`
    - `0x50` `nearclip_alt`
    - `0x54` `farclip_alt`
    - `0x58` `unk58`
    - `0x5C..0x67` `nuvec_s pos`
- Evidence for the field split:
  - `fn_8001C20C` passes `0x40/0x44/0x48/0x4C` as four scalar camera/projection
    parameters to `fn_800234A8`
  - `fn_8001C20C` compares `0x50/0x54` against zero and falls back to
    `0x48/0x4C`, indicating alternate clip-range fields
  - `fn_8001C20C` passes `state + 0x5C` by pointer into `fn_8000EA74`,
    so the tail is a 3-float vector, not padding

### Object file differences (dtk elf fixup)
- MWCC outputs don't have `.nofrag` section — `dtk elf fixup` is
  needed before comparison (adds missing sections).

### First nuanim object split candidate
- Added the first real `splits.txt` object candidate:
  - `nu3dx/nuanim.c`
  - `.text start:0x80016A98 end:0x8001752C`
- Evidence:
  - existing donor proposal in `docs/symbol_donors/proposed_splits.md`
  - matching `proposed_splits.txt` entry `nu2/NuAnimation.c:0x80016A98`
  - local symbol neighborhood:
    - `fn_80016A98`
    - `fn_80016C84`
    - `NuAnimCurve2CalcVal`
  - subsystem continuity from `ls1_match_plan.py nuanim`
- This is a source-file boundary recovery step, not a claim that the full
  downstream `0x8001752C+` range belongs to the same object.
- Current blocker:
  - `dtk dol split` still panics before regenerating `build/GL5E4F/config.json`
  - because of that, `objdiff.json` cannot yet reflect the new split automatically

### Safe nuanim rename
- Renamed `fn_80018DA8` -> `NuAnimDataLoadBuff`
- Evidence:
  - direct reference to `str_NuAnimDataLoadBuff_OldFormatExpectedNew`
  - the function checks the animation data header/version and dispatches into
    the fixup path
  - this matches the donor API name `NuAnimDataLoadBuff`
