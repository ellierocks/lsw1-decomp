# CrashWOC donor-guided code-match queue

One-to-one LSW1/Crash body matches with a single safe Nu2 C definition in the local CrashWOC checkout. `exact` rows are byte-identical (mostly stubs); `norm` rows match after normalization and are the substantial candidates. Use these as source-port candidates, not as proof that the first C compile will match.

Source checkout: `/tmp/crashwoc-decomp`

| LSW1 target | Size | Match | Crash function | Crash source |
|---|---:|---|---|---|
| `fn_8000E918` @ `0x8000e918` | `0x00a4` | `exact` | `NuVec4MtxTransformVU0` @ `0x800be7c0` | `numath/nu_asm.c:16` |
| `fn_80069900` @ `0x80069900` | `0x008c` | `norm` | `edanimLookupSpecial` @ `0x8008468c` | `gamelib/edanim.c:477` |
| `fn_80069900` @ `0x80069900` | `0x008c` | `norm` | `edobjLookupInstance` @ `0x800883ec` | `gamelib/edobj.c:913` |
| `fn_8000CD08` @ `0x8000cd08` | `0x0060` | `norm` | `NuVecDist` @ `0x800c1968` | `numath/nuvec.c:145` |
| `fn_8000CD08` @ `0x8000cd08` | `0x0060` | `norm` | `NuVecDistSqr` @ `0x800c19c8` | `numath/nuvec.c:160` |
| `fn_8009D550` @ `0x8009d550` | `0x002c` | `norm` | `malloc_x` @ `0x800be71c` | `nucore/numem.c:73` |
| `fn_800099D4` @ `0x800099d4` | `0x0024` | `norm` | `NuSoundPlay` @ `0x800c23f0` | `nusound/nusound.c:129` |
| `fn_800099D4` @ `0x800099d4` | `0x0024` | `norm` | `edppRegisterPointerToGameCharLocation` @ `0x80088478` | `gamelib/edptl.c:46` |
