"""Callee declaration repairs: return widths, void/int callees, float parameter blocks."""

from __future__ import annotations

import re

from . import repair
from ._common import call_args


NARROW_RET = re.compile(r"^(?:u8|s8|u16|s16) (?=\w+\()", re.M)


@repair("variant", 40)
def wide_return(body: str) -> str:
    """BODY with a u8/s8/u16/s16 return type widened to s32 when it returns -1. m2c types
    the return from a narrow load (`lbz`), but `li r3, -0x1` with no extension needs an int
    return: `return -1U` from a u8 function emits `li r3, 0xff` (fn_8005A5B0)."""
    if not re.search(r"\breturn -1U?;", body) or not NARROW_RET.search(body):
        return body
    return re.sub(r"\breturn -1U;", "return -1;", NARROW_RET.sub("s32 ", body, count=1))


CALLEE_DECL = re.compile(r"^(?!extern\b)(?:s32|u32|int|s16|u16|s8|u8|void \*|s32 \*|u32 \*) ?(\w+)\(([^()]*)\);$",
                         flags=re.M)


@repair("variant", 50)
def void_callees(body: str) -> str:
    """BODY with each m2c-declared callee whose result is never used declared `void`.
    m2c types every callee `s32`; a call statement that discards an `int` result
    schedules its argument loads differently from a `void` call, so `li r3`/`li r4`
    land on the other side of an independent `lfs` (fn_800A1C70)."""
    out = body
    for m in CALLEE_DECL.finditer(body):
        fn = m[1]
        uses = len(re.findall(rf"\b{fn}\b", body)) - 1
        stmts = len(re.findall(rf"^[ \t]+{fn}\(.*\);$", body, flags=re.M))
        if uses and uses == stmts:
            out = out.replace(m[0], f"void {fn}({m[2]});", 1)
    return out


VOID_DECL = re.compile(r"^void (\w+)\(([^()]*)\);$", flags=re.M)


@repair("variants", 50)
def int_callees(body: str, limit: int = 6) -> list[str]:
    """BODY with one `void` callee declared `s32` instead, for each such callee: the
    reverse of `void_callees`. A call to an `int` function whose result is dropped
    schedules the first argument's `addi` later than a `void` call does (OSCreateThread
    in fn_8009D23C), so neither return type is always right for a discarded result."""
    return [body.replace(m[0], f"s32 {m[1]}({m[2]});", 1) for m in VOID_DECL.finditer(body)][:limit]


PROTO_DECL = re.compile(r"^(?:\w+ \**)+?(\w+)\(([^()]+)\);$", flags=re.M)
FLOAT_PARAM = re.compile(r"^(?:const )?(?:f32|f64|float|double)\b(?! *\*)")


@repair("variants", 60)
def float_blocks(body: str, limit: int = 8) -> list[str]:
    """BODY with prototypes' trailing float parameters moved, as a block, in front of
    earlier integer parameters, and every call reordered to match. Floats and ints use
    separate register files, so m2c cannot see the interleaving and puts the floats
    last; the true position changes argument evaluation order and with it the
    scheduling (fn_8009E6FC: two callees are both `(s32 x4, f32 x4, u32 *, s32)`).
    Each shift is tried on every eligible prototype at once first (sibling callees
    tend to share a layout), then on each prototype alone."""
    protos = []
    for m in PROTO_DECL.finditer(body):
        params = [p.strip() for p in m[2].split(",")]
        kinds = [bool(FLOAT_PARAM.match(p)) for p in params]
        nint = kinds.index(True) if True in kinds else len(kinds)
        if 0 < nint < len(kinds) and all(kinds[nint:]):
            protos.append((m, params, nint))

    def move(text: str, m, params: list[str], nint: int, shift: int) -> str | None:
        pos = nint - shift
        if pos < 0:
            return None
        order = list(range(pos)) + list(range(nint, len(params))) + list(range(pos, nint))
        proto = m[0].replace(m[2], ", ".join(params[i] for i in order))
        text = text.replace(m[0], proto, 1)
        dstart, done = text.index(proto), []
        for call in re.finditer(rf"\b{m[1]}\(", text):
            if dstart <= call.start() < dstart + len(proto):
                continue
            got = call_args(text, call.end() - 1)
            if got and len(got[0]) == len(params):
                done.append((call.end() - 1, got[1], got[0]))
        for start, end, args in reversed(done):
            text = text[:start] + "(" + ", ".join(args[i] for i in order) + ")" + text[end:]
        return text if done else None

    out = []
    longest = max((nint for _, _, nint in protos), default=0)
    if len(protos) > 1:
        for shift in range(1, longest + 1):
            text = body
            for m, params, nint in protos:
                text = move(text, m, params, nint, shift) or text
            if text != body:
                out.append(text)
    for m, params, nint in protos:
        out += [t for t in (move(body, m, params, nint, s) for s in range(1, nint + 1)) if t]
    seen = set()
    return [t for t in out if not (t in seen or seen.add(t))][:limit]
