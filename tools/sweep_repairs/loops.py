"""Loop repairs: guard compares, index loops, countdown loops."""

from __future__ import annotations

import re

from . import repair


GUARD = re.compile(r"\bif \((?P<n>\(s32\) \w+(?:\[0\])?|\w+(?:\[0\])?|\(s32\) \(\*\([^;{}]*?\)\)|\(\*\([^;{}]*?\)\)) > 0\) \{")


@repair("scored", 30)
def loop_guard(body: str) -> str:
    """BODY with m2c's loop guard `if (N > 0)` written as `if (i < N)` when the counter i,
    zeroed before it, is compared with the same N at the loop's end. GCC keeps a `for`
    loop's entry test as `cmpw i, N` with i still 0, which a constant compare cannot
    reproduce (fn_80064B40, fn_80063D9C)."""
    def repl(m):
        n = m["n"]
        bare = re.escape(n[6:] if n.startswith("(s32) ") else n)
        cmp = re.search(rf"\b(\w+) (?:>=|<) (?:\(s32\) )?{bare}\)", body[m.end():])
        if not cmp or not re.search(rf"^\s*{cmp[1]} = 0;$", body[:m.start()], flags=re.M):
            return m[0]
        return f"if ({cmp[1]} < {n}) {{"
    return GUARD.sub(repl, body)


LOOP_END = re.compile(r"^(?P<ind>[ \t]*)\} while \((?P<cond>.*)\);$")
PTR_BOUND = re.compile(r"(\w+) <= \(void \*\) \(\(\(s8 \*\)(.+)\) \+ (0x[0-9A-Fa-f]+|\d+)\)")


@repair("scored", 40)
def index_loops(body: str) -> str:
    """BODY with each m2c pointer loop rewritten over an index. m2c prints a loop over an
    array of N-byte records as `p = B; do { ...; p += N; } while (p <= (void *) ((s8 *)B + K));`,
    but GCC formed that pointer from `&B[i]` and compares the eliminated `s32 i` with a signed
    `cmpw` where the draft's pointer compare gives `cmplw` (fn_8014D01C). Each such pointer, and
    each pointer stepped beside a `c += 1` counter that starts at 0 (fn_8014CFA4), is
    recomputed from the index at the top of the body; a pointer bound becomes `i <= K / N`."""
    lines = body.split("\n")
    for end in range(len(lines) - 1, -1, -1):
        new = _index_loop(lines, end)
        if new is not None:
            return index_loops("\n".join(new))  # one loop per pass: line numbers shift
    return body


def _index_loop(lines: list[str], end: int) -> list[str] | None:
    m = LOOP_END.match(lines[end])
    if not m:
        return None
    ind, inner = m["ind"], m["ind"] + "    "
    starts = [i for i in range(end) if lines[i] == f"{ind}do {{"]
    if not starts:
        return None
    start = starts[-1]
    loop = lines[start + 1:end]
    if any(ln.strip() and not ln.startswith(inner) for ln in loop):
        return None  # not the matching `do`
    found: dict[str, list[tuple[int, int]]] = {}
    for k, ln in enumerate(loop):
        sm = re.fullmatch(rf"{inner}(\w+) \+= (0x[0-9A-Fa-f]+|\d+);", ln)
        if sm:
            found.setdefault(sm[1], []).append((k, int(sm[2], 0)))
    steps = {v: st[0] for v, st in found.items() if len(st) == 1}

    def init_of(v: str) -> tuple[int, str] | None:
        for i in range(start - 1, -1, -1):
            if re.search(rf"\b{v}\b", lines[i]):
                im = re.fullmatch(rf"([ \t]*){v} = ([^;]+);", lines[i])
                return (i, im[2]) if im and ind.startswith(im[1]) else None
        return None

    def assigned(v: str, skip: int) -> bool:
        return any(re.search(rf"\b{v}\s*(?:[-+*/|&^]?=(?!=)|\+\+|--)|(?:\+\+|--){v}\b", ln)
                   for k, ln in enumerate(loop) if k != skip)

    after = "\n".join(lines[end + 1:])
    counter = new_index = bound = None
    cm = PTR_BOUND.fullmatch(m["cond"])
    if cm and cm[1] in steps:
        init, n = init_of(cm[1]), steps[cm[1]][1]
        if init and init[1].strip() in (cm[2], f"(void *) {cm[2]}") and int(cm[3], 0) % n == 0:
            new_index, bound = f"i_{cm[1]}", int(cm[3], 0) // n
    else:
        for v, (k, n) in steps.items():
            init = init_of(v)
            if n == 1 and init and init[1] == "0" and re.match(rf"{v} [<>]=? ", m["cond"]) \
                    and not assigned(v, k):
                counter = v
                break
    if not (new_index or counter):
        return None
    ptrs = []
    for v, (k, n) in steps.items():
        if v == counter or (n == 1 and v != (cm[1] if cm else None)):
            continue
        init = init_of(v)
        if init and not re.search(rf"\b{v}\b", after) and not assigned(v, k):
            ptrs.append((v, k, n, init))
    if new_index and not any(v == cm[1] for v, *_ in ptrs):
        return None
    if not ptrs:
        return None
    idx = new_index or counter
    drop = {k for _, k, _, _ in ptrs}
    body = [f"{inner}{v} = (void *) ((s8 *) ({init[1]}) + {idx} * {n});" for v, _, n, init in ptrs]
    body += [ln for k, ln in enumerate(loop) if k not in drop]
    inits = sorted({init[0] for *_, init in ptrs})
    out = list(lines)
    if new_index:
        body.append(f"{inner}{idx} += 1;")
        out[end] = f"{ind}}} while ({idx} <= {bound});"
    out[start + 1:end] = body
    if new_index:
        out[inits[0]] = re.sub(r"\S.*", f"{idx} = 0;", out[inits[0]])
        inits = inits[1:]
    for i in reversed(inits):
        del out[i]
    if new_index:
        top = next((i for i, ln in enumerate(out) if ln == "{" or re.match(r"^\w.*\) \{$", ln)), None)
        if top is None:
            return None
        out.insert(top + 1, f"    s32 {idx};")
    return out


@repair("scored", 50)
def countdown_loops(body: str) -> str:
    """BODY with each m2c countdown loop written as the `for` loop GCC compiled. m2c prints
    `for (i = 0; i < n; i++) f(a[i]);` as `if (n > 0) { c = n; o = 0; do { f(*(o + a));
    o += 4; c -= 1; } while (c != 0); }`; ProDG schedules the source form differently
    (PodraceLoadLapSettings hoisted a string address out of the draft's loop). Each stepped
    variable is recomputed from the index at the top of the body."""
    lines = body.split("\n")
    for end in range(len(lines) - 1, -1, -1):
        new = _countdown_loop(lines, end)
        if new is not None:
            return countdown_loops("\n".join(new))
    return body


def _countdown_loop(lines: list[str], end: int) -> list[str] | None:
    m = re.fullmatch(r"([ \t]*)\} while \((\w+) != 0\);", lines[end])
    if not m or end + 1 >= len(lines):
        return None
    ind, cnt = m[1], m[2]
    inner, g = ind + "    ", ind[:-4]
    if not ind.endswith("    ") or lines[end + 1] != f"{g}}}":
        return None
    starts = [i for i in range(end) if lines[i] == f"{ind}do {{"]
    if not starts:
        return None
    start = starts[-1]
    loop = lines[start + 1:end]
    if any(ln.strip() and not ln.startswith(inner) for ln in loop):
        return None
    if loop[-1] != f"{inner}{cnt} -= 1;":
        return None
    head = start
    while head > 0 and re.fullmatch(rf"{ind}\w+ = [^;]+;", lines[head - 1]):
        head -= 1
    if head < 1:
        return None
    gm = re.fullmatch(rf"{g}if \((.+) > 0\) \{{", lines[head - 1])
    if not gm:
        return None
    n = gm[1]
    inits = {}
    for ln in lines[head:start]:
        im = re.fullmatch(rf"{ind}(\w+) = ([^;]+);", ln)
        inits[im[1]] = im[2]
    if inits.get(cnt, "").strip("() ") != n.strip("() ") and inits.get(cnt) != n:
        return None
    steps = {}
    rest = []
    for ln in loop[:-1]:
        sm = re.fullmatch(rf"{inner}(\w+) \+= (0x[0-9A-Fa-f]+|\d+);", ln)
        if sm and sm[1] in inits and sm[1] not in steps:
            steps[sm[1]] = int(sm[2], 0)
        else:
            rest.append(ln)
    text = "\n".join(lines)
    after = "\n".join(lines[end + 2:])
    for v in [cnt, *steps]:
        if re.search(rf"\b{v}\b", after):
            return None
    if re.search(rf"\b{cnt}\b", "\n".join(rest)) or set(inits) - set(steps) - {cnt}:
        return None
    for v in steps:
        if re.search(rf"\b{v}\s*(?:[-+*/|&^]?=(?!=)|\+\+|--)", "\n".join(rest)):
            return None
    new = [f"{g}for ({cnt} = 0; {cnt} < {n}; {cnt}++) {{"]
    for v, st in steps.items():
        ptr = re.search(rf"^[ \t]+[\w ]*\*\s*{v};", text, flags=re.M)
        x = inits[v]
        if ptr:
            new.append(f"{ind}{v} = (void *) ((s8 *) ({x}) + {cnt} * {st});")
        else:
            new.append(f"{ind}{v} = {cnt} * {st};" if x == "0" else f"{ind}{v} = ({x}) + {cnt} * {st};")
    new += [ln[4:] for ln in rest]
    new.append(f"{g}}}")
    return lines[:head - 1] + new + lines[end + 2:]
