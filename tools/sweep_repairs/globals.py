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


def _locals(body: str) -> set[str]:
    names = {m.group(1) for m in ANY_DECL.finditer(body)}
    for m in PARAMS.finditer(body):
        names |= set(re.findall(r"(\w+)\s*(?:\[[^\]]*\])?\s*(?:,|$)", m["params"]))
    return names


def _cached(body: str) -> list[tuple[re.Match, str, re.Match]]:
    """(declaration, value, assignment) for each temp assigned exactly once from a bare
    global, optionally cast, and never written or address-taken otherwise."""
    local = _locals(body)
    out = []
    for decl in LOCAL_DECL.finditer(body):
        v = re.escape(decl["v"])
        sets = list(re.finditer(rf"^[ \t]+{v} = (?P<rhs>(?:\([\w ]+\**\) ?)?(?P<g>[A-Za-z_]\w*));[ \t]*\n",
                                body, re.M))
        writes = re.findall(rf"(?<![\w.>])&{v}\b|\b{v}\s*(?:[-+*/%&|^]|<<|>>)?=(?!=)|\+\+{v}\b|--{v}\b|"
                            rf"\b{v}\s*(?:\+\+|--)", body)
        if len(sets) != 1 or len(writes) != 1 or sets[0]["g"] in local or sets[0]["g"] in KEYWORDS:
            continue
        g = sets[0]["g"]
        if re.search(rf"\b{re.escape(g)}\s*\(", body):  # a function, not data
            continue
        out.append((decl, f"(({decl['type'].strip()}) {sets[0]['rhs']})", sets[0]))
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
