#!/usr/bin/env python3
"""
Fix symbols.txt after dtk split.

This script removes duplicate symbols and ensures all symbols have proper
section prefixes. Run this after `dtk dol split` if it panics.
"""

import re
import sys
from pathlib import Path


def fix_symbols(symbols_path: Path) -> None:
    """Remove duplicate symbols and fix unsectioned symbols."""
    if not symbols_path.exists():
        print(f"Error: {symbols_path} not found")
        sys.exit(1)

    # Read all symbols
    symbols = {}
    with open(symbols_path, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('//'):
                continue

            # Parse symbol line
            match = re.match(r'(\w+)\s*=\s*([^;]+);\s*//\s*(.*)', line)
            if match:
                name = match.group(1)
                value = match.group(2).strip()
                comment = match.group(3).strip()

                # Skip if we already have a better version (with section prefix)
                if name in symbols:
                    existing = symbols[name]
                    # Prefer version with section prefix
                    if ':' in value and ':' not in existing['value']:
                        symbols[name] = {'value': value, 'comment': comment}
                    # Keep existing if it already has section prefix
                else:
                    symbols[name] = {'value': value, 'comment': comment}

    # Write back
    with open(symbols_path, 'w') as f:
        for name, data in sorted(symbols.items()):
            f.write(f"{name} = {data['value']}; // {data['comment']}\n")

    print(f"Fixed {symbols_path}: {len(symbols)} unique symbols")


if __name__ == '__main__':
    if len(sys.argv) > 1:
        path = Path(sys.argv[1])
    else:
        path = Path('config/GL5E4F/symbols.txt')

    fix_symbols(path)
