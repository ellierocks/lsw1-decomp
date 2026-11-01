# Development Workflow

How changes get from a branch to `main`. Build setup lives in [README.md](../README.md); C verification and promotion
live in [C matching](code_matching_workflow.md).

## Repositories

| Remote | Role |
|--------|------|
| [GitHub `ellierocks/lsw1-decomp`](https://github.com/ellierocks/lsw1-decomp) | Primary. All pushes go here; CI runs here. |
| Codeberg | Read-only mirror, updated from GitHub by the `mirror` workflow. |

Codeberg receives changes from GitHub. Do not push to it directly.

## Branches and pull requests

- `main` is the integration branch. Commit to it only through a merged pull request.
- Work on a topic branch named for its kind of change: `feat/`, `fix/`, `docs/`,
  `chore/` or `ci/`. Use one branch per concern.
- Commit messages use Conventional Commits with a lowercase imperative description,
  for example `fix(ci): install ninja for retail builds`.
- Stage explicit paths. Check `git status` and `git diff --cached` before committing.
- Open a pull request into `main`. Keep its title and description accurate as the
  change evolves: what changed, why, and what was verified.
- A PR may carry several commits, but each should build and pass the portable checks.

## Checks

Portable checks need no retail input and run in CI on every PR:

```sh
python3 -m compileall -q configure.py tools tests
python3 -m unittest discover -s tests
python3 configure.py
mkdir -p artifacts && python3 tools/symbols_to_map.py --all --output artifacts/GL5E4F.map
bash -n build.sh tools/build_patched_dtk.sh tools/orig_crypt.sh
```

Retail checks need the original DOL at `orig/GL5E4F/sys/main.dol`, which must match
SHA-1 `95cca08a19224775d1a8d6cc64601fb7d0080981`:

```sh
bash build.sh                          # verify input, build tools, split, compile, report
python3 tools/matching_progress.py     # objdiff matching summary
```

Run the retail checks locally when a change touches splitting, configuration,
compiler wiring, candidate sources or the report. Pure tooling changes need only the
portable checks. Documentation-only changes need local link validation,
migrated-content preservation review and `git diff --check`, without unrelated
software builds. Required current-head CI still applies.

Before proposing a C candidate for matching, verify it with `tools/verify_fn.py`
(see [code matching workflow](code_matching_workflow.md)). A candidate is not linked
into the build until it matches.

## Continuous integration

| Job | Workflow file | Runs on | Purpose |
|-----|---------------|---------|---------|
| `tooling` | `.github/workflows/build.yml` | push to `main`, PRs | Portable checks above |
| `retail` | `.github/workflows/build.yml` | push to `main`, same-repo PRs, dispatch | Decrypts the DOL with `ORIG_KEY`, then runs `bash build.sh` and the verifier tests against genuine ProDG objects (`VERIFY_FN_REQUIRE_RETAIL=1`) |
| `readme` | `.github/workflows/build.yml` | successful main push or dispatched update branch | Publishes README progress to a check branch and fast-forwards `main` after it passes |
| `build` | `.github/workflows/build.yml` | push to `main`, PRs | Builds the report and uploads `GL5E4F_report` for decomp.dev |
| `build-map` | `.github/workflows/generate-map.yml` | changes to symbols or map tooling | Uploads the Dolphin map |

Fork PRs have no `ORIG_KEY`, so `retail` is skipped there and `build` skips its
build steps without failing.

A PR is ready when its required checks succeed on its current head. Pending, missing,
skipped or cancelled required checks are not success. Push a new head and the checks
must be observed again.

## Merging

Before merging:

- Required CI has succeeded on the PR's current head.
- The diff has been reviewed and any findings are resolved or explicitly deferred.
- Relevant local checks have run for the affected area.
- Required approvals and holds are honored. Branch protections are not bypassed.

After merging, confirm on the forge that the PR is merged and note the merge commit.
Delete the topic branch once it is merged.

## Secrets

`ORIG_KEY` is the passphrase for `orig/GL5E4F/sys/main.dol.enc`. It is set as an
Actions secret on GitHub, the only forge that runs the retail job. Never commit the decrypted DOL,
and never print the key in logs. Rotation steps are in
[key rotation](#key-rotation).

## Matching evidence

C matching sessions follow the bounded [matching loop](code_matching_workflow.md#measure-and-iterate).
Phase 3 sprint PRs use the [sprint template](sprint_workflow.md#records). PRs
retain the baseline, best attempt, target addresses and exact verification
commands, promoted bytes and remaining limits. Local experiment snapshots stay
under ignored `work/`; include a
concise explanation of what improved and whether any bytes were promoted.

Symbol naming and matched code are separate measures. Naming (`config/GL5E4F/symbols.txt`)
moves quickly from donor evidence. Matched code is what the objdiff report and decomp.dev
measure. Report both, and state which one a change affects.

Donor projects (CrashWOC, the LSW1 and LSW2 Mac builds) are evidence, not sources to copy.
Project code is written from analysis of this binary, and no original game code or assets
are committed.

## Reports and splitter maintenance

The GitHub `build` job uploads `build/GL5E4F/report.json` as `GL5E4F_report`.
decomp.dev consumes the latest successful `main` artifact. This measures objdiff
matched code, with strict relocation comparison (`functionRelocDiffs=all`),
separately from naming progress. Generating a report does not require a matching
whole-DOL checksum.

`tools/build_patched_dtk.sh` builds the pinned DTK commit with the patch in
`tools/patches/`. The patch tolerates ProDG exception tables and the target
symbol map. Do not substitute a release tag or unpatched splitter; see
[build configuration](build_config.md#splitter-and-startup-constraints).

Maintainers configure `ORIG_KEY` as an Actions secret on GitHub. Public
reporting additionally requires repository registration at decomp.dev and its
GitHub App for workflow-completion webhooks. The `GL5E4F-dolphin-map` artifact
comes from the separate GitHub map workflow.

### Key rotation

With the replacement `ORIG_KEY` supplied securely in the environment, run:

```sh
bash tools/orig_crypt.sh encrypt
```

Commit the new `orig/GL5E4F/sys/main.dol.enc`, then update the Actions secret on
GitHub. `orig_crypt.sh` checks the retail SHA-1 before encrypting and after
decrypting. Keep plaintext retail input and passphrases out of commits and logs.

## Automatic README progress

After the GitHub `tooling` and `retail` checks succeed on `main`, the `readme`
job publishes the README rendered by `python3 tools/update_readme.py` in the
retail job. It renders the marked README status block from that build's
`report.json` and `symbols.txt`, without changing surrounding prose. Missing reports fail rather than publishing zeros.
Identical figures produce no commit, branch or date-only churn.

Changed figures push one README commit on the source SHA to a
`ci/readme-progress-<source SHA>` branch, then explicitly dispatch `build.yml`
on that branch to run tooling and retail checks again. Explicit dispatch is needed
because a push made with the automatic `GITHUB_TOKEN` does not trigger another
workflow run. The dispatched `readme` job fast-forwards `main` to the checked
commit and deletes the branch only when the commit's parent is still `main`, it
changes README alone, and its entire contents reproduce the generated block
applied to the parent README.

The flow uses no pull requests. `tools/update_readme.py` reads the branch head and
dispatches the workflow through the GitHub API; all ref updates go through git.
`tests/test_update_readme.py` runs the whole cycle against a real git origin. Older
main runs skip publication after main advances. Failed checks leave the progress
branch unmerged.

To refresh locally after `bash build.sh`, run `python3 tools/update_readme.py`.
Only the CI publication step needs repository contents and actions write
permissions; it uses the workflow token, with no additional stored secret.
