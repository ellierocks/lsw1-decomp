# Archaeology: `xbox_demo_oxm045` vs `xbox_retail`

**demo  2005-06**  →  **retail  2005-04**

## Executable

| | `xbox_demo_oxm045` | `xbox_retail` |
|-|---------|---------|
| Size | 2,125,824 bytes | 2,179,072 bytes |
| Delta | | +53,248 bytes |

## String Overview

| Category | `xbox_demo_oxm045` | `xbox_retail` | Only in A | Only in B |
|----------|---------|---------|-----------|-----------|
| all | 3689 | 3752 | 47 | 110 |
| source_paths | 19 | 19 | 0 | 0 |
| nu_debug | 8 | 8 | 0 | 0 |
| assert_error | 52 | 53 | 0 | 1 |

## Assert / Error Message Changes

**Added:**
- `NTLCFDDSF: Unable to create cube texture '%s'`