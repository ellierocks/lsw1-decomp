# LSW1 Revision Graph

Evidence-driven revision map. All edges carry explicit evidence and confidence levels.
Disconnected nodes indicate builds with no measured relationship yet.

---

## Build Registry

| Build ID | Platform | Region | Variant | Version | Date | Available |
|----------|----------|--------|---------|---------|------|-----------|
| `gc_uk_retail` | gc | uk | retail | GL5P4F | 2005-04 | ✓ |
| `gc_us_retail` | gc | us | retail | GL5E4F | 2005-04 | ✓ |
| `mac_demo_dbg` | mac | us | demo |  | 2005 | ✓ |
| `pc_us_demo` | pc | us | demo |  | 2005 | ✓ |
| `pc_us_retail` | pc | us | retail |  | 2005-04 | ✓ |
| `ps2_eu_demo` | ps2 | eu | demo |  | 2005 | ✓ |
| `ps2_eu_retail_v1` | ps2 | eu | retail | 1.0 | 2005-04 | ✓ |
| `ps2_prototype` | ps2 | us | prototype |  | 2005-01-10 | ✓ |
| `ps2_us_retail_gh` | ps2 | us | retail | 2.0 | 2005 | ✓ |
| `ps2_us_retail_v1` | ps2 | us | retail | 1.01 | 2005-04 | ✓ |
| `xbox_demo_oxm045` | xbox | us | demo |  | 2005-06 | ✓ |
| `xbox_retail` | xbox | us | retail |  | 2005-04 | ✓ |

---

## Verified Payload Clusters

Builds confirmed to carry identical payloads (same LEGO Star Wars data).

### PS2 US Kiosk Demo Q2-Q3 2005
- **Members**: `ps2_us_kiosk_demo`
- **Carriers**: OPS2M 107, Jampack Vol 12, Kiosk Q2-Q3 2005
- **Evidence**: Verified: same LEGO Star Wars data size across all three carriers.
- **Confidence**: high

### PS2 EU Demo Family
- **Members**: `ps2_eu_demo`, `ps2_eu_bonus9_demo`
- **Carriers**: Bonus Demo 9 EU, OPS2M French Christmas Special, OPS2M Special Edition 20, OPS2M 59, OPS2M 62, OPS2M 79
- **Evidence**: Verified: same LEGO Star Wars data size across all listed carriers. Byte identity vs ps2_eu_demo not yet confirmed.
- **Confidence**: medium

---

## Revision Relationships

| From | To | Relationship | Confidence | Score | Evidence |
|------|----|-------------|------------|-------|----------|
| `ps2_prototype` | `ps2_eu_demo` | precedes | medium | `19.2%` | Positional byte match 19.2% (shared ~565K/2.94MB bytes). Prototype predates demo by date. |
| `ps2_prototype` | `ps2_eu_retail_v1` | precedes | medium | `19.3%` | Positional byte match 19.3% (567K/2.94MB). |
| `ps2_prototype` | `ps2_us_retail_v1` | precedes | medium | `19.3%` | Positional byte match 19.3% (567K/2.94MB). |
| `ps2_prototype` | `ps2_us_retail_gh` | precedes | medium | `19.0%` | Positional byte match 19.0% (559K/2.94MB). |
| `ps2_eu_demo` | `ps2_eu_retail_v1` | closely_related | high | `31.3%` | Positional byte match 31.3% (993K/3.17MB). Similar size and string count; same SLED/SLES family. |
| `ps2_eu_demo` | `ps2_us_retail_v1` | closely_related | high | `31.0%` | Positional byte match 31.0% (982K/3.17MB). |
| `ps2_eu_retail_v1` | `ps2_us_retail_v1` | closely_related | high | `43.0%` | Positional byte match 43.0% (1.36MB/3.17MB). Highest similarity in PS2 family; likely same codebase,... |
| `ps2_us_retail_v1` | `ps2_us_retail_gh` | diverged | high | `27.3%` | Positional byte match 27.3% (866K/3.17MB). Significant executable differences; cause undetermined. |
| `ps2_eu_retail_v1` | `ps2_us_retail_gh` | diverged | high | `27.4%` | Positional byte match 27.4% (868K/3.17MB). |
| `xbox_demo_oxm045` | `xbox_retail` | precedes | low | – | Demo predates retail by date. No byte comparison yet; different file layout (demo is loose files, re... |
| `pc_us_demo` | `pc_us_retail` | precedes | medium | – | Exe dates: demo 2005-01-13, retail 2005-03-15. Both x86 PE; string comparison pending. |
| `gc_us_retail` | `ps2_us_retail_v1` | same_game_codebase | high | – | String Jaccard 0.41 (4059 shared strings). Same Nu2 engine; different CPU arch (PPC vs MIPS). |
| `gc_us_retail` | `gc_uk_retail` | closely_related | high | – | Both GL5xF GC builds; string overlap expected; byte comparison pending. |

---

## Similarity Matrix (measured)

| Build A | Build B | Method | Score | Matched | Total |
|---------|---------|--------|-------|---------|-------|
| `ps2_eu_retail_v1` | `ps2_us_retail_v1` | key_string_jaccard | `1.000` | 435 | 435 |
| `gc_us_retail` | `gc_uk_retail` | key_string_jaccard | `0.995` | 427 | 429 |
| `ps2_eu_demo` | `ps2_us_retail_gh` | key_string_jaccard | `0.993` | 435 | 438 |
| `ps2_eu_retail_v1` | `ps2_us_retail_v1` | string_jaccard | `0.989` | 4696 | 4750 |
| `ps2_eu_demo` | `ps2_us_retail_gh` | string_jaccard | `0.988` | 4712 | 4770 |
| `gc_us_retail` | `gc_uk_retail` | string_jaccard | `0.983` | 4633 | 4711 |
| `xbox_retail` | `xbox_demo_oxm045` | key_string_jaccard | `0.971` | 165 | 170 |
| `xbox_retail` | `xbox_demo_oxm045` | string_jaccard | `0.959` | 3642 | 3799 |
| `ps2_eu_demo` | `ps2_us_retail_v1` | string_jaccard | `0.913` | 4518 | 4947 |
| `ps2_eu_demo` | `ps2_eu_retail_v1` | string_jaccard | `0.913` | 4511 | 4940 |
| `ps2_us_retail_v1` | `ps2_us_retail_gh` | string_jaccard | `0.912` | 4520 | 4957 |
| `ps2_eu_retail_v1` | `ps2_us_retail_gh` | string_jaccard | `0.911` | 4510 | 4953 |
| `ps2_prototype` | `ps2_eu_retail_v1` | key_string_jaccard | `0.879` | 392 | 446 |
| `ps2_prototype` | `ps2_us_retail_v1` | key_string_jaccard | `0.879` | 392 | 446 |
| `pc_us_demo` | `pc_us_retail` | key_string_jaccard | `0.845` | 1124 | 1330 |
| `ps2_prototype` | `ps2_eu_retail_v1` | string_jaccard | `0.769` | 3810 | 4955 |
| `ps2_prototype` | `ps2_us_retail_v1` | string_jaccard | `0.767` | 3810 | 4969 |
| `ps2_prototype` | `ps2_eu_demo` | string_jaccard | `0.709` | 3645 | 5139 |
| `ps2_prototype` | `ps2_us_retail_gh` | string_jaccard | `0.708` | 3646 | 5150 |
| `ps2_eu_demo` | `ps2_eu_retail_v1` | key_string_jaccard | `0.696` | 357 | 513 |
| `ps2_eu_demo` | `ps2_us_retail_v1` | key_string_jaccard | `0.696` | 357 | 513 |
| `ps2_eu_retail_v1` | `ps2_us_retail_gh` | key_string_jaccard | `0.692` | 357 | 516 |
| `ps2_us_retail_v1` | `ps2_us_retail_gh` | key_string_jaccard | `0.692` | 357 | 516 |
| `pc_us_demo` | `pc_us_retail` | string_jaccard | `0.633` | 10409 | 16451 |
| `ps2_prototype` | `ps2_eu_demo` | key_string_jaccard | `0.608` | 317 | 521 |
| `ps2_prototype` | `ps2_us_retail_gh` | key_string_jaccard | `0.605` | 317 | 524 |
| `gc_uk_retail` | `ps2_us_retail_gh` | string_jaccard | `0.584` | 3469 | 5939 |
| `gc_uk_retail` | `ps2_eu_demo` | string_jaccard | `0.584` | 3463 | 5933 |
| `gc_us_retail` | `ps2_us_retail_gh` | string_jaccard | `0.583` | 3473 | 5957 |
| `gc_us_retail` | `ps2_eu_demo` | string_jaccard | `0.582` | 3465 | 5953 |
| `gc_uk_retail` | `ps2_us_retail_v1` | string_jaccard | `0.582` | 3454 | 5937 |
| `gc_uk_retail` | `ps2_eu_retail_v1` | string_jaccard | `0.581` | 3446 | 5931 |
| `gc_us_retail` | `ps2_us_retail_v1` | string_jaccard | `0.581` | 3458 | 5955 |
| `gc_us_retail` | `ps2_eu_retail_v1` | string_jaccard | `0.579` | 3448 | 5951 |
| `ps2_eu_demo` | `xbox_retail` | string_jaccard | `0.524` | 2920 | 5567 |
| `gc_uk_retail` | `xbox_retail` | string_jaccard | `0.524` | 2894 | 5519 |
| `ps2_us_retail_v1` | `xbox_retail` | string_jaccard | `0.524` | 2918 | 5564 |
| `ps2_us_retail_gh` | `xbox_retail` | string_jaccard | `0.524` | 2922 | 5577 |
| `ps2_eu_retail_v1` | `xbox_retail` | string_jaccard | `0.523` | 2909 | 5559 |
| `gc_us_retail` | `xbox_retail` | string_jaccard | `0.523` | 2897 | 5538 |

---

## Open Questions

- Are `ps2_eu_bonus9_demo` and `ps2_eu_demo` byte-identical?
- Are `xbox_demo_oxm045` and `xbox_demo_oxm061` byte-identical?
- Which build does `ps2_ops2m_93_demo` correspond to?
- What is the relationship between `ps2_us_kiosk_demo` and `ps2_eu_demo`?
- Does `mac_demo` share the same build date as `mac_demo_dbg`?
- What changed between `ps2_us_retail_v1` and `ps2_us_retail_gh`?
- What is the build date of the Xbox retail?
