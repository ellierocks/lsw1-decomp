#!/usr/bin/env python3
"""
Add symbol names based on research documentation.

This script renames symbols that have been identified through
speedrun research, memory watches, and code analysis documented in docs/.

Sources:
- docs/menu_system.md - Menu system, memory watches, and debug menu

Run this after dtk split to apply documented names.
"""

import re
from pathlib import Path


def rename_symbols(symbols_path: Path, renames: dict) -> int:
    """Rename symbols in symbols.txt based on address matching."""
    if not symbols_path.exists():
        print(f"Error: {symbols_path} not found")
        return 0

    lines = []
    renamed = 0

    with open(symbols_path, 'r') as f:
        for line in f:
            original = line.rstrip('\n')
            stripped = original.strip()

            if not stripped or stripped.startswith('//'):
                lines.append(original)
                continue

            match = re.match(r'(\w+)\s*=\s*([^;]+);\s*//\s*(.*)', stripped)
            if match:
                name, value, comment = match.group(1), match.group(2).strip(), match.group(3)

                if value in renames and name != renames[value]:
                    new_name = renames[value]
                    print(f"{name} -> {new_name}")
                    renamed += 1
                    lines.append(f"{new_name} = {value}; // {comment}")
                else:
                    lines.append(original)
            else:
                lines.append(original)

    with open(symbols_path, 'w') as f:
        f.write('\n'.join(lines) + '\n')

    return renamed


if __name__ == '__main__':
    symbols_file = Path('config/GL5E4F/symbols.txt')

    # Symbols renamed based on documentation
    # Format: "section:address" -> new_name
    renames = {
        # Input state (docs/menu_system.md)
        '.bss:0x802292C0': 'gInputState',

        # Stud counters (docs/menu_system.md)
        '.bss:0x803E049C': 'gTotalStudsBalance',
        '.sbss:0x8040B1AC': 'gPlayerTotalStuds',

        # Progress counters (docs/menu_system.md)
        '.bss:0x803E029A': 'gSuperkitTotal',
        '.sbss:0x8040B1A4': 'gMinikitTotal',
        '.sbss:0x8040B1A8': 'gTrueJediTotal',

        # Menu system (docs/menu_system.md)
        '.text:0x80078D08': 'Menu_RegisterDescriptors',

        # Format strings (docs/menu_system.md)
        '.rodata:0x801A5450': 'str_DebugMenu_FpsFormat',
        '.rodata:0x801A5440': 'str_LoadingFormat',

        # Utility functions (from __start analysis)
        '.text:0x80155D54': 'MemZero',

        # Menu callback functions (docs/menu_system.md)
        '.text:0x800B3378': 'Menu_Main_BuildOptions',
        '.text:0x800B326C': 'Menu_Main_HandleSelection',
        '.text:0x800B11B4': 'Menu_Unknown1_BuildOptions',
        '.text:0x800B0DB8': 'Menu_Unknown1_HandleSelection',
        '.text:0x800B26BC': 'Menu_Unknown3_BuildOptions',
        '.text:0x800B30A0': 'Menu_Unknown3_HandleSelection',
        '.text:0x800B2F60': 'Menu_Unknown4_Enter',
        '.text:0x800B08D8': 'Menu_Unknown4_BuildOptions',
        '.text:0x800B05CC': 'Menu_Unknown4_HandleSelection',
        '.text:0x800B2FB8': 'Menu_Unknown4_Close',
        '.text:0x800B0C70': 'Menu_Unknown5_BuildOptions',
        '.text:0x800B0B64': 'Menu_Unknown5_HandleSelection',
        '.text:0x800B3414': 'Menu_Unknown6_BuildOptions',
        '.text:0x800B1318': 'Menu_Unknown6_HandleSelection',
        '.text:0x800B2FD8': 'Menu_Unknown9_Enter',
        '.text:0x800B1CA4': 'Menu_Unknown9_BuildOptions',
        '.text:0x800B1B74': 'Menu_Unknown9_HandleSelection',
        '.text:0x800B34A4': 'Menu_Unknown10_BuildOptions',
        '.text:0x800B320C': 'Menu_Unknown10_HandleSelection',
        '.text:0x800B3434': 'Menu_Unknown11_BuildOptions',
        '.text:0x800B3134': 'Menu_Unknown11_HandleSelection',
        '.text:0x800B359C': 'Menu_Unknown12_BuildOptions',
        '.text:0x800B299C': 'Menu_Unknown12_HandleSelection',
        '.text:0x800B2ADC': 'Menu_Unknown13_BuildOptions',
        '.text:0x800B3648': 'Menu_Unknown13_HandleSelection',
        '.text:0x800B3668': 'Menu_Unknown14_Enter',
        '.text:0x800B36A8': 'Menu_Unknown14_BuildOptions',
        '.text:0x800B3688': 'Menu_Unknown14_HandleSelection',
        '.text:0x800B36CC': 'Menu_Unknown14_Close',
        '.text:0x800B36C8': 'Menu_Unknown15_Enter',
        '.text:0x800B36EC': 'Menu_Unknown15_BuildOptions',
        '.text:0x800B370C': 'Menu_Unknown16_BuildOptions',
        '.text:0x800B2BB4': 'Menu_Unknown16_HandleSelection',
        '.text:0x800B3754': 'Menu_Unknown17_BuildOptions',
        '.text:0x800B3734': 'Menu_Unknown17_HandleSelection',

        # Player pointers (docs/menu_system.md)
        '.data:0x801D5338': 'gPlayer1Pointer',
        '.data:0x801D533C': 'gPlayerPointers',

        # Camera state (docs/menu_system.md)
        '.bss:0x803E01F4': 'gCameraState',
        '.bss:0x803E027A': 'gCameraAngles',

        # Menu strings (docs/menu_system.md)
        '.rodata:0x801A0188': 'str_MenuSelect',
        '.rodata:0x801A0340': 'str_MenuBack',
        '.rodata:0x801A0348': 'str_MenuNoEntry',
        '.rodata:0x801A0350': 'str_ExitDemo',
        '.rodata:0x801A0358': 'str_MenuMove',

        # Common UI strings
        '.rodata:0x801A00BC': 'str_Yes',
        '.rodata:0x801A00C4': 'str_No',
        '.rodata:0x801A00EC': 'str_Confirm',
        '.rodata:0x801A00F8': 'str_Cancel',
        '.rodata:0x801A0110': 'str_On',
        '.rodata:0x801A0114': 'str_Off',
        '.rodata:0x801A0128': 'str_Enabled',
        '.rodata:0x801A0134': 'str_Disabled',
        '.rodata:0x801A0150': 'str_Select',
        '.rodata:0x801A01D0': 'str_Mode',
        '.rodata:0x801A0200': 'str_Player',
        '.rodata:0x801A0204': 'str_FPS',
        '.rodata:0x801A0310': 'str_OK',
        '.rodata:0x801A0314': 'str_End',
        '.rodata:0x801A0318': 'str_Back',
        '.rodata:0x801A031C': 'str_No2',
        '.rodata:0x801A0320': 'str_MenuMove2',
        '.rodata:0x801A0344': 'str_menuBack',
        '.rodata:0x801A037C': 'str_menuMove',
        '.rodata:0x801A03A4': 'str_menuBack2',

        # Extras (docs/menu_system.md)
        '.data:0x801D5996': 'gExtrasFlags',
    }

    renamed = rename_symbols(symbols_file, renames)
    print(f"\nRenamed {renamed} symbols")
