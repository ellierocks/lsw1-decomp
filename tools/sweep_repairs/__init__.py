"""Draft repairs for tools/sweep.py, one module per family, behind an ordered registry.

A repair is a text rewrite of an m2c draft that sweep.py scores and keeps when it scores
better. Each module in this package registers its repairs with `@repair(stage, order)`;
the package imports every module in its directory, so a new repair is one new file.
A stage runs its repairs in ascending `order`; a taken (stage, order) slot or a reused
function name raises at import, so two branches that pick the same slot fail in tests.

Stages (the result flag sweep.py writes into a row is the function's `__name__`):
  frame     (body, asm) -> str        target frame size; sweep_one, before `scored`
  scored    (body) -> str             sweep_one, each kept only when it scores better
  final     (body, name, asm, unit) -> str | list[str]   independent alternatives after the pipeline
  variant   (body) -> str             try_variants, one alternative per repair
  variants  (body) -> list[str]       try_variants, after every `variant`
A repair returns BODY unchanged (or []) when it does not apply.
"""

from __future__ import annotations

import importlib
import pkgutil
from typing import Callable

STAGES = {
    "frame": "(body, asm) -> str",
    "scored": "(body) -> str",
    "final": "(body, name, asm, unit) -> str | list[str]",
    "variant": "(body) -> str",
    "variants": "(body) -> list[str]",
}


class Registry:
    """Repairs by stage, each at an explicit integer order."""

    def __init__(self) -> None:
        self._slots: dict[str, dict[int, Callable]] = {s: {} for s in STAGES}
        self._names: dict[str, tuple[str, int]] = {}

    def repair(self, stage: str, order: int) -> Callable[[Callable], Callable]:
        """Decorator registering a function in STAGE at ORDER; returns it unchanged."""
        if stage not in STAGES:
            raise ValueError(f"unknown repair stage {stage!r}; one of {', '.join(STAGES)}")

        def register(fn: Callable) -> Callable:
            slots = self._slots[stage]
            if order in slots:
                raise ValueError(f"repair slot ({stage!r}, {order}) is taken by {slots[order].__name__} "
                                 f"({slots[order].__module__}); pick an unused order for {fn.__name__}")
            if fn.__name__ in self._names:
                raise ValueError(f"repair name {fn.__name__!r} is already registered at {self._names[fn.__name__]}")
            slots[order] = fn
            self._names[fn.__name__] = (stage, order)
            return fn
        return register

    def stage(self, stage: str) -> tuple[Callable, ...]:
        """STAGE's repairs in ascending order."""
        return tuple(fn for _, fn in sorted(self._slots[stage].items()))

    def load(self, path, package: str) -> None:
        """Import every module of PACKAGE (directory PATH); `_`-prefixed modules are helpers."""
        for info in sorted(pkgutil.iter_modules(path), key=lambda i: i.name):
            if not info.name.startswith("_"):
                importlib.import_module(f"{package}.{info.name}")


REGISTRY = Registry()
repair = REGISTRY.repair
stage = REGISTRY.stage
REGISTRY.load(__path__, __name__)
