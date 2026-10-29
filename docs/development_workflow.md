# Development Workflow

How changes get from a branch to `main`. Layout and tool usage live in
[AGENTS.md](../AGENTS.md); CI job details and one-time forge setup live in
[CI and progress reporting](ci_and_decomp_dev.md).

## Repositories

| Remote | Role |
|--------|------|
| Forgejo `ellie/lsw1-decomp` | Primary. All pushes go here. |
| [GitHub `ellierocks/lsw1-decomp`](https://github.com/ellierocks/lsw1-decomp) | Public mirror. Its GitHub-only CI jobs produce the decomp.dev report and Dolphin map. |
| Codeberg | Mirror. |

Mirrors receive changes from Forgejo. Do not push to them directly.

## Branches and pull requests

- `main` is the integration branch. Commit to it only through a merged pull request.
- Work on a topic branch named for its kind of change: `feat/`, `fix/`, `docs/`,
  `chore/` or `ci/`. Use one branch per concern.
- Commit messages use Conventional Commits with a lowercase imperative description,
  for example `fix(ci): install ninja for forgejo retail builds`.
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
portable checks.

Before proposing a C candidate for matching, verify it with `tools/verify_fn.py`
(see [code matching workflow](code_matching_workflow.md)). A candidate is not linked
into the build until it matches.

## Continuous integration

| Job | Workflow file | Runs on | Forge | Purpose |
|-----|---------------|---------|-------|---------|
| `tooling` | `.github/workflows/build.yml` | push to `main`, PRs | Forgejo and GitHub | Portable checks above |
| `retail` | `.github/workflows/build.yml` | push to `main`, PRs | Forgejo only | Decrypts the DOL with `ORIG_KEY`, then runs `bash build.sh` and the verifier tests against genuine ProDG objects (`VERIFY_FN_REQUIRE_RETAIL=1`) |
| `build` | `.github/workflows/build.yml` | push to `main`, PRs | GitHub only | Builds the report and uploads `GL5E4F_report` for decomp.dev |
| `build-map` | `.github/workflows/generate-map.yml` | changes to symbols or map tooling | GitHub only | Uploads the Dolphin map |

On GitHub, fork PRs have no `ORIG_KEY` and skip the `build` job without failing. The
Forgejo `retail` job fails when `ORIG_KEY` is missing.

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
Actions secret on each forge that runs the retail job. Never commit the decrypted DOL,
and never print the key in logs. Rotation steps are in
[CI and progress reporting](ci_and_decomp_dev.md#rotating-the-key).

## Matching evidence

Symbol naming and matched code are separate measures. Naming (`config/GL5E4F/symbols.txt`)
moves quickly from donor evidence. Matched code is what the objdiff report and decomp.dev
measure. Report both, and state which one a change affects.

Donor projects (CrashWOC, the LSW1 and LSW2 Mac builds) are evidence, not sources to copy.
Project code is written from analysis of this binary, and no original game code or assets
are committed.
