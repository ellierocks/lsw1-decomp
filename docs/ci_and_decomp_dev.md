# CI and decomp.dev Progress Reporting

This project follows the standard GameCube/Wii decomp convention: a CI workflow
builds the project, generates an [objdiff](https://github.com/encounter/objdiff)
`report.json`, and uploads it as an artifact that [decomp.dev](https://decomp.dev)
ingests to track matching progress over time.

The matching metric decomp.dev shows is the **objdiff matched-code percentage**
(per-function instruction matching), which is distinct from the symbol-naming
percentages in the README status table.

## How it works

- `.github/workflows/build.yml` runs on every push to `main` and on PRs.
- It decrypts the original DOL, builds, and runs `ninja build/GL5E4F/report.json`.
- The report is uploaded as an artifact named **`GL5E4F_report`**
  (the `<VERSION>_report` convention decomp.dev expects).
- decomp.dev pulls that artifact from the latest successful `main` run.

> The full DOL link does not yet succeed (early-stage decomp), so CI targets
> `report.json` directly rather than the SHA-checked link. `report.json` is
> generated from the per-unit objects and does not require a complete link.

## One-time admin setup

These steps require the original retail DOL and repo-admin access. Do them once.

### 1. Encrypt and commit the original DOL

The retail DOL is copyrighted and must never be committed in the clear. We commit
only an AES-256 blob and let CI decrypt it with a secret.

```sh
# Pick a strong passphrase and keep it somewhere safe (a password manager).
export ORIG_KEY='<your-strong-passphrase>'

# With your verified retail DOL at orig/GL5E4F/sys/main.dol:
bash tools/orig_crypt.sh encrypt        # writes orig/GL5E4F/sys/main.dol.enc
git add orig/GL5E4F/sys/main.dol.enc
git commit -m "ci: add encrypted original DOL"
```

`tools/orig_crypt.sh` verifies the DOL SHA-1 (`95cca08a…`) before encrypting and
again after decrypting, so a wrong key or wrong DOL fails loudly.

### 2. Set the repository secret

```sh
gh secret set ORIG_KEY        # paste the same passphrase
```

Or: repo **Settings → Secrets and variables → Actions → New repository secret**,
name `ORIG_KEY`.

After this, push to `main` and confirm the **Build** workflow goes green and
produces the `GL5E4F_report` artifact.

### 3. Register on decomp.dev

1. As a repo admin, go to <https://decomp.dev/manage/new> and add the repository.
2. Install the **decomp.dev GitHub App** on the repo (or org). The app gives
   decomp.dev workflow-completion webhooks (instead of 5-minute polling) and lets
   it post progress-diff comments on PRs.

That's it — once reports land on `main`, decomp.dev tracks the project
automatically.

## Notes

- **Fork PRs** cannot read secrets, so CI skips the build there (it does not
  fail). Progress is reported from same-repo runs on `main`.
- To regenerate the report locally: `ninja build/GL5E4F/report.json`, then
  `python3 tools/progress.py --matching`.
- To rotate the key: re-run `encrypt` with a new `ORIG_KEY`, commit the new
  `.enc`, and update the `ORIG_KEY` secret.
