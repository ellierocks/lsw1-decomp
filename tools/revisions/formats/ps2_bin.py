"""PS2 disc image extraction (Mode 2 Form 1 .bin/.cue and raw ISO)."""
from __future__ import annotations

import re
import struct
from pathlib import Path


# PS2 bin sector layout (Mode 2 Form 1)
BIN_SECTOR_SIZE = 2352
BIN_DATA_OFFSET = 24   # 12 sync + 3 addr + 1 mode + 4 subhdr + 4 subhdr copy
BIN_DATA_SIZE = 2048
ISO_SECTOR_SIZE = 2048


def _is_bin(path: Path) -> bool:
    return path.stat().st_size % BIN_SECTOR_SIZE == 0 and path.suffix.lower() in (".bin", ".img")


def _read_sector(f, lba: int, is_bin: bool) -> bytes:
    if is_bin:
        f.seek(lba * BIN_SECTOR_SIZE + BIN_DATA_OFFSET)
    else:
        f.seek(lba * ISO_SECTOR_SIZE)
    return f.read(ISO_SECTOR_SIZE)


def _parse_iso9660_dir(f, lba: int, size: int, is_bin: bool) -> list[dict]:
    """Return list of {name, lba, size, is_dir} from an ISO9660 directory."""
    entries = []
    remaining = size
    sector_off = 0
    while remaining > 0:
        data = _read_sector(f, lba + sector_off, is_bin)
        pos = 0
        while pos < ISO_SECTOR_SIZE:
            rec_len = data[pos]
            if rec_len == 0:
                break
            name_len = data[pos + 32]
            raw_name = data[pos + 33: pos + 33 + name_len]
            try:
                name = raw_name.decode("ascii", errors="replace").split(";")[0]
            except Exception:
                name = ""
            start = struct.unpack_from("<I", data, pos + 2)[0]
            sz = struct.unpack_from("<I", data, pos + 10)[0]
            flags = data[pos + 25]
            if name not in ("", "\x00", "\x01"):
                entries.append({"name": name, "lba": start, "size": sz, "is_dir": bool(flags & 2)})
            pos += rec_len
        remaining -= ISO_SECTOR_SIZE
        sector_off += 1
    return entries


def extract_ps2_exe(disc_path: Path, out_path: Path) -> Path | None:
    """
    Extract the PS2 main executable from a .bin or .iso disc image.
    Returns the path to the extracted ELF, or None on failure.
    """
    is_bin = _is_bin(disc_path)
    with open(disc_path, "rb") as f:
        # ISO9660 Primary Volume Descriptor at LBA 16
        pvd = _read_sector(f, 16, is_bin)
        if pvd[1:6] != b"CD001":
            return None

        root_lba = struct.unpack_from("<I", pvd, 158)[0]
        root_size = struct.unpack_from("<I", pvd, 166)[0]

        # Find SYSTEM.CNF in root
        root_entries = _parse_iso9660_dir(f, root_lba, root_size, is_bin)
        system_cnf = next((e for e in root_entries if e["name"].upper() == "SYSTEM.CNF"), None)
        if not system_cnf:
            return None

        # Read SYSTEM.CNF to find BOOT2= line
        data = _read_sector(f, system_cnf["lba"], is_bin)[: system_cnf["size"]]
        text = data.decode("ascii", errors="replace")
        boot2 = re.search(r"BOOT2\s*=\s*cdrom0:\\([^;\r\n]+)", text, re.IGNORECASE)
        if not boot2:
            return None
        exe_name = boot2.group(1).strip().split("\\")[-1].split(";")[0]

        # Find the ELF in the filesystem (may be in a subdirectory)
        def find_entry(entries: list[dict], name: str):
            for e in entries:
                if e["name"].upper() == name.upper():
                    return e
                if e["is_dir"]:
                    sub = _parse_iso9660_dir(f, e["lba"], e["size"], is_bin)
                    result = find_entry(sub, name)
                    if result:
                        return result
            return None

        exe_entry = find_entry(root_entries, exe_name)
        if not exe_entry:
            return None

        # Extract the ELF
        out_path.parent.mkdir(parents=True, exist_ok=True)
        with open(out_path, "wb") as out:
            remaining = exe_entry["size"]
            lba = exe_entry["lba"]
            while remaining > 0:
                chunk = _read_sector(f, lba, is_bin)
                write_len = min(remaining, ISO_SECTOR_SIZE)
                out.write(chunk[:write_len])
                remaining -= write_len
                lba += 1

    return out_path
