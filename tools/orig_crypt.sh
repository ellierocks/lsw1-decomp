#!/usr/bin/env bash
#
# Encrypt / decrypt the original game DOL for CI use.
#
# The retail DOL is copyrighted and must NEVER be committed in the clear. We
# commit only an AES-256 encrypted blob (orig/GL5E4F/sys/main.dol.enc); CI
# decrypts it at build time using the ORIG_KEY repository secret. This is the
# standard pattern used by GameCube/Wii decomp projects to get reproducible CI
# builds without distributing copyrighted material.
#
# Usage:
#   ORIG_KEY=<passphrase> bash tools/orig_crypt.sh encrypt   # local, one-time
#   ORIG_KEY=<passphrase> bash tools/orig_crypt.sh decrypt   # CI / fresh clone
#
set -euo pipefail

MODE="${1:-}"
PLAIN="orig/GL5E4F/sys/main.dol"
CIPHER="orig/GL5E4F/sys/main.dol.enc"
EXPECTED_SHA1="95cca08a19224775d1a8d6cc64601fb7d0080981"

: "${ORIG_KEY:?Set ORIG_KEY (the encryption passphrase) in the environment}"

sha1_of() {
  if command -v sha1sum >/dev/null 2>&1; then
    sha1sum "$1" | cut -d' ' -f1
  else
    shasum -a 1 "$1" | cut -d' ' -f1
  fi
}

case "$MODE" in
  encrypt)
    [ -f "$PLAIN" ] || { echo "error: missing $PLAIN (place your retail DOL there first)" >&2; exit 1; }
    got="$(sha1_of "$PLAIN")"
    if [ "$got" != "$EXPECTED_SHA1" ]; then
      echo "error: $PLAIN SHA-1 $got != expected $EXPECTED_SHA1 (wrong/dirty DOL)" >&2
      exit 1
    fi
    openssl enc -aes-256-cbc -pbkdf2 -salt -in "$PLAIN" -out "$CIPHER" -pass env:ORIG_KEY
    echo "wrote $CIPHER (commit this; keep $PLAIN local)"
    ;;
  decrypt)
    [ -f "$CIPHER" ] || { echo "error: missing $CIPHER (run 'encrypt' and commit it first)" >&2; exit 1; }
    mkdir -p "$(dirname "$PLAIN")"
    openssl enc -d -aes-256-cbc -pbkdf2 -salt -in "$CIPHER" -out "$PLAIN" -pass env:ORIG_KEY
    got="$(sha1_of "$PLAIN")"
    if [ "$got" != "$EXPECTED_SHA1" ]; then
      echo "error: decrypted $PLAIN SHA-1 $got != expected $EXPECTED_SHA1 (wrong ORIG_KEY?)" >&2
      rm -f "$PLAIN"
      exit 1
    fi
    echo "decrypted + verified $PLAIN"
    ;;
  *)
    echo "usage: ORIG_KEY=<passphrase> $0 {encrypt|decrypt}" >&2
    exit 2
    ;;
esac
