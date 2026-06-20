# Inspection: `fn_8009CC00`

This is an evidence-only inspection of `fn_8009CC00` (`0x8009CC00`, size
`0x14C`). It does not rename the function or assert a runtime boot sequence.
Callsite order below is static instruction order inside this one function.

## Direct caller

The direct-call export has one caller:

| Caller | Caller address | Callsite | Evidence |
|---|---:|---:|---|
| `Menu_FullReset` | `0x800A0844` | `0x800A0870` | `0x4BFFC391`, target `0x8009CC00` |

This is a HIGH-confidence direct `bl`/`bla` edge. The caller name is not proof
of menu-reset behavior.

## Direct callees in static callsite order

| Callsite | Callee | Address | Platform/Nu label | Confidence / note |
|---:|---|---:|---|---|
| `0x8009CC1C` | `fn_800055EC` | `0x800055EC` | unknown | HIGH edge; generic callee. |
| `0x8009CC30` | `fn_80038628` | `0x80038628` | generic platform-setup candidate | HIGH edge; its own pack records direct OS/PAD/GX calls. |
| `0x8009CC48` | `Menu_InitDefinitions` | `0x800B3D88` | named menu-family symbol | HIGH edge; menu classification is name-only. |
| `0x8009CC7C` | `fn_8011A83C` | `0x8011A83C` | unknown | HIGH edge; generic callee. |
| `0x8009CCD0` | `NuInitHardware` | `0x80038A98` | Nu engine | HIGH edge; named Nu symbol. |
| `0x8009CCD4` | `NuCameraCreate` | `0x80023CD0` | Nu engine | HIGH edge; named Nu symbol; camera role is name-only. |
| `0x8009CCDC` | `fn_800D9784` | `0x800D9784` | unknown | HIGH edge; generic callee. |
| `0x8009CCE0` | `fn_8009E174` | `0x8009E174` | unknown | HIGH edge; generic callee. |
| `0x8009CCE4` | `fn_8011E864` | `0x8011E864` | unknown | HIGH edge; generic callee. |
| `0x8009CCEC` | `fn_8011EB70` | `0x8011EB70` | unknown | HIGH edge; generic callee. |
| `0x8009CCF0` | `fn_80027348` | `0x80027348` | unknown | HIGH edge; generic callee. |

The edge rows and PPC instruction evidence are in
`task_packs/fn_8009CC00/direct_callees.tsv`.

## Static sequence around the two initialization-related callees

- Before `fn_80038628`: `fn_800055EC`.
- Between `fn_80038628` and `NuInitHardware`: `Menu_InitDefinitions` and
  `fn_8011A83C`.
- After `NuInitHardware`: `NuCameraCreate`, `fn_800D9784`, `fn_8009E174`,
  `fn_8011E864`, `fn_8011EB70`, and `fn_80027348`.

`fn_80038628` and `NuInitHardware` are sibling direct callees. There is no
direct edge between them in the export. Their static callsite positions do not
prove that one executes before, inside, or after the other at runtime.

## Data and string evidence

No indexed `gc_data_xrefs.tsv` or `gc_dol_string_renames.tsv` row names
`fn_8009CC00`. Its extracted assembly nevertheless directly references these
raw labels, without an assigned meaning in this note:

`lbl_80408E58`, `lbl_8040AF54`, `lbl_8040AF58`, `lbl_80408F68`,
`lbl_80408F8C`, `lbl_80407E08`, `lbl_804093AC`, `lbl_803D9A58`,
`lbl_80408E68`, `lbl_8040AF5C`, `lbl_80409410`, `lbl_8019E5A8`,
`lbl_8019E5AC`, `lbl_8019E5B0`, `lbl_8019E5B4`, `lbl_80408E9C`,
`lbl_80408EA0`, `lbl_80408E98`, `lbl_8019E5B8`, and `lbl_8040835C`.

## Nearby symbols

`fn_8009CB2C`, `fn_8009CB88`, `fn_8009CBC4`, `fn_8009CC00`,
`fn_8009CD4C`, `fn_8009CF2C`, and `fn_8009CF30` are the adjacent `.text`
symbols in `symbols.txt`. Proximity alone provides no role evidence.

## Assessment and unknowns

The function directly calls a generic platform-setup candidate, a named menu
symbol, and named Nu engine symbols. That is sufficient to call it an
**orchestration candidate**, but insufficient to classify it as main startup,
game init, level init, or SDK glue. The sole direct caller is named
`Menu_FullReset`; this is relevant name evidence but does not establish its
behavior.

Unknowns include indirect calls, runtime control-flow conditions, arguments,
the roles of seven generic direct callees, and the meanings of the raw global
labels.

## Next functions to inspect

1. `fn_800055EC` — the sole direct call before `fn_80038628`.
2. `fn_8011A83C` — direct call between `Menu_InitDefinitions` and
   `NuInitHardware`.
3. `Menu_FullReset` — the sole direct caller, to establish the surrounding
   caller context.
4. `fn_800D9784` and `fn_8009E174` — first generic calls after the named Nu
   initialization/camera calls.
5. `fn_8011E864`, `fn_8011EB70`, and `fn_80027348` — remaining generic direct
   calls in the static sequence.

## LOW-confidence, unaccepted rename ideas

- `MenuResetOrchestrator`
- `StartupOrMenuInit`

These are not justified by the present evidence. Keep `fn_8009CC00` unchanged
until caller context, generic callee roles, and data-label usage are resolved.

## Confidence and limitations

- HIGH: direct caller/callee edges with recorded PPC `bl`/`bla` instructions.
- MED: `fn_80038628` platform-call context from its separate direct-edge pack.
- LOW: name-family labels and all rename ideas.
- Direct edges do not include indirect calls and do not prove temporal order,
  entrypoint status, or a function's semantic role.
