#!/usr/bin/env python3
"""Review a decomp-permuter output against its base.c (read-only).

    python3 tools/permute_review.py NAME                     # best (lowest score) output
    python3 tools/permute_review.py NAME output-160-1        # a named output of work/permute/NAME
    python3 tools/permute_review.py NAME path/to/output-dir

Prints, for the output's `source.c` against `base.c`:

  1. every top-level declaration that differs: prototypes, extern types, struct/typedef
     changes, other function definitions, and the target's own signature. A match can come
     from a changed prototype with an identical body (PR #65, `fn_80095558` returning
     void), so read this part first.
  2. a unified diff of just the target function body, after normalising whitespace, layout
     and redundant parentheses (the permuter reprints the whole file).
  3. suspicious changes: arithmetic on `void *`, removed `volatile`, and a local that is
     read before any assignment (a text-order heuristic, so treat it as a prompt to look).

Port a finding into the unit source by hand and accept it only with
`verify_fn.py NAME --build --strict-symbols`.
"""
from __future__ import annotations

import argparse
import collections
import difflib
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
PERMUTE_DIR = ROOT / "work" / "permute"

TOKEN = re.compile(r"""
    "(?:\\.|[^"\\])*" | '(?:\\.|[^'\\])*'            # strings, chars
  | 0[xX][0-9a-fA-F]+[uUlL]* | \d+\.?\d*(?:[eE][+-]?\d+)?[fFuUlL]* | \.\d+(?:[eE][+-]?\d+)?[fF]?
  | [A-Za-z_]\w*
  | <<= | >>= | \.\.\. | -> | \+\+ | -- | << | >> | <= | >= | == | != | && | \|\|
  | [-+*/%&|^]= | \S
""", re.X)
COMMENT = re.compile(r"/\*.*?\*/|//[^\n]*", re.S)
PREPROC = re.compile(r"^[ \t]*#(?:[^\n\\]|\\.)*", re.M)
STORAGE = {"static", "inline", "__inline", "__inline__", "extern", "register", "const", "volatile"}
KEYWORDS = {"if", "while", "for", "switch", "return", "sizeof", "case", "else", "do", "goto"}
NOT_DECL = KEYWORDS | {"break", "continue", "default"}
IDENT = re.compile(r"[A-Za-z_]\w*$")


def tokenize(text: str) -> list[str]:
    text = COMMENT.sub(" ", text)
    text = PREPROC.sub(" ", text)
    return TOKEN.findall(text)


def is_ident(tok: str) -> bool:
    return bool(IDENT.match(tok))


ASSIGN = {"=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>="}
ALWAYS_BINARY = {"/", "%", "<<", ">>", "<", ">", "<=", ">=", "==", "!=", "&&", "||", "^", "|", "?", ":", ","} | ASSIGN
AMBIGUOUS = {"*", "&", "-", "+"}
POSTFIX = {".", "->", "[", "(", "++", "--"}
CAST_FOLLOWERS = {"*", "&", "-", "+", "!", "~", "(", "sizeof", "++", "--"}
DELIMS = ASSIGN | {",", "return", ";", "{", "}", "[", "(", "?", ":"}
CLOSERS = {";", ",", ")", "]", "}", ":"}
TYPE_WORDS = {"unsigned", "signed", "struct", "union", "enum", "const", "volatile", "void", "char", "short", "int",
              "long", "float", "double"}


def operand_end(tok: str) -> bool:
    return (is_ident(tok) and tok not in KEYWORDS) or tok[0].isdigit() or tok[0] in "\"'" or tok in (")", "]")


def has_top_level_binary(content: list[str]) -> bool:
    depth = 0
    for k, t in enumerate(content):
        if t in ("(", "["):
            depth += 1
        elif t in (")", "]"):
            depth -= 1
        elif depth == 0:
            if t in ALWAYS_BINARY:
                return True
            if t in AMBIGUOUS and k and operand_end(content[k - 1]):
                return True
    return False


PREC = {"*": 10, "/": 10, "%": 10, "+": 9, "-": 9, "<<": 8, ">>": 8, "<": 7, ">": 7, "<=": 7, ">=": 7,
        "==": 6, "!=": 6, "&": 5, "^": 4, "|": 3, "&&": 2, "||": 1, "?": 0.5, ":": 0.5, **{a: 0.2 for a in ASSIGN}}


def min_precedence(content: list[str]) -> float:
    """Lowest precedence among the binary operators at the top level of `content`."""
    depth, low = 0, 99.0
    for k, t in enumerate(content):
        if t in ("(", "["):
            depth += 1
        elif t in (")", "]"):
            depth -= 1
        elif depth == 0 and t in PREC and (t not in AMBIGUOUS or (k and operand_end(content[k - 1]))):
            low = min(low, PREC[t])
    return low


def looks_like_type(content: list[str]) -> bool:
    """A parenthesised type name such as `u8 *`, `unsigned int` or `struct S *`."""
    if not all(is_ident(t) or t == "*" for t in content):
        return False
    stars = len(content) - len([t for t in content if t != "*"])
    if "*" in content[: len(content) - stars]:
        return False  # `a * b` is an expression, a type has its stars last
    return "*" in content or any(t in TYPE_WORDS for t in content) or len(content) > 1


def paren_pairs(toks: list[str]) -> list[tuple[int, int]]:
    stack, pairs = [], []
    for i, t in enumerate(toks):
        if t == "(":
            stack.append(i)
        elif t == ")" and stack:
            pairs.append((stack.pop(), i))
    return pairs


def removable_paren(toks: list[str], i: int, j: int, casts: set[int]) -> bool:
    content = toks[i + 1: j]
    if not content or i in casts:
        return False
    prev = toks[i - 1] if i else ""
    nxt = toks[j + 1] if j + 1 < len(toks) else ""
    if (is_ident(prev) and prev not in ("return",)) or prev == "]":
        return False  # call, sizeof or control-statement parenthesis
    if prev == ")" and (i - 1) not in {c for c in casts_close(toks, casts)}:
        return False
    if looks_like_type(content):
        return False
    after_cast = prev == ")"
    if len(content) == 1 and is_ident(content[0]) and (
            is_ident(nxt) or nxt[:1].isdigit() or nxt[:1] in "\"'" or nxt == "("
            or (nxt in CAST_FOLLOWERS and not after_cast)):
        return False  # may be a cast to a typedef name
    if "," in content and has_top_level_binary(content):
        depth = 0
        for t in content:
            depth += t in ("(", "[")
            depth -= t in (")", "]")
            if t == "," and depth == 0:
                return False
    if not has_top_level_binary(content):
        if nxt in POSTFIX and len(content) > 1 and not (is_ident(content[0]) or content[0][0].isdigit()):
            return False
        return nxt not in ("(",)
    prec = min_precedence(content)
    if prev in PREC and prev not in (":", "?") and not (prev in AMBIGUOUS and not (i > 1 and operand_end(toks[i - 2]))):
        left = PREC[prev]
        left_ok = prec > left
    elif prev in DELIMS or prev == "":
        left_ok = prec >= (0.5 if prev in ("?", ":") else 0.2 if prev in ASSIGN else 0)
    else:
        return False
    if nxt in CLOSERS or nxt == "":
        right_ok = True
    elif nxt in PREC and nxt != ":":
        right_ok = prec > PREC[nxt] if PREC[nxt] < 1 else prec >= PREC[nxt]
    else:
        right_ok = False
    return left_ok and right_ok


def casts_close(toks: list[str], casts: set[int]) -> list[int]:
    return [j for i, j in paren_pairs(toks) if i in casts]


def find_casts(toks: list[str]) -> set[int]:
    """Opening-paren indexes of groups that look like casts `(type)` followed by an operand."""
    casts = set()
    for i, j in paren_pairs(toks):
        content = toks[i + 1: j]
        nxt = toks[j + 1] if j + 1 < len(toks) else ""
        prev = toks[i - 1] if i else ""
        if not content or (is_ident(prev) and prev != "return") or prev == "]":
            continue
        if looks_like_type(content):
            casts.add(i)
        elif len(content) == 1 and is_ident(content[0]) and (is_ident(nxt) or nxt[:1].isdigit() or nxt == "("):
            casts.add(i)
    return casts


def strip_parens(toks: list[str]) -> list[str]:
    """Drop parentheses that cannot change meaning (the permuter's pycparser output adds many)."""
    toks = list(toks)
    for _ in range(20):
        casts = find_casts(toks)
        drop = {k for i, j in paren_pairs(toks) if removable_paren(toks, i, j, casts) for k in (i, j)}
        if not drop:
            break
        toks = [t for k, t in enumerate(toks) if k not in drop]
    return toks


def split_declarators(toks: list[str]) -> list[str]:
    """Rewrite `T a, b;` declaration statements as `T a; T b;` (the permuter prints them split)."""
    out: list[str] = []
    stmt: list[str] = []
    for t in toks:
        if t in (";", "{", "}"):
            out += expand_declaration(stmt, t) if t == ";" else stmt
            if t != ";":
                out.append(t)
            stmt = []
        else:
            stmt.append(t)
    return out + stmt


def expand_declaration(stmt: list[str], end: str) -> list[str]:
    if (len(stmt) < 4 or "," not in stmt or "(" in stmt or "=" in stmt or stmt[0] in NOT_DECL or stmt[0] == "typedef"
            or not (is_ident(stmt[0]) and (is_ident(stmt[1]) or stmt[1] == "*"))):
        return stmt + [end]
    n = next((k for k, t in enumerate(stmt) if is_ident(t) and k + 1 < len(stmt) and stmt[k + 1] in (",", "[")), None)
    if n is None:
        return stmt + [end]
    p = n
    while p > 0 and stmt[p - 1] == "*":
        p -= 1
    typ, parts, cur = stmt[:p], [], []
    for t in stmt[p:]:
        if t == ",":
            parts.append(cur)
            cur = []
        else:
            cur.append(t)
    parts.append(cur)
    return [t for part in parts for t in typ + part + [end]]


def normalize(toks: list[str]) -> list[str]:
    return strip_parens(split_declarators(toks))


def sort_leading_qualifiers(toks: list[str]) -> list[str]:
    n = 0
    while n < len(toks) and toks[n] in STORAGE:
        n += 1
    return sorted(toks[:n]) + toks[n:]


def join_tokens(toks: list[str]) -> str:
    out: list[str] = []
    for i, t in enumerate(toks):
        if i:
            p = toks[i - 1]
            tight = (t in (",", ";", ")", "]", ".", "->") or p in ("(", "[", ".", "->")
                     or (t == "(" and is_ident(p) and p not in KEYWORDS))
            if not tight:
                out.append(" ")
        out.append(t)
    return "".join(out)


def layout(toks: list[str]) -> list[tuple[int, list[str]]]:
    """Break tokens into (indent, tokens) lines: after `;` `{`, around `}`, after labels."""
    lines: list[tuple[int, list[str]]] = []
    cur: list[str] = []
    indent = paren = 0

    def flush():
        nonlocal cur
        if cur:
            lines.append((indent, cur))
            cur = []

    for i, t in enumerate(toks):
        nxt = toks[i + 1] if i + 1 < len(toks) else ""
        if t == "}":
            flush()
            indent = max(0, indent - 1)
        cur.append(t)
        if t == "(":
            paren += 1
        elif t == ")":
            paren = max(0, paren - 1)
        elif t == "{":
            flush()
            indent += 1
        elif t == ";" and paren == 0:
            flush()
        elif t == "}" and indent > 0 and nxt not in ("else", ";", ",", ")"):
            flush()
        elif t == ":" and paren == 0 and cur and (cur[0] in ("case", "default") or (len(cur) == 2 and is_ident(cur[0]))):
            flush()
    flush()
    return lines


def format_text(toks: list[str], indent_step: str = "  ") -> str:
    return "\n".join(indent_step * ind + join_tokens(ln) for ind, ln in layout(toks))


# ---- top-level items ---------------------------------------------------------------------


class Item:
    def __init__(self, toks: list[str]):
        self.toks = toks
        self.kind, self.name = classify(toks)

    @property
    def key(self) -> tuple[str, str]:
        return self.kind, self.name


def split_items(toks: list[str]) -> list[list[str]]:
    items, cur = [], []
    brace = paren = 0
    for t in toks:
        cur.append(t)
        if t == "(":
            paren += 1
        elif t == ")":
            paren = max(0, paren - 1)
        elif t == "{":
            brace += 1
        elif t == "}":
            brace = max(0, brace - 1)
            if brace == 0 and paren == 0 and is_function_def(cur):
                items.append(cur)
                cur = []
        elif t == ";" and brace == 0:
            items.append(cur)
            cur = []
    if cur:
        items.append(cur)
    return items


def header(toks: list[str]) -> list[str]:
    """Tokens before the first `{`, without a trailing `;`."""
    h = toks[: toks.index("{")] if "{" in toks else list(toks)
    return h[:-1] if h and h[-1] == ";" else h


def is_function_def(toks: list[str]) -> bool:
    if "{" not in toks:
        return False
    h = header(toks)
    return bool(h) and h[-1] == ")" and "=" not in h and h[0] != "typedef"


def classify(toks: list[str]) -> tuple[str, str]:
    h = header(toks)
    if not h:
        return "other", " ".join(toks)
    if h[0] == "typedef":
        if "}" in toks:
            after = toks[len(toks) - 1 - toks[::-1].index("}") + 1:]
            names = [t for t in after if is_ident(t)]
            return "typedef", names[0] if names else " ".join(h)
        for i in range(len(h) - 3):
            if h[i] == "(" and h[i + 1] == "*" and is_ident(h[i + 2]) and h[i + 3] == ")":
                return "typedef", h[i + 2]
        names = [t for t in h if is_ident(t)]
        return "typedef", names[-1] if names else " ".join(h)
    eq = h.index("=") if "=" in h else len(h)
    pre = h[:eq]
    if "(" in pre:
        i = pre.index("(")
        if i + 2 < len(pre) and pre[i + 1] == "*" and is_ident(pre[i + 2]):
            return "var", pre[i + 2]  # function pointer variable
        if i and is_ident(pre[i - 1]):
            return ("def" if is_function_def(toks) else "proto"), pre[i - 1]
    if pre[0] in ("struct", "union", "enum") and "{" in toks and len(pre) <= 2:
        return pre[0], " ".join(pre)
    cut = pre.index("[") if "[" in pre else len(pre)
    names = [t for t in pre[:cut] if is_ident(t)]
    return "var", names[-1] if names else " ".join(h)


def parse(text: str) -> list[Item]:
    return [Item(t) for t in split_items(split_declarators(tokenize(text)))]


def item_text(item: Item) -> str:
    toks = item.toks
    return format_text(sort_leading_qualifiers(normalize(toks)))


def declaration_diff(base: str, out: str, name: str) -> list[dict]:
    """Declarations that differ. The target definition contributes its signature only."""
    def texts(items: list[Item], implied: dict) -> dict[tuple[str, str], list[str]]:
        groups: dict[tuple[str, str], list[str]] = collections.OrderedDict()
        for it in items:
            if it.kind == "def" and it.name == name:
                key, text = ("signature", name), format_text(sort_leading_qualifiers(normalize(header(it.toks))))
            else:
                key, text = it.key, item_text(it)
            seen = groups.setdefault(key, [])
            if text not in seen:
                seen.append(text)
            if it.kind == "def" and it.name != name:  # a definition also declares the function
                implied.setdefault(("proto", it.name), []).append(
                    format_text(sort_leading_qualifiers(normalize(header(it.toks) + [";"]))))
        return groups

    base_implied: dict = {}
    out_implied: dict = {}
    old, new = texts(parse(base), base_implied), texts(parse(out), out_implied)
    changes = []
    for key in list(old) + [k for k in new if k not in old]:
        o, n = old.get(key, []), new.get(key, [])
        # A prototype the permuter added for a definition it pruned is not a change.
        n_known = set(n) | set(out_implied.get(key, []))
        o_known = set(o) | set(base_implied.get(key, []))
        if set(o) == set(n) or (n_known == o_known and set(n) <= o_known and set(o) <= n_known):
            continue
        if key[0] == "def" and not n:
            continue  # the permuter drops other function definitions from source.c
        changes.append({"kind": key[0], "name": key[1],
                        "removed": [t for t in o if t not in n], "added": [t for t in n if t not in o]})
    return changes


def omitted_definitions(base: str, out: str, name: str) -> int:
    """Other function definitions present in base.c but pruned from the permuter's source.c."""
    have = {it.name for it in parse(out) if it.kind == "def"}
    return len({it.name for it in parse(base) if it.kind == "def" and it.name != name and it.name not in have})


# ---- target body -----------------------------------------------------------------------------


def function_tokens(text: str, name: str) -> list[str] | None:
    for it in parse(text):
        if it.kind == "def" and it.name == name:
            return it.toks
    return None


def body_lines(toks: list[str]) -> list[str]:
    return [("  " * ind) + join_tokens(ln) for ind, ln in layout(normalize(toks))]


def body_diff(base: str, out: str, name: str) -> list[str] | None:
    """Unified diff lines of the target function (None if either side lacks it)."""
    b, o = function_tokens(base, name), function_tokens(out, name)
    if b is None or o is None:
        return None
    return list(difflib.unified_diff(body_lines(b), body_lines(o), "base.c", "source.c", lineterm="", n=3))


# ---- suspicious changes ----------------------------------------------------------------------


def void_pointer_names(toks: list[str], items: list[Item]) -> set[str]:
    names = set()
    streams = [toks] + [it.toks for it in items if it.kind == "var"]
    for s in streams:
        for i in range(len(s) - 2):
            if s[i] == "void" and s[i + 1] == "*":
                j = i + 2
                while j < len(s) and s[j] in ("const", "volatile", "*"):
                    j += 1
                if j < len(s) and is_ident(s[j]):
                    names.add(s[j])
    return names


def void_arithmetic(toks: list[str], items: list[Item]) -> collections.Counter:
    """Snippets where a `void *` variable or cast result is used with binary + or -."""
    names = void_pointer_names(toks, items)
    found: collections.Counter = collections.Counter()
    arith = ("+", "-", "++", "--", "+=", "-=")
    for i, t in enumerate(toks):
        hit = False
        if t in names and (i == 0 or toks[i - 1] not in (".", "->")):
            nxt = toks[i + 1] if i + 1 < len(toks) else ""
            prev = toks[i - 1] if i else ""
            hit = nxt in arith or (prev in ("+", "-") and i > 1 and (is_ident(toks[i - 2]) or toks[i - 2] in (")", "]")
                                                                or toks[i - 2][0].isdigit()))
        elif t == "(" and toks[i + 1: i + 4] == ["void", "*", ")"] and i + 5 < len(toks):
            hit = toks[i + 5] in arith and (is_ident(toks[i + 4]) or toks[i + 4][0].isdigit())
        if hit:
            found[join_tokens(toks[max(0, i - 2): i + 6])] += 1
    return found


def uninitialised_locals(toks: list[str]) -> list[str]:
    """Scalar locals declared without initialiser whose first mention is not an assignment."""
    lines = [ln for _, ln in layout(normalize(toks))]
    start = next((i for i, ln in enumerate(lines) if ln and ln[-1] == "{" and ln[0] != "{" and "(" in ln), 0)
    declared: dict[str, int] = {}
    for idx in range(start + 1, len(lines)):
        ln = lines[idx]
        if ln[-1] != ";" or "=" in ln or "(" in ln or ln[0] in NOT_DECL or ln[0] in ("{", "}"):
            continue
        body = [t for t in ln[:-1] if t not in STORAGE and t not in ("unsigned", "signed", "struct", "union")]
        if len(body) < 2 or not is_ident(body[0]):
            continue
        rest = body[1:]
        if "[" in rest:
            continue
        for part in " ".join(rest).split(","):
            ids = [t for t in part.split() if is_ident(t)]
            if ids:
                declared.setdefault(ids[-1], idx)
    bad = []
    flat = [(idx, t) for idx, ln in enumerate(lines) for t in ln]
    for var, didx in declared.items():
        for k, (idx, t) in enumerate(flat):
            if t != var or idx <= didx:
                continue
            prev = flat[k - 1][1] if k else ""
            nxt = flat[k + 1][1] if k + 1 < len(flat) else ""
            if prev in (".", "->"):
                continue
            if nxt == "=" or prev == "&":
                break  # assigned (or address taken) first
            bad.append(var)
            break
    return bad


def suspicious(base: str, out: str, name: str, changes: list[dict]) -> list[str]:
    flags = []
    bt, ot = function_tokens(base, name), function_tokens(out, name)
    if bt is not None and ot is not None:
        extra = void_arithmetic(normalize(ot), parse(out)) - void_arithmetic(normalize(bt), parse(base))
        for snippet in extra:
            flags.append(f"arithmetic on void *: `{snippet}` (not in base)")
        base_bad = set(uninitialised_locals(bt))
        for var in uninitialised_locals(ot):
            if var not in base_bad:
                flags.append(f"local `{var}` may be read before assignment (first mention is not `{var} =`)")
    bv, ov = base_tokens(base).count("volatile"), base_tokens(out).count("volatile")
    if ov < bv:
        flags.append(f"volatile removed: {bv} in base.c, {ov} in source.c")
    for ch in changes:
        if any("volatile" in t for t in ch["removed"]) and not any("volatile" in t for t in ch["added"]):
            flags.append(f"volatile dropped from declaration `{ch['name']}`")
    return flags


def base_tokens(text: str) -> list[str]:
    return tokenize(text)


# ---- report --------------------------------------------------------------------------------------

KIND_LABEL = {"proto": "prototype", "def": "function definition", "var": "variable/extern", "struct": "struct",
              "union": "union", "enum": "enum", "typedef": "typedef", "signature": "target signature",
              "other": "other"}


def render(name: str, label: str, base: str, out: str) -> tuple[str, dict]:
    changes = declaration_diff(base, out, name)
    diff = body_diff(base, out, name)
    flags = suspicious(base, out, name, changes)
    lines = [f"== {name}: {label}"]
    lines.append(f"-- declaration changes ({len(changes)})")
    if not changes:
        lines.append("  none")
    omitted = omitted_definitions(base, out, name)
    if omitted:
        lines.append(f"  ({omitted} other function definitions in base.c are absent from source.c; not shown)")
    for ch in changes:
        lines.append(f"~ {ch['name']}  [{KIND_LABEL.get(ch['kind'], ch['kind'])}]")
        for t in ch["removed"]:
            lines += ["  - " + ln for ln in t.splitlines()]
        for t in ch["added"]:
            lines += ["  + " + ln for ln in t.splitlines()]
    if diff is None:
        lines.append(f"-- body diff: function {name} not found in base.c or source.c")
    elif not diff:
        lines.append("-- body diff: identical after normalisation")
        if changes:
            lines.append("NOTE: body unchanged; any improvement comes from the declaration changes above")
    else:
        lines.append("-- body diff")
        lines += diff
    lines.append(f"-- suspicious ({len(flags)})")
    lines += [f"  ! {f}" for f in flags] or ["  none"]
    return "\n".join(lines), {"changes": changes, "diff": diff, "flags": flags}


def output_score(path: Path) -> tuple[int, int] | None:
    m = re.fullmatch(r"output-(\d+)-(\d+)", path.name)
    return (int(m.group(1)), int(m.group(2))) if m else None


def pick_output(name: str, spec: str | None, root: Path = PERMUTE_DIR) -> Path:
    work = root / name
    if spec:
        for cand in (Path(spec), work / spec):
            if cand.is_dir():
                return cand
        raise SystemExit(f"permute_review: no output directory {spec} (looked in . and {work})")
    outs = sorted((p for p in work.glob("output-*") if p.is_dir() and output_score(p)), key=output_score)
    if not outs:
        raise SystemExit(f"permute_review: no output-* directories in {work}")
    return outs[0]


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("name", help="target function name (work/permute/NAME)")
    ap.add_argument("output", nargs="?", help="output dir (path or name under work/permute/NAME); default best score")
    ap.add_argument("--base", help="base.c to compare against (default: base.c beside the output dir)")
    ap.add_argument("--root", default=str(PERMUTE_DIR), help="permute work directory (default work/permute)")
    args = ap.parse_args(argv)
    root = Path(args.root)
    out_dir = pick_output(args.name, args.output, root)
    base_path = Path(args.base) if args.base else out_dir.parent / "base.c"
    if not base_path.exists():
        base_path = root / args.name / "base.c"
    src = out_dir / "source.c"
    for p in (base_path, src):
        if not p.exists():
            raise SystemExit(f"permute_review: missing {p}")
    score = output_score(out_dir)
    label = f"{out_dir.name}" + (f" (score {score[0]})" if score else "") + f" vs {base_path.name}"
    text, _ = render(args.name, label, base_path.read_text(errors="replace"), src.read_text(errors="replace"))
    print(text)
    return 0


if __name__ == "__main__":
    sys.exit(main())
