"""Global repairs: a global the draft caches in a local where retail reloads it."""

from __future__ import annotations

import re

from . import repair

# m2c reads a global once into a temp (`temp_r9 = lbl_80408DF0;`) and reuses the temp, so
# GCC keeps it in a callee-saved register across calls; retail reloads the global after
# each call (`lwz r9, lbl@sda21(r0)` again): the source named the global at every use.
LOCAL_DECL = re.compile(r"^[ \t]+(?P<type>[\w ]+?\s*\**)\s*(?P<v>\b(?:temp|var)_\w+);[ \t]*$", re.M)
ANY_DECL = re.compile(r"^[ \t]+(?:(?:const|unsigned|signed|struct|volatile|static) )*\w+[ \t*]+(\w+)(?:\[[^\]\n]*\])?;[ \t]*$", re.M)
PARAMS = re.compile(r"^[^\s#][^;{}\n]*?\b\w+\s*\((?P<params>[^;{}\n]*)\)\s*\{", re.M)
CALL = re.compile(r"\b\w+\s*\(")
KEYWORDS = {"if", "while", "for", "switch", "return", "sizeof", "do"}


# What LOCAL_DECL needs up to its `;`, checked in linear time: the word before the `;` is
# the temp, and everything before it is indentation then [\w\s*] (type, spaces, stars).
DECL_NAME = re.compile(r"(?<!\w)(?:temp|var)_\w+\Z")
DECL_HEAD = re.compile(r"[ \t][\w\s*]+")


def _decls(body: str):
    """LOCAL_DECL.finditer(BODY) without its cost: the pattern's overlapping whitespace runs
    make every failed attempt on a deeply indented line very slow, so it runs only at line
    starts that pass the necessary DECL_NAME/DECL_HEAD check. A match holds no `;` before
    the one ending it, so that `;` is the first after the line start."""
    start = 0
    while start < len(body):
        semi = body.find(";", start)
        if semi < 0:
            return
        seg = body[start:semi]
        name = DECL_NAME.search(seg)
        if name and DECL_HEAD.fullmatch(seg, 0, name.start()):
            m = LOCAL_DECL.match(body, start)
            if m:
                yield m
                start = m.end()  # mid-line: resume at the next line start
        nl = body.find("\n", start)
        if nl < 0:
            return
        start = nl + 1


def _locals(body: str) -> set[str]:
    names = {m.group(1) for m in ANY_DECL.finditer(body)}
    for m in PARAMS.finditer(body):
        names |= set(re.findall(r"(\w+)\s*(?:\[[^\]]*\])?\s*(?:,|$)", m["params"]))
    return names


TEMP = r"(?:temp|var)_\w+"
# One scan each for every temp's assignments from a bare global and its writes, instead of
# two whole-body scans per declared temp (quadratic on large drafts).
SET = re.compile(rf"^[ \t]+(?P<v>{TEMP}) = (?P<rhs>(?:\([\w ]+\**\) ?)?(?P<g>[A-Za-z_]\w*));[ \t]*\n", re.M)
WRITE = re.compile(rf"(?<![\w.>])&(?P<a>{TEMP})\b|\b(?P<b>{TEMP})\s*(?:[-+*/%&|^]|<<|>>)?=(?!=)|"
                   rf"\+\+(?P<c>{TEMP})\b|--(?P<d>{TEMP})\b|\b(?P<e>{TEMP})\s*(?:\+\+|--)")


def _cached(body: str) -> list[tuple[re.Match, str, re.Match]]:
    """(declaration, value, assignment) for each temp assigned exactly once from a bare
    global, optionally cast, and never written or address-taken otherwise."""
    local = _locals(body)
    sets: dict[str, list[re.Match]] = {}
    for m in SET.finditer(body):
        sets.setdefault(m["v"], []).append(m)
    writes: dict[str, int] = {}
    for m in WRITE.finditer(body):
        v = m["a"] or m["b"] or m["c"] or m["d"] or m["e"]
        writes[v] = writes.get(v, 0) + 1
    out = []
    for decl in _decls(body):
        found = sets.get(decl["v"], [])
        if len(found) != 1 or writes.get(decl["v"], 0) != 1 or found[0]["g"] in local or found[0]["g"] in KEYWORDS:
            continue
        g = found[0]["g"]
        if re.search(rf"\b{re.escape(g)}\s*\(", body):  # a function, not data
            continue
        out.append((decl, f"(({decl['type'].strip()}) {found[0]['rhs']})", found[0]))
    return out


def _inline(body: str, after_call: bool) -> str:
    for decl, value, assign in sorted(_cached(body), key=lambda c: -c[0].start()):
        v = decl["v"]
        if body.count(assign[0]) != 1:
            continue
        head, tail = body.split(assign[0], 1)
        if after_call:  # keep the temp up to the first call after the assignment
            call = next((m for m in CALL.finditer(tail) if m[0].rstrip("( ") not in KEYWORDS), None)
            if call is None:
                continue
            cut = tail.find("\n", call.end())
            cut = len(tail) if cut < 0 else cut + 1
            later = re.sub(rf"\b{re.escape(v)}\b", lambda _: value, tail[cut:])
            if later == tail[cut:]:
                continue
            body = head + assign[0] + tail[:cut] + later
            continue
        tail = re.sub(rf"\b{re.escape(v)}\b", lambda _: value, tail)
        head = re.sub(rf"\b{re.escape(v)}\b", lambda _: value, head.replace(decl[0] + "\n", "", 1))
        body = head + tail
    return body


@repair("final", 10)
def inline_globals(body: str, name="", asm="", unit="") -> str:
    """BODY with each local that m2c assigned once from a bare global (`temp_r9 = lbl_X;`,
    optionally cast) replaced at every use by the global read through the local's type,
    and the local's declaration and assignment dropped (fn_800D4DAC)."""
    return _inline(body, False)


@repair("final", 20)
def inline_globals_after_calls(body: str, name="", asm="", unit="") -> str:
    """inline_globals for the uses after the first call that follows the assignment only:
    retail keeps the cached value until a call clobbers its volatile register."""
    return _inline(body, True)
