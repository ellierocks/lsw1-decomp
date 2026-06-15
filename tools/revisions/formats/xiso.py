"""Xbox XISO (XDVD) disc image extraction."""
from __future__ import annotations

import struct
from pathlib import Path

SECTOR = 2048
XISO_MAGIC = b"MICROSOFT*XBOX*MEDIA"
XISO_HEADER_SECTOR = 32


def _read_sector(f, lba: int) -> bytes:
    """Read one 2048-byte sector at absolute LBA."""
    f.seek(lba * SECTOR)
    return f.read(SECTOR)


def _parse_xiso_dir(f, dir_sector: int, dir_size: int) -> list[dict]:
    """
    Parse an XISO directory sector tree.
    dir_sector and all sector references in entries are absolute LBAs.
    """
    entries = []
    raw = b""
    sectors_needed = (dir_size + SECTOR - 1) // SECTOR
    for i in range(sectors_needed):
        raw += _read_sector(f, dir_sector + i)

    def _walk(offset: int):
        if offset == 0xFFFF:
            return
        # Each XISO dir entry: left(2), right(2), sector(4), size(4), attrs(1), name_len(1), name[]
        byte_off = offset * 4
        if byte_off + 14 > len(raw):
            return
        left = struct.unpack_from("<H", raw, byte_off)[0]
        right = struct.unpack_from("<H", raw, byte_off + 2)[0]
        sector = struct.unpack_from("<I", raw, byte_off + 4)[0]
        size = struct.unpack_from("<I", raw, byte_off + 8)[0]
        attrs = raw[byte_off + 12]
        name_len = raw[byte_off + 13]
        name = raw[byte_off + 14: byte_off + 14 + name_len].decode("ascii", errors="replace")
        is_dir = bool(attrs & 0x10)
        if name:
            entries.append({"name": name, "sector": sector, "size": size, "is_dir": is_dir})
        if left not in (0, 0xFFFF):
            _walk(left)
        if right not in (0, 0xFFFF):
            _walk(right)

    _walk(0)
    return entries


def list_xiso(disc_path: Path) -> list[dict] | None:
    """
    List files in an XISO image. Returns None if not a valid XISO.
    Each entry: {path, sector, size}  (sector is absolute LBA)
    """
    with open(disc_path, "rb") as f:
        header = _read_sector(f, XISO_HEADER_SECTOR)
        if header[:20] != XISO_MAGIC:
            return None
        root_sector = struct.unpack_from("<I", header, 20)[0]
        root_size = struct.unpack_from("<I", header, 24)[0]

        def _recurse(entries: list[dict], path_prefix: str) -> list[dict]:
            result = []
            for e in entries:
                full_path = f"{path_prefix}/{e['name']}" if path_prefix else e["name"]
                if e["is_dir"]:
                    sub = _parse_xiso_dir(f, e["sector"], e["size"])
                    result.extend(_recurse(sub, full_path))
                else:
                    result.append({"path": full_path, "sector": e["sector"], "size": e["size"]})
            return result

        root_entries = _parse_xiso_dir(f, root_sector, root_size)
        return _recurse(root_entries, "")


def extract_xbe(disc_path: Path, out_path: Path, xbe_name: str = "default.xbe") -> Path | None:
    """Extract the main XBE from an XISO image."""
    entries = list_xiso(disc_path)
    if entries is None:
        return None

    target = next(
        (e for e in entries if e["path"].lower().endswith(xbe_name.lower())), None
    )
    if not target:
        target = next((e for e in entries if e["path"].lower().endswith(".xbe")), None)
    if not target:
        return None

    out_path.parent.mkdir(parents=True, exist_ok=True)
    with open(disc_path, "rb") as f:
        remaining = target["size"]
        lba = target["sector"]
        with open(out_path, "wb") as out:
            while remaining > 0:
                f.seek(lba * SECTOR)
                chunk = f.read(min(remaining, SECTOR))
                out.write(chunk)
                remaining -= len(chunk)
                lba += 1
    return out_path
