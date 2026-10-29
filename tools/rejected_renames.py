"""Names known to be wrong for a given GC address.

Rename-suggestion tools use this so they do not re-propose a name that was
removed from or rejected for an address. The list is
docs/symbol_donors/rejected_renames.tsv (address, rejected name, current name,
reason); a name is only rejected at the listed address, not globally.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REJECTED_TSV = ROOT / "docs" / "symbol_donors" / "rejected_renames.tsv"


def load_rejected(path: Path = REJECTED_TSV) -> dict[int, set[str]]:
    rejected: dict[int, set[str]] = {}
    if not path.exists():
        return rejected
    for line in path.read_text().splitlines():
        if not line.strip() or line.startswith("#"):
            continue
        cols = line.split("\t")
        if len(cols) < 2:
            continue
        rejected.setdefault(int(cols[0], 16), set()).add(cols[1].strip())
    return rejected
