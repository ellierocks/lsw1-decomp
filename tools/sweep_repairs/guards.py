"""Guard repairs: early-return guards written the positive way round."""

from __future__ import annotations

import re

from . import repair


def split_top(expr: str, op: str) -> list[str]:
    """EXPR split at each OP (`||`, `&&`, `==`, ...) outside parentheses."""
    parts, depth, cur, i = [], 0, 0, 0
    while i < len(expr):
        c = expr[i]
        if c == "(":
            depth += 1
        elif c == ")":
            depth -= 1
        elif depth == 0 and expr.startswith(op, i) and not expr.startswith(op + "=", i) \
                and (op not in ("==", "!=") or expr[i - 1] not in "<>=!"):
            parts.append(expr[cur:i].strip())
            i += len(op)
            cur = i
            continue
        i += 1
    parts.append(expr[cur:].strip())
    return parts


def unwrap(expr: str) -> str:
    """EXPR without parentheses that enclose all of it."""
    while expr.startswith("("):
        depth = 0
        for i, c in enumerate(expr):
            depth += (c == "(") - (c == ")")
            if depth == 0:
                break
        if i != len(expr) - 1:
            return expr
        expr = expr[1:-1].strip()
    return expr


def negate(expr: str) -> str:
    """The C negation of EXPR without NaN-unsafe flips: `!(e)` gives e, a top-level `==`
    or `!=` flips, anything else is wrapped in `!(...)`."""
    e = unwrap(expr)
    if e.startswith("!(") and unwrap(e[1:]) != e[1:]:
        return unwrap(e[1:])  # `!(a > b)`: only when the parentheses span the rest
    for op, inv in (("!=", "=="), ("==", "!=")):
        sides = split_top(e, op)
        if len(sides) == 2 and len(split_top(e, "||")) == 1 and len(split_top(e, "&&")) == 1:
            return f"{sides[0]} {inv} {sides[1]}"
    return f"!({e})"


GUARD_RETURN = re.compile(r"^(?P<ind>[ \t]*)if \((?P<cond>.*)\) \{\n(?P=ind)    return (?P<a>[^;\n]+);\n"
                          r"(?P=ind)\}\n(?P=ind)return (?P<b>[^;\n]+);\n", re.M)


@repair("scored", 60)
def positive_guards(body: str) -> str:
    """BODY with each `if (A || B || !(C)) { return a; } return b;` written the other
    way round as `if (!A && !B && C) { return b; } return a;`. m2c inverts the branch it
    sees, so a source `if (x == 1 && y > z) return 1; return 0;` drafts as the early
    `return 0` guard, and GCC then lays out the comparisons and `li r3` differently."""
    def flip(m: re.Match) -> str:
        terms = split_top(unwrap(m["cond"]), "||")
        if len(terms) == 1 and len(split_top(terms[0], "&&")) > 1:
            return m[0]
        cond = " && ".join(f"({t})" if len(split_top(t, "&&")) > 1 or len(split_top(t, "||")) > 1
                           else t for t in (negate(t) for t in terms))
        ind = m["ind"]
        return (f"{ind}if ({cond}) {{\n{ind}    return {m['b']};\n{ind}}}\n"
                f"{ind}return {m['a']};\n")
    return GUARD_RETURN.sub(flip, body)
