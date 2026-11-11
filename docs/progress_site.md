# Progress website

The progress website is a separate Next.js application (`lsw1-progress`) deployed on Vercel.
This repository owns the public statistics export and the build evidence behind it.

## Publication

The `retail` job exports the `dashboard-progress` artifact after `bash build.sh` and the
verifier regression tests succeed. The `progress` job runs only for main pushes or main
workflow dispatches, after `tooling` and `retail` succeed. It combines the exported
snapshot with the existing history and pushes `progress.json` to the `progress-data` branch.

That branch contains only the JSON dataset. Publication does not switch branches, stage
files or alter the source checkout. It uses the workflow's repository-scoped token with
`contents: write` and never force-pushes. Concurrent publishers are serialized, and older
reports are rejected. No Vercel secret or cross-repository token is needed.

The site reads the raw public JSON over HTTPS, with a one-minute server cache and browser
polling. When the feed is missing, the site says so; its bundled snapshot is not presented
as live data. Frontend changes deploy independently through Vercel's Git integration.

## Export locally

```sh
python3 tools/export_progress.py --output work/progress.json
python3 tools/export_progress.py --output work/progress.json --sweep work/sweep/results.json
```

The exporter reads the existing report; it does not build or sweep. Pass
`--revision FULL_SHA` only for a report built from that exact checkout. CI supplies the
source SHA and the Actions run URL. A local export without that evidence has a null
revision, and `publish_progress.py` refuses to publish it.

Units and their functions are listed in load-address order, taken from objdiff's
`virtual_address` metadata, so `.init` code comes before `.text`. Addresses are used for
ordering only and are not exported. Entries without an address keep report order at the end.

Exported fields are allowlisted: aggregate counts, unit names and categories, function
names, sizes and objdiff similarities. Optional sweep statistics include names, units,
sizes, verdict counts and exact-instruction similarity. Section data, instructions, object
paths, raw logs and proprietary artifacts are never copied.

Normal CI builds do not run a sweep. Their candidate dataset is null, which the site labels
as unavailable; an empty exported sweep is a different state. Published candidate scores
are leads, not verification or ownership claims.

## History and metrics

Schema version 1 has `latest` and `history`. History records one aggregate point per source
revision, and a rerun replaces that revision's point. Main builds keep earlier points, so
the website needs no database.

Byte coverage is the main progress measure. Objdiff matches, strict function verification
and promoted units are distinct. Complete-unit figures come directly from the report and
can include data-only ranges. The website suppresses recent-gain comparisons when code or
function totals change. It does not estimate an ETA or invent history from before
publication began.

## Checks

```sh
python3 -m unittest tests.test_export_progress tests.test_publish_progress
```

The publisher test pushes two snapshots through a temporary bare Git remote and checks
that the source branch, uncommitted files and history are preserved. The exporter tests
cover the public allowlist, malformed metrics, provenance and candidate filtering. The
frontend repository owns schema validation, statistics tests and its production build.
