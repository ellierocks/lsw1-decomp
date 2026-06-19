#!/usr/bin/env python3
"""
PS2 Prototype Analysis Tool

Extracts symbols and subsystem information from the PS2 prototype
and cross-references with the GameCube build.
"""

import re
import struct
from pathlib import Path
from collections import defaultdict

# PS2 prototype paths
PS2_ELF = Path('orig/ps2/ps2-prototype/extracted/SLUS_999.99')
PS2_STRINGS = Path('/tmp/ps2_strings.txt')

# GameCube paths
GC_DOL = Path('orig/GL5E4F/sys/main.dol')
GC_SYMBOLS = Path('config/GL5E4F/symbols.txt')

def extract_ps2_strings():
    """Extract all strings from PS2 ELF."""
    import subprocess
    result = subprocess.run(
        ['strings', '-n', '8', str(PS2_ELF)],
        capture_output=True,
        text=True
    )
    return result.stdout.split('\n')

def categorize_strings(strings):
    """Categorize strings by subsystem."""
    categories = {
        'source_files': [],
        'nu2_functions': [],
        'script_commands': [],
        'ai_commands': [],
        'cutscene_commands': [],
        'debug_strings': [],
        'assert_strings': [],
        'error_strings': [],
        'file_paths': [],
        'subsystem_names': [],
    }
    
    for s in strings:
        s = s.strip()
        if not s:
            continue
            
        # Source file paths
        if s.startswith('../'):
            categories['source_files'].append(s)
            
        # Nu2 engine functions
        elif re.match(r'^(NuFile|NuMem|NuString|NuAnim|NuMtl|NuTex|NuRndr|NuGScn|NuGHG|NuGCut|NuSound|NuFmv|NuDat|NuFPar|NuPP|NuVec|NuAlloc|instNuGCut)', s):
            categories['nu2_functions'].append(s)
            
        # Script commands (uppercase with underscores)
        elif re.match(r'^[A-Z_]{3,}$', s) and not s.startswith('_'):
            if any(cmd in s for cmd in ['TRIGGER', 'CUTSCENE', 'SCRIPT', 'VISIBILITY', 'PARAM']):
                categories['script_commands'].append(s)
                
        # AI commands
        elif s.startswith('AI') or s.startswith('ai_'):
            categories['ai_commands'].append(s)
            
        # Debug strings
        elif any(word in s.lower() for word in ['debug', 'cheat', 'lego option', 'go to scene', 'go to level']):
            categories['debug_strings'].append(s)
            
        # Assert strings
        elif 'assert' in s.lower():
            categories['assert_strings'].append(s)
            
        # Error strings
        elif 'error' in s.lower() or 'failed' in s.lower():
            categories['error_strings'].append(s)
            
        # File paths
        elif '\\' in s or ('/' in s and not s.startswith('../')):
            categories['file_paths'].append(s)
            
        # Subsystem names
        elif re.match(r'^(NuFile|NuMem|NuString|NuAnim|NuMtl|NuTex|NuRndr|NuGScn|NuGHG|NuGCut|NuSound|NuFmv|NuDat|NuFPar|NuPP|NuVec)', s):
            categories['subsystem_names'].append(s)
    
    return categories

def find_string_in_gc(string, gc_data, text_offset, text_addr, text_size):
    """Find a string in the GameCube binary and return its address."""
    try:
        string_bytes = string.encode('ascii') + b'\x00'
    except:
        return None
    
    pos = gc_data.find(string_bytes)
    if pos == -1:
        # Try without null terminator
        string_bytes = string.encode('ascii')
        pos = gc_data.find(string_bytes)
        if pos == -1:
            return None
    
    if pos >= text_offset and pos < text_offset + text_size:
        return text_addr + (pos - text_offset)
    
    return None

def cross_reference_with_gc(categories, gc_data, text_offset, text_addr, text_size):
    """Cross-reference PS2 strings with GameCube binary."""
    results = {
        'found_in_gc': [],
        'not_in_gc': [],
    }
    
    # Check key strings
    key_strings = []
    for cat in ['nu2_functions', 'script_commands', 'ai_commands', 'cutscene_commands', 'debug_strings']:
        key_strings.extend(categories.get(cat, []))
    
    for string in key_strings:
        addr = find_string_in_gc(string, gc_data, text_offset, text_addr, text_size)
        if addr:
            results['found_in_gc'].append((string, addr))
        else:
            results['not_in_gc'].append(string)
    
    return results

def main():
    print("=== PS2 Prototype Analysis ===\n")
    
    # Extract PS2 strings
    print("Extracting strings from PS2 prototype...")
    strings = extract_ps2_strings()
    print(f"Found {len(strings)} strings\n")
    
    # Categorize
    print("Categorizing strings...")
    categories = categorize_strings(strings)
    
    for cat, items in categories.items():
        print(f"  {cat}: {len(items)}")
    print()
    
    # Show key findings
    print("=== Source Files ===")
    for f in sorted(set(categories['source_files']))[:20]:
        print(f"  {f}")
    print()
    
    print("=== Script Commands ===")
    for cmd in sorted(set(categories['script_commands']))[:20]:
        print(f"  {cmd}")
    print()
    
    print("=== AI Commands ===")
    for cmd in sorted(set(categories['ai_commands']))[:20]:
        print(f"  {cmd}")
    print()
    
    print("=== Debug Strings ===")
    for s in sorted(set(categories['debug_strings']))[:20]:
        print(f"  {s}")
    print()
    
    # Cross-reference with GameCube
    print("=== Cross-referencing with GameCube ===")
    
    with open(GC_DOL, 'rb') as f:
        gc_data = f.read()
    
    text_offset = struct.unpack('>I', gc_data[0x00:0x04])[0]
    text_addr = struct.unpack('>I', gc_data[0x48:0x4C])[0]
    text_size = struct.unpack('>I', gc_data[0x90:0x94])[0]
    
    results = cross_reference_with_gc(categories, gc_data, text_offset, text_addr, text_size)
    
    print(f"\nFound {len(results['found_in_gc'])} strings in GameCube binary")
    print(f"Not found: {len(results['not_in_gc'])} strings\n")
    
    print("=== Strings Found in GameCube ===")
    for string, addr in sorted(results['found_in_gc'], key=lambda x: x[1])[:30]:
        print(f"  0x{addr:08X}: {string[:60]}")
    print()
    
    # Save results
    output_file = Path('build/ps2_analysis.txt')
    output_file.parent.mkdir(exist_ok=True)
    
    with open(output_file, 'w') as f:
        f.write("=== PS2 Prototype Analysis ===\n\n")
        
        f.write(f"Total strings: {len(strings)}\n\n")
        
        for cat, items in categories.items():
            f.write(f"=== {cat} ({len(items)}) ===\n")
            for item in sorted(set(items)):
                f.write(f"  {item}\n")
            f.write("\n")
        
        f.write("=== Cross-reference Results ===\n\n")
        f.write(f"Found in GameCube: {len(results['found_in_gc'])}\n")
        for string, addr in sorted(results['found_in_gc'], key=lambda x: x[1]):
            f.write(f"  0x{addr:08X}: {string}\n")
        f.write("\n")
        
        f.write(f"Not in GameCube: {len(results['not_in_gc'])}\n")
        for string in sorted(results['not_in_gc']):
            f.write(f"  {string}\n")
    
    print(f"Results saved to {output_file}")

if __name__ == '__main__':
    main()
