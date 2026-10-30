# Decompile fn_80018E64

## Task

Write a matching C function for `fn_80018E64` in the LEGO Star Wars: The Video Game
(GameCube) decompilation project.

- **Symbol:** `fn_80018E64`
- **Address:** `0x80018E64`
- **Size:** `0x34` (52 bytes)
- **Section:** `text`
- **Source module:** `nuanim`

## Constraints

- Match the original Metrowerks CodeWarrior for GameCube (PPC 750CL) compiler output exactly.
- Use the provided struct definitions from `context.md`.
- Name all called functions exactly as given (keep `fn_ADDR` if unnamed).
- The stack frame and prologue/epilogue must match exactly.
- Preserve instruction selection — the compiler was not optimizing and
  was fairly literal with C control flow.
- Register assignment in the original binary was determined by the
  Metrowerks calling convention (r3–r10 for parameters, r3 for return,
  r13–r31 are callee-saved).
- Use `f32` for `float`, `s32` for `int` / `signed int`, `u32` for
  `unsigned int`, `s16` for `short`, `u16` for `unsigned short`, `s8`
  for `char` (signed), `u8` for `unsigned char`.
- Do **not** add comments explaining what the code does.
- Match control flow exactly — every branch, every loop.



