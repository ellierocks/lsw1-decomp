"""Switch repairs: small switches as if/else chains, pivot cases."""

from __future__ import annotations

import re

from . import repair


SWITCH_LINE = re.compile(r"^(?P<ind>[ \t]*)switch \((?P<x>.+)\) \{[ \t]*(?:/\*.*\*/)?$")
CASE = re.compile(r"^(?P<ind>[ \t]*)(?:case (?P<v>[^:]+)|default):$")
TERMINAL = re.compile(r"^[ \t]*(?:return\b[^;]*|goto \w+|break);$")


@repair("scored", 70)
def switch_chain(body: str, max_cases: int = 4) -> str:
    """BODY with each m2c `switch` of at most MAX_CASES case labels written as an
    `if (x == a) {...} else if (x == b) {...} else {...}` chain in case order. GCC 2.95
    compiles a two- or three-case switch as a compare chain whose blocks follow the
    switch's own layout (cases after the compares); retail code from an if/else chain
    places each body right after its compare (fn_800D2E44: 123/132 as a switch, 129
    as a chain). A switch is left alone when a case falls through, a case body holds
    another `break` or a nested case label, or the selector contains a call."""
    lines = body.split("\n")
    out, i = [], 0
    while i < len(lines):
        m = SWITCH_LINE.match(lines[i])
        conv = _switch_to_chain(lines, i, m, max_cases) if m else None
        if conv is None:
            out.append(lines[i])
            i += 1
            continue
        chain, i = conv
        out.extend(chain)
    return "\n".join(out)


def _switch_to_chain(lines: list[str], start: int, m: re.Match, max_cases: int):
    ind, x = m["ind"], m["x"]
    if re.search(r"\w\s*\(", x):
        return None
    end = next((j for j in range(start + 1, len(lines)) if lines[j] == f"{ind}}}"), None)
    if end is None:
        return None
    arms: list[tuple[list[str], list[str]]] = []  # (case values, body lines); [] values = default
    for ln in lines[start + 1:end]:
        c = CASE.match(ln)
        if c and c["ind"] == ind:
            if arms and not arms[-1][1]:
                arms[-1][0].append(c["v"])  # stacked labels share one body
            else:
                arms.append(([c["v"]], []))
            continue
        if c or not arms:
            return None  # nested case label, or code before the first case
        arms[-1][1].append(ln)
    labels = sum(len(vals) for vals, _ in arms)
    if not arms or labels > max_cases or sum(None in vals for vals, _ in arms) > 1:
        return None
    for k, (vals, blines) in enumerate(arms):
        code = [ln for ln in blines if ln.strip() and not re.match(r"^\w+:$", ln.strip())]
        if None in vals and len(vals) > 1:
            return None
        if k < len(arms) - 1 and not (code and TERMINAL.match(code[-1])):
            return None  # falls through
        if code and code[-1].strip() == "break;":
            blines = blines[:len(blines) - 1 - blines[::-1].index(code[-1])] + \
                blines[len(blines) - blines[::-1].index(code[-1]):]
        if any(ln.strip() == "break;" for ln in blines):
            return None
        arms[k] = (vals, blines)
    default = next((a for a in arms if None in a[0]), None)
    if default is not None and default is not arms[-1]:
        return None
    chain, first = [], True
    for vals, blines in arms:
        if None in vals:
            chain.append(f"{ind}}} else {{")
        else:
            cond = " || ".join(f"{x} == {v}" for v in vals)
            chain.append(f"{ind}if ({cond}) {{" if first else f"{ind}}} else if ({cond}) {{")
        first = False
        chain.extend(blines)
    chain.append(f"{ind}}}")
    return chain, end + 1


SWITCH = re.compile(r"^(?P<ind>[ \t]*)switch \(.*\) \{\n", re.M)


@repair("variants", 20)
def switch_pivots(body: str) -> list[str]:
    """BODY with an empty `case max+1:` (then `case min-1:`) added to each switch of three
    or more integer cases. GCC 2.95 balances its compare tree on the case count, so a
    source case with an empty body (dropped from the code) moves the pivot."""
    out = []
    for m in SWITCH.finditer(body):
        ind = m["ind"]
        close = re.compile(rf"^{re.escape(ind)}\}}\n?", re.M).search(body, m.end())
        if not close:
            continue
        labels = [int(v, 0) for v in re.findall(rf"^{re.escape(ind)}(?:    )?case (-?(?:0x[0-9A-Fa-f]+|\d+)):",
                                                 body[m.end():close.start()], flags=re.M)]
        if len(labels) < 3:
            continue
        case_ind = re.search(r"^([ \t]*)case ", body[m.end():close.start()], flags=re.M)[1]
        for k in (max(labels) + 1, min(labels) - 1):
            extra = f"{case_ind}case {k}:\n{case_ind}    break;\n"
            out.append(body[:close.start()] + extra + body[close.start():])
    return out
