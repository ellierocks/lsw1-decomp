# CI and Progress Reporting

Two forges run CI for this repository. Forgejo is primary; the GitHub mirror runs
the jobs that feed the public decomp.dev progress hub. The branch and PR process is in
[development workflow](development_workflow.md).

## Jobs

| Job | Forge | Trigger | Needs `ORIG_KEY` | Output |
|-----|-------|---------|------------------|--------|
| `tooling` (`build.yml`) | Forgejo, GitHub | push to `main`, PRs | no | Unit tests, configure check, map check, shell syntax |
| `retail` (`build.yml`) | Forgejo | push to `main`, PRs | yes | Decrypts and hash-checks the DOL, runs `bash build.sh` |
| `readme` (`build.yml`) | Forgejo | successful main push or dispatched update branch | no | Publishes and merges verified README progress PRs |
| `build` (`build.yml`) | GitHub | push to `main`, PRs | yes | Builds the patched DTK and report, uploads `GL5E4F_report` |
| `build-map` (`generate-map.yml`) | GitHub | changes to symbols or map tooling | no | Uploads `GL5E4F-dolphin-map` |

`build.yml` picks the retail job by server: `retail` runs when the server is not
github.com, and `build` runs only on github.com. Forgejo's `retail` job fails if
`ORIG_KEY` is missing. GitHub's `build` job skips on fork PRs, where secrets are
unavailable.

The `retail` and `build` jobs both decrypt `orig/GL5E4F/sys/main.dol.enc`. The
encrypted blob is committed; the plaintext DOL never is.

## decomp.dev reporting

The matching metric decomp.dev shows is the **objdiff matched-code percentage**
(per-function instruction matching). This is distinct from the symbol-naming percentages
in the README status table.

- The `build` job runs `ninja build/GL5E4F/report.json` and uploads it as
  **`GL5E4F_report`**, the `<VERSION>_report` name decomp.dev expects.
- decomp.dev reads that artifact from the latest successful run on `main`.
- The full DOL link does not yet succeed, so the report targets `report.json`
  directly. It is generated from per-unit objects and does not need a complete link.
- The patched DTK tolerates this ProDG binary's exception tables. It is built from a
  pinned commit by `tools/build_patched_dtk.sh`, with patches in `tools/patches/`.

## One-time GitHub setup

These steps need repository-admin access. The encrypted DOL is already committed.

1. Set the `ORIG_KEY` secret: `gh secret set ORIG_KEY`, or in the repository's
   **Settings → Secrets and variables → Actions**.
2. Register the repository at <https://decomp.dev/manage/new>.
3. Install the **decomp.dev GitHub App** on the repository or organization. It sends
   workflow-completion webhooks and can post progress-diff comments on PRs.

Once reports land on `main`, decomp.dev tracks the project automatically.

## Forgejo setup

Set the `ORIG_KEY` secret in the repository's Actions settings on Forgejo. The retail job
does not run without it.

## Local equivalents

```sh
python3 tools/orig_crypt.sh decrypt   # with ORIG_KEY set, restores orig/GL5E4F/sys/main.dol
bash build.sh                         # the retail job's steps, locally
python3 tools/progress.py --matching  # matched-code summary from the report
```

## Rotating the key

Re-encrypt with the new passphrase, commit the new `main.dol.enc`, then update
`ORIG_KEY` on both forges:

```sh
ORIG_KEY='<new-passphrase>' bash tools/orig_crypt.sh encrypt
```

`tools/orig_crypt.sh` verifies the DOL SHA-1 (`95cca08a…`) before encrypting and after
decrypting, so a wrong key or wrong DOL fails loudly. Keep the passphrase in a password
manager; never print it in logs or commit it.

## Automatic README progress

After the Forgejo `tooling` and `retail` checks succeed on `main`, the `readme`
job publishes the README rendered by `python3 tools/update_readme.py` in the
retail job. It renders the marked README status block from that build's
`report.json` and `symbols.txt`, without changing surrounding prose. Missing reports fail rather than publishing zeros.
Identical figures produce no commit, PR or date-only churn.

Changed figures create a `ci/readme-progress-<source SHA>` branch and README-only
PR using the job's Forgejo token, then explicitly dispatches `build.yml` on
that branch to run tooling and retail checks again. Explicit dispatch is needed
because [Forgejo automatic tokens](https://forgejo.org/docs/latest/user/actions/basic-concepts/)
suppress workflows triggered indirectly by their commits and PRs.
The dependent `readme` job merges only when the PR changes README alone and its entire
contents reproduce the generated block applied to the current base README. The
merge is guarded by the checked head SHA; branch protections still apply. GitHub
only consumes the mirrored result. Older main runs skip publication after main
advances. Failed checks leave the update PR unmerged.

To refresh locally after `bash build.sh`, run `python3 tools/update_readme.py`.
Only the CI publication step needs repository contents and pull-request write
permissions; it uses the existing job token, with no additional stored secret.
