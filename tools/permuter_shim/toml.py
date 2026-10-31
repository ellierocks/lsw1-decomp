"""Minimal `toml` for decomp-permuter's local mode, backed by the standard tomllib.

The permuter only reads settings.toml locally; its network mode (toml.dump) is unsupported.
"""
import tomllib


def load(f):
    return tomllib.loads(f.read() if hasattr(f, "read") else open(f, encoding="utf-8").read())


def loads(s):
    return tomllib.loads(s)
