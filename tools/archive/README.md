# tools/archive

One-shot and superseded analysis scripts kept for reference.
These are not part of the active workflow — see `AGENTS.md` for the current toolset.

| Script | Superseded by |
|--------|--------------|
| `crashwoc_fn_match.py` | `tools/nu2_fn_match.py` (handles CrashWOC + Nemo) |
| `lineage_analysis.py` | `tools/lsw1_cross_binary_lineage.py` (pipeline stage) |
| `deep_mac_string_matcher.py` | `tools/mac_anchor_rename_queue.py` (pipeline stage) |
| `second_pass_mac_matcher.py` | `tools/mac_anchor_rename_queue.py` |
| `mac_string_xref_scanner.py` | `tools/gc_data_xref_scanner.py` (pipeline stage) |
| `match_mac_data_to_gc.py` | `tools/gc_data_xref_scanner.py` |
| `extract_mac_symbols.py` | `tools/mac_anchor_rename_queue.py` |
| `ps2_prototype_analysis.py` | `tools/lsw1_cross_binary_lineage.py` |
| `fingerprint_matcher.py` | — (one-shot, findings recorded in `docs/symbol_donors/fingerprint_matches.md`) |
| `generate_final_summary.py` | — (one-shot summary; pipeline index replaces this) |
