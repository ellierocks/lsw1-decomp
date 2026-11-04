#!/usr/bin/env python3
"""Read-only GameCube/loose asset inventory and conservative script evidence index."""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CHUNK = 1024 * 1024
MAX_SCRIPT = 16 * CHUNK
TABLES = ((0x801BC040, 8, 'Script_Keyword_Parser'),
          (0x801BC068, 8, 'Script_Keyword_Parser2'),
          (0x801BC090, 12, 'AI_Conditions_API'),
          (0x801BC288, 8, 'AI_Actions_API'),
          (0x801E0D14, 12, 'Level_Conditions'),
          (0x801E1158, 8, 'Level_Actions'))


def digest(path: Path, offset: int = 0, length: int | None = None) -> tuple[str, str]:
    size = path.stat().st_size
    if length is None:
        length = size - offset
    if offset < 0 or length < 0 or offset + length > size:
        raise ValueError(f'Out-of-bounds range in {path}')
    h = hashlib.sha256()
    prefix = b''
    with path.open('rb') as f:
        f.seek(offset)
        remaining = length
        while remaining:
            data = f.read(min(CHUNK, remaining))
            if not data:
                raise ValueError(f'Truncated input: {path}')
            prefix = (prefix + data[:16])[:16]
            h.update(data)
            remaining -= len(data)
    return h.hexdigest(), prefix.hex()


def member_name(name: str) -> str:
    if not name or name in ('.', '..') or '/' in name or '\\' in name:
        raise ValueError(f'Unsafe member name: {name!r}')
    return name


def gc_members(path: Path) -> list[dict]:
    """Parse a GC FST, validating nesting, names and every file range."""
    size = path.stat().st_size
    with path.open('rb') as f:
        header = f.read(0x430)
        if len(header) != 0x430 or struct.unpack_from('>I', header, 0x1C)[0] != 0xC2339F3D:
            raise ValueError('Not a GameCube disc image')
        offset, length = struct.unpack_from('>II', header, 0x424)
        if offset < 0x430 or length < 12 or offset + length > size or length > 64 * CHUNK:
            raise ValueError('Invalid or oversized FST range')
        f.seek(offset)
        raw = f.read(length)
    root, parent, count = struct.unpack_from('>III', raw)
    if root != 0x01000000 or parent != 0 or not 1 <= count <= length // 12:
        raise ValueError('Invalid FST root')
    names = raw[count * 12:]
    stack = [('', count, 0)]
    result = []
    seen = set()
    for index in range(1, count):
        while stack and index >= stack[-1][1]:
            stack.pop()
        if not stack:
            raise ValueError('Invalid FST nesting')
        tag, start, end = struct.unpack_from('>III', raw, index * 12)
        kind, name_offset = tag >> 24, tag & 0xFFFFFF
        stop = names.find(b'\0', name_offset)
        if kind not in (0, 1) or name_offset >= len(names) or stop < 0:
            raise ValueError('Invalid FST name/type')
        # Retain byte identity for non-ASCII filenames in JSON via surrogateescape.
        name = member_name(names[name_offset:stop].decode('utf-8', 'surrogateescape'))
        full = '/'.join(filter(None, (stack[-1][0], name)))
        if full in seen:
            raise ValueError('Duplicate FST path')
        seen.add(full)
        if kind:
            if start != stack[-1][2] or not index < end <= stack[-1][1]:
                raise ValueError('Invalid FST directory parent/end')
            stack.append((full, end, index))
        else:
            if start + end > size:
                raise ValueError('FST file outside image')
            result.append({'member': full, 'offset': start, 'length': end})
    return result


def inventory(source: Path, build: str, kind: str) -> dict:
    source = source.resolve(strict=True)
    rows = []
    if kind == 'gc':
        source_hash = digest(source)[0]
        for entry in gc_members(source):
            sha, magic = digest(source, entry['offset'], entry['length'])
            rows.append(dict(entry, source=str(source), source_sha256=source_hash,
                             sha256=sha, prefix_hex=magic, method='gc-fst'))
    else:
        if not source.is_dir():
            raise ValueError('Loose input must be a directory')
        # Do not follow symlinks outside the selected installation.
        for path in sorted(source.rglob('*')):
            if path.is_symlink():
                raise ValueError(f'Symlink in corpus: {path}')
            if path.is_file():
                sha, magic = digest(path)
                rows.append({'member': path.relative_to(source).as_posix(),
                             'source': str(path), 'source_sha256': sha,
                             'offset': 0, 'length': path.stat().st_size,
                             'sha256': sha, 'prefix_hex': magic, 'method': 'loose'})
    for row in rows:
        row['extension'] = Path(row['member']).suffix.lower()
    return {'schema': 1, 'build': build, 'kind': kind, 'source': str(source), 'files': rows}


def dispatch(dol: Path, symbols: Path) -> dict:
    """Read explicit retail tables using actual DOL section mappings, no BSS guessing."""
    data = dol.read_bytes()
    if len(data) < 0x100:
        raise ValueError('Truncated DOL')
    sections = []
    for i in range(18):
        off = struct.unpack_from('>I', data, i * 4)[0]
        addr = struct.unpack_from('>I', data, 0x48 + i * 4)[0]
        size = struct.unpack_from('>I', data, 0x90 + i * 4)[0]
        if size:
            if off < 0x100 or off + size > len(data):
                raise ValueError('Invalid DOL section')
            sections.append((addr, off, size, i < 7))

    def read(addr, size):
        for base, off, length, _ in sections:
            if base <= addr and addr + size <= base + length:
                return data[off + addr - base:off + addr - base + size]
        raise ValueError(f'Unmapped DOL address {addr:#x}')

    names = {}
    for line in symbols.read_text().splitlines():
        m = re.match(r'(\S+)\s*=\s*\.text:0x([0-9A-Fa-f]+);', line)
        if m:
            names[int(m[2], 16)] = m[1]
    commands = {}
    for base, stride, table in TABLES:
        for index in range(256):
            words = struct.unpack('>' + 'I' * (stride // 4), read(base + index * stride, stride))
            string, handler = words[:2]
            if handler == 0:
                break
            if not any(code and addr <= handler < addr + length for addr, _, length, code in sections):
                raise ValueError(f'Non-code handler in {table}')
            word = bytearray()
            for n in range(256):
                c = read(string + n, 1)[0]
                if c == 0:
                    break
                word.append(c)
            else:
                raise ValueError(f'Unterminated command in {table}')
            command = word.decode('ascii')
            if not command:
                raise ValueError(f'Empty command in {table}')
            entry = {'table': table, 'index': index, 'address': f'0x{handler:08X}',
                     'symbol': names.get(handler), 'extra_word': words[2] if stride == 12 else None}
            commands.setdefault(command, []).append(entry)
        else:
            raise ValueError(f'Unterminated dispatch table {table}')
    return {'dol_sha256': hashlib.sha256(data).hexdigest(),
            'symbols_sha256': digest(symbols)[0], 'commands': commands}


# Lexical evidence only: unknown grammar and unmatched text remain in records.
TOKEN = re.compile(r"//[^\r\n]*|/\*[\s\S]*?(?:\*/|\Z)|\"(?:\\.|[^\"\\])*\"|'(?:\\.|[^'\\])*'|[{}]|(?:(?!//|/\*)[^\s{}\"'])+", re.MULTILINE)


def script_records(raw: bytes, commands: dict) -> dict:
    if raw.startswith((b'\xff\xfe', b'\xfe\xff')):
        encoding, skip = ('utf-16-le', 2) if raw[:2] == b'\xff\xfe' else ('utf-16-be', 2)
    else:
        encoding, skip = 'utf-8', 3 if raw.startswith(b'\xef\xbb\xbf') else 0
    try:
        text = raw[skip:].decode(encoding)
    except UnicodeDecodeError as exc:
        return {'encoding': encoding, 'records': [], 'diagnostics': [f'Decode failed: {exc}'],
                'status': 'unparsed'}
    offsets = [skip]
    for char in text:
        offsets.append(offsets[-1] + len(char.encode(encoding)))
    records, diagnostics, scope = [], [], []
    current, line, line_start, pos = [], 1, 0, 0

    def flush():
        if current:
            records.append({'line': current[0]['line'], 'scope': list(scope),
                            'tokens': list(current), 'candidates': [
                                {'token': i, 'command': t['text'], 'handlers': commands[t['text']]}
                                for i, t in enumerate(current) if t['kind'] == 'word' and t['text'] in commands]})
            current.clear()

    for match in TOKEN.finditer(text):
        gap = text[pos:match.start()]
        if gap.strip():
            diagnostics.append(f'Unparsed text at byte {offsets[pos]}')
        if '\n' in gap:
            flush()
        line += gap.count('\n')
        if '\n' in gap:
            line_start = text.rfind('\n', pos, match.start()) + 1
        value = match[0]
        token_line = line
        if value.startswith(('//', '/*')):
            if value.startswith('/*') and not value.endswith('*/'):
                diagnostics.append(f'Unterminated comment at line {line}')
            if '\n' in value:
                flush()
        elif value == '{':
            label = ' '.join(t['text'] for t in current)
            if not label and records and records[-1]['tokens'][-1]['line'] == token_line - 1:
                label = ' '.join(t['text'] for t in records[-1]['tokens'])
            flush()
            scope.append({'label': label, 'line': token_line, 'byte': offsets[match.start()]})
        elif value == '}':
            flush()
            if scope:
                scope.pop()
            else:
                diagnostics.append(f'Unmatched closing brace at line {line}')
        else:
            current.append({'text': value, 'kind': 'quoted' if value[0] in '\"\'' else 'word',
                            'line': token_line, 'column': match.start() - line_start + 1,
                            'byte': offsets[match.start()], 'end_byte': offsets[match.end()]})
        line += value.count('\n')
        if '\n' in value:
            line_start = text.rfind('\n', match.start(), match.end()) + 1
        pos = match.end()
    flush()
    if text[pos:].strip():
        diagnostics.append(f'Unparsed trailing text at byte {offsets[pos]}')
    if scope:
        diagnostics.append('Unclosed brace scopes')
    return {'encoding': encoding, 'records': records, 'diagnostics': diagnostics,
            'status': 'lexical-candidates'}


def index_scripts(manifest: dict, suffixes: list[str], mapping: dict) -> dict:
    results, checked = [], set()
    for row in manifest['files']:
        if Path(row['member']).suffix.lower() not in suffixes:
            continue
        source = Path(row['source'])
        key = (str(source), row['source_sha256'])
        if key not in checked:
            if digest(source)[0] != row['source_sha256']:
                raise ValueError(f'Source changed since inventory: {source}')
            checked.add(key)
        if row['length'] > MAX_SCRIPT:
            results.append({'asset': row, 'status': 'unparsed', 'diagnostics': ['Script exceeds 16 MiB limit']})
            continue
        if digest(source, row['offset'], row['length'])[0] != row['sha256']:
            raise ValueError(f'Member hash mismatch: {row["member"]}')
        with source.open('rb') as f:
            f.seek(row['offset'])
            raw = f.read(row['length'])
        results.append({'asset': row, **script_records(raw, mapping.get('commands', {}))})
    return {'schema': 1, 'build': manifest['build'], 'dispatch': mapping,
            'interpretation': 'Exact-case lexical candidates, not parsed commands or runtime reachability',
            'scripts': results}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest='command', required=True)
    inv = sub.add_parser('inventory')
    inv.add_argument('source', type=Path)
    inv.add_argument('--kind', choices=['gc', 'loose'], required=True)
    inv.add_argument('--build', required=True)
    idx = sub.add_parser('scripts')
    idx.add_argument('inventory', type=Path)
    idx.add_argument('--suffix', action='append', default=None, help='Explicit script suffix; repeatable (default .scp)')
    idx.add_argument('--dol', type=Path, help='Optional GL5E4F DOL for native dispatch links')
    idx.add_argument('--symbols', type=Path, default=ROOT / 'config/GL5E4F/symbols.txt')
    for child in (inv, idx):
        child.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    try:
        output = args.output.resolve()
        if not any(output.is_relative_to(ROOT / folder) for folder in ('work', 'build')):
            raise ValueError('Keep derived reports under this checkout\'s work/ or build/')
        if output.exists():
            raise ValueError('Output already exists; choose a new report path')
        if args.command == 'inventory':
            if args.source.is_dir() and output.is_relative_to(args.source.resolve()):
                raise ValueError('Output must be outside the input corpus')
            result = inventory(args.source, args.build, args.kind)
        else:
            manifest = json.loads(args.inventory.read_text())
            if args.dol and manifest['build'] != 'GL5E4F':
                raise ValueError('Retail dispatch addresses require --build GL5E4F inventory')
            if args.dol and hashlib.sha1(args.dol.read_bytes()).hexdigest() != '95cca08a19224775d1a8d6cc64601fb7d0080981':
                raise ValueError('Dispatch linking requires the pinned GL5E4F retail DOL')
            mapping = dispatch(args.dol, args.symbols) if args.dol else {}
            result = index_scripts(manifest, args.suffix or ['.scp'], mapping)
            result['inventory_sha256'] = digest(args.inventory)[0]
        output.parent.mkdir(parents=True, exist_ok=True)
        with output.open('x') as f:
            json.dump(result, f, indent=2, ensure_ascii=True)
            f.write('\n')
        print(f'Wrote {output}')
    except (OSError, ValueError, KeyError, struct.error) as exc:
        parser.exit(1, f'error: {exc}\n')


if __name__ == '__main__':
    main()
