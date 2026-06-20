# `Menu_FullReset` early static phase after `fn_8009CC00`

This report covers static direct-callee positions 2–21 in `Menu_FullReset`:
the first 20 exported direct callsites after `fn_8009CC00`. It is instruction
layout evidence only and does not establish dynamic runtime order.

## Static callsite table

| Static # | Callsite | Callee | Address | Label | Indexed data refs | Evidence |
|---:|---:|---|---:|---|---:|---|
| 2 | `0x800A0874` | `fn_8009D23C` | `0x8009D23C` | generic | 7 | `0x4BFFC9C9` |
| 3 | `0x800A088C` | `fn_80071E0C` | `0x80071E0C` | generic | 0 | `0x4BFD1581` |
| 4 | `0x800A08E8` | `fn_800B43F0` | `0x800B43F0` | generic | 3 | `0x48013B09` |
| 5 | `0x800A0910` | `fn_8006DF1C` | `0x8006DF1C` | generic | 0 | `0x4BFCD60D` |
| 6 | `0x800A092C` | `fn_8006C748` | `0x8006C748` | generic | 8 | `0x4BFCBE1D` |
| 7 | `0x800A0960` | `fn_80091FC0` | `0x80091FC0` | generic | 0 | `0x4BFF1661` |
| 8 | `0x800A0978` | `fn_8008EE48` | `0x8008EE48` | generic | 0 | `0x4BFEE4D1` |
| 9 | `0x800A0984` | `fn_800C9E44` | `0x800C9E44` | generic | 4 | `0x480294C1` |
| 10 | `0x800A099C` | `fn_8009E6FC` | `0x8009E6FC` | generic | 0 | `0x4BFFDD61` |
| 11 | `0x800A09A4` | `fn_800274BC` | `0x800274BC` | generic | 0 | `0x4BF86B19` |
| 12 | `0x800A09BC` | `AXFXSetHooks` | `0x8002765C` | audio/platform-named | 0 | `0x4BF86CA1` |
| 13 | `0x800A09C0` | `fn_80027564` | `0x80027564` | generic | 0 | `0x4BF86BA5` |
| 14 | `0x800A09C8` | `NuRndrFootPrints` | `0x80025A70` | Nu rendering-named | 0 | `0x4BF850A9` |
| 15 | `0x800A09E0` | `fn_8014C20C` | `0x8014C20C` | generic | 2 | `0x480AB82D` |
| 16 | `0x800A09E4` | `fn_800CC970` | `0x800CC970` | generic | 0 | `0x4802BF8D` |
| 17 | `0x800A0A68` | `fn_8009D5D0` | `0x8009D5D0` | generic | 0 | `0x4BFFCB69` |
| 18 | `0x800A0A7C` | `fn_8009E6FC` | `0x8009E6FC` | generic | 0 | `0x4BFFDC81` |
| 19 | `0x800A0A84` | `fn_800274BC` | `0x800274BC` | generic | 0 | `0x4BF86A39` |
| 20 | `0x800A0A9C` | `AXFXSetHooks` | `0x8002765C` | audio/platform-named | 0 | `0x4BF86BC1` |
| 21 | `0x800A0AA0` | `fn_80027564` | `0x80027564` | generic | 0 | `0x4BF86AC5` |

All table rows are HIGH-confidence direct `bl`/`bla` call edges. Labels other
than `generic` are name-family labels only.

## Category summary

- Menu-named direct callees: none in this 20-callsite window.
- Audio/platform-named direct callsites: `AXFXSetHooks` twice.
- Nu rendering-named direct callsites: `NuRndrFootPrints` once.
- Generic/unrecovered direct callsites: 17.

## Indexed data/string references

Existing `gc_data_xrefs.tsv` rows are available for `fn_8009D23C`,
`fn_800B43F0`, `fn_8006C748`, `fn_800C9E44`, and `fn_8014C20C`; the counts
are recorded in the table. The referenced labels are not interpreted here.
No matching row for these callees was found in the inspected
`gc_dol_string_renames.tsv` table.

## Recommended next generic targets

1. `fn_8009D23C` — first post-`fn_8009CC00` callee and seven indexed data
   references.
2. `fn_8006C748` — static position 6 and eight indexed data references.
3. `fn_800B43F0` — static position 4 and three indexed data references.
4. `fn_800C9E44` — static position 9 and four indexed data references.
5. `fn_8014C20C` — immediately after the first named Nu-rendering call and
   has two indexed data references.

## What this says about `fn_8009CC00`

`fn_8009CC00` is statically followed by a mixed generic/audio/Nu-rendering
call region inside `Menu_FullReset`. This is consistent with it occurring
before that static region, but does not show that it is setup-before-menu-work:
there is no menu-named direct callee in this window, and static callsite order
is not dynamic execution proof. It should not be renamed or accepted as a
menu reset/init helper on this evidence alone.

## Limitations

- Direct `bl`/`bla` edges only; indirect calls are absent.
- Data-reference counts are existing indexed rows, not semantic labels.
- No dynamic order, condition, argument, or role inference is made.
