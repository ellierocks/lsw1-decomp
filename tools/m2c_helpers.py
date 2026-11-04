"""Shared, pure m2c input/output repairs for interactive drafts and sweeps."""
from __future__ import annotations
import re

UNKNOWN_TYPES = {"M2C_UNK": "s32", "M2C_UNK8": "s8", "M2C_UNK16": "s16", "M2C_UNK32": "s32",
                 "M2C_UNK64": "s64"}

def split_args(text: str, start: int) -> tuple[list[str], int]:
    """Comma-separated arguments of the call whose `(` is at text[start]; returns (args, end)."""
    depth, args, cur = 0, [], start + 1
    for i in range(start, len(text)):
        c = text[i]
        if c == "(":
            depth += 1
        elif c == ")":
            depth -= 1
            if depth == 0:
                args.append(text[cur:i])
                return [a.strip() for a in args], i + 1
        elif c == "," and depth == 1:
            args.append(text[cur:i])
            cur = i + 1
    raise ValueError("unbalanced macro call")

def expand_macros(text: str) -> str:
    """Rewrite m2c valid-syntax macros as plain C casts so a matched draft needs no header."""
    for name, ctype in UNKNOWN_TYPES.items():
        text = re.sub(rf"\b{name}\b", ctype, text)
    # With a typed context m2c reads the first member of a struct local as
    # M2C_BITWISE(f32, sp8); a cast cannot convert a struct, so reinterpret its storage.
    struct_locals = set(re.findall(r"^\s+struct \w+ (\w+);", text, flags=re.M))
    while (m := re.search(r"\bM2C_BITWISE\s*\(", text)):
        args, end = split_args(text, m.end() - 1)
        fmt = "(*({0} *)&({1}))" if args[1] in struct_locals else "(({0})({1}))"
        text = text[:m.start()] + fmt.format(*args) + text[end:]
    for macro, fmt in (("M2C_FIELD", "(*({1})((s8 *)({0}) + ({2})))"),
                       ("M2C_STRUCT_COPY", "memcpy({0}, {1}, {2})"),
                       ("M2C_MEMCPY_ALIGNED", "memcpy({0}, {1}, {2})")):
        while (m := re.search(rf"\b{macro}\s*\(", text)):
            args, end = split_args(text, m.end() - 1)
            text = text[:m.start()] + fmt.format(*args) + text[end:]
    return text

def fold_cror(asm: str) -> str:
    """Qualify default-field bit aliases without folding away unordered comparisons."""
    def qualify(match):
        return match[1] + re.sub(r"(?<![\w])(lt|gt|eq|un|so)(?![\w])",
                                lambda bit: "cr0" + bit[0], match[2])
    return re.sub(r"(\bcr(?:andc?|orc?|xor|nor|nand|eqv|clr|set|move)\s+)([^\n]*)",
                  qualify, asm)


PS_BLOCK = re.compile(
    r"psq_l f1, 0x0\((?P<a>r\d+)\), 0, qr0\npsq_l f3, 0x0\((?P<b>r\d+)\), 0, qr0\n"
    r"psq_l f2, 0x8\((?P=a)\), 1, qr0\npsq_l f4, 0x8\((?P=b)\), 1, qr0\n"
    r"ps_(?P<op>add|sub) f1, f1, f3\nps_(?P=op) f2, f2, f4\n"
    r"psq_st f1, 0x0\((?P<d>r\d+)\), 0, qr0\npsq_st f2, 0x8\((?P=d)\), 1, qr0\n")

ASM_COMMENT = re.compile(r"^/\*.*?\*/\s*")

PS_CALLS = ("NuVecAddPS", "NuVecSubPS")

def fix_ps_calls(body: str) -> str:
    """Give every NuVec{Add,Sub}PS call exactly its three arguments."""
    for fn in PS_CALLS:
        for start, end, args in reversed(list(call_sites(body, fn))):
            fixed = (args[:3] + ["0"] * 3)[:3]
            call = f"{fn}({', '.join(fixed)})"
            # The block is an asm statement with no value; m2c models r3 as the return
            # value, and after the fold's `mr r3, dst` that is the destination.
            if (m := re.search(r"([ \t]*)(\w+) = $", body[:start])) and body[end:end + 1] == ";":
                body = (body[:m.start()] + f"{m.group(1)}{call};\n{m.group(1)}{m.group(2)} = {fixed[0]}"
                        + body[end:])
            else:
                body = body[:start] + call + body[end:]
    return body

def moves_to_args(srcs: list[str]) -> list[str]:
    """`mr` lines that load SRCS into r3, r4, r5 in parallel (r0 breaks a cycle). Registers
    already in place get a trailing `mr rN, rN` so m2c still sees them passed."""
    pending = [(f"r{3 + i}", s) for i, s in enumerate(srcs) if s != f"r{3 + i}"]
    out = []
    while pending:
        free = next(((d, s) for d, s in pending if all(d != s2 for _, s2 in pending)), None)
        if free is None:
            d0 = pending[0][0]
            out.append(f"\tmr r0, {d0}")
            pending = [(d, "r0" if s == d0 else s) for d, s in pending]
            continue
        pending.remove(free)
        out.append(f"\tmr {free[0]}, {free[1]}")
    out += [f"\tmr r{3 + i}, r{3 + i}" for i, s in enumerate(srcs) if s == f"r{3 + i}"]
    return out

def fold_ps(asm: str) -> str:
    """Rewrite each inlined paired-single vector add/sub block as a call m2c can read."""
    lines = asm.splitlines()
    text = [ASM_COMMENT.sub("", ln).strip() for ln in lines]
    out, i = [], 0
    while i < len(lines):
        m = PS_BLOCK.match("\n".join(text[i:i + 8]) + "\n") if i + 8 <= len(lines) else None
        if not m:
            out.append(lines[i])
            i += 1
            continue
        body = [*moves_to_args([m["d"], m["a"], m["b"]]), f"\tbl {PS_CALLS[m['op'] == 'sub']}"]
        out.extend(body)
        i += 8
    return "\n".join(out) + "\n"

UNTYPED_DEREF = re.compile(r"\*\((?![\w ]+\*+\))")

def type_derefs(text: str) -> str:
    return UNTYPED_DEREF.sub("*(s32 *)(", text)

def strip_vararg_saves(asm: str) -> str:
    """Drop a variadic prologue's `bne cr1, .L; stfd f1..f8` float save area.

    The compiler emits it itself (cr1eq is the ABI's "no float arguments" flag), and m2c
    would read cr1eq and the float registers before anything sets them. The arguments the
    body reads from the va_list area remain stack accesses in the draft.
    """
    lines = asm.splitlines()
    for i, line in enumerate(lines):
        m = re.search(r"\bbne cr1, (\.L_\w+)\s*$", line)
        if not m:
            continue
        saves = lines[i + 1:i + 9]
        if all(re.search(rf"\bstfd f{n + 1}, 0x[0-9a-f]+\(r1\)\s*$", ln) for n, ln in enumerate(saves)) \
                and len(saves) == 8:
            return "\n".join(lines[:i] + lines[i + 9:]) + "\n"
    return asm

UNSET = re.compile(r"M2C_ERROR\(/\* Read from unset register \$(?P<r>[rf]\d+) \*/\)")

def declare_unset(text: str, name: str) -> str:
    """Name registers the target reads before writing as uninitialized locals.

    The retail code passes or stores a variable that was never assigned (an `mr r10, r11`
    with r11 unset); the local keeps that visible in the draft instead of an M2C_ERROR.
    """
    regs = sorted({m["r"] for m in UNSET.finditer(text)})
    # m2c names a callee-saved register read before any write (`saved_reg_r28`) without declaring it.
    saved = sorted({s for s in re.findall(r"\bsaved_reg_\w+\b", text)
                    if not re.search(rf"^\s+\w[\w ]*\b{s};", text, flags=re.M)})
    if not regs and not saved:
        return text
    decls = "".join(f"    {'s32' if r[0] == 'r' else 'f64'} unset_{r};\n" for r in regs)
    decls += "".join(f"    {'f64' if s.startswith('saved_reg_f') else 's32'} {s};\n" for s in saved)
    text = UNSET.sub(lambda m: "unset_" + m["r"], text)
    return re.sub(rf"^(.*\b{re.escape(name)}\(.*\) \{{\n)", lambda m: m.group(1) + decls, text,
                  count=1, flags=re.M)

def call_sites(text: str, fn: str):
    for m in re.finditer(rf"(?<![\w.>]){re.escape(fn)}\s*\(", text):
        try:
            args, end = split_args(text, m.end() - 1)
        except ValueError:
            continue
        yield m.start(), end, [] if args == [""] else args


def prepare_asm(asm: str) -> str:
    return fold_ps(fold_cror(strip_vararg_saves(asm)))


def repair_indirect_prototypes(body: str, asm: str) -> str:
    """Use unspecified arguments only when every indirect call sets the ABI cr1eq bit.

    Mixed call conventions need data-flow analysis; leave those drafts for manual review.
    """
    instructions = [ASM_COMMENT.sub("", line).strip() for line in asm.splitlines()]
    calls = [i for i, line in enumerate(instructions) if re.match(r"(?:blrl|bctrl)\b", line)]
    if not calls:
        return body
    previous = -1
    for i in calls:
        if not any(re.match(r"cr(?:clr|set) cr1eq$", line)
                   for line in instructions[max(previous + 1, i - 3):i]):
            return body
        previous = i
    return re.sub(r"(\(\*\s*\w+(?:\([^;{}\n]*?\))?\s*\)\s*)\([^;{}\n]*\)(?=;)",
                  r"\1()", body)
