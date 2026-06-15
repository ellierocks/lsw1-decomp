"""GameCube ISO extraction — locate and extract main.dol."""
from __future__ import annotations

import struct
from pathlib import Path

GC_SECTOR = 2048
GC_MAGIC = 0xC2339F3D


def extract_gc_dol(disc_path: Path, out_path: Path) -> Path | None:
    """Extract main.dol from a GameCube ISO image."""
    with open(disc_path, "rb") as f:
        # GC disc header: magic at 0x1C, DOL offset at 0x420
        f.seek(0x1C)
        magic = struct.unpack(">I", f.read(4))[0]
        if magic != GC_MAGIC:
            return None

        f.seek(0x420)
        dol_offset = struct.unpack(">I", f.read(4))[0]

        # DOL header: sizes of text+data sections
        f.seek(dol_offset)
        hdr = f.read(0x100)
        text_sizes = struct.unpack_from(">7I", hdr, 0x90)
        data_sizes = struct.unpack_from(">11I", hdr, 0xAC)
        total_size = sum(text_sizes) + sum(data_sizes) + 0x100

        f.seek(dol_offset)
        dol_data = f.read(total_size)

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_bytes(dol_data)
    return out_path
