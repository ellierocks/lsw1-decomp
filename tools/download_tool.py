#!/usr/bin/env python3

###
# Downloads various tools from GitHub releases.
#
# Usage:
#   python3 tools/download_tool.py wibo build/tools/wibo --tag 1.0.0
#
# If changes are made, please submit a PR to
# https://github.com/encounter/dtk-template
###

import argparse
import http.client
import io
import os
import platform
import shutil
import stat
import sys
import tempfile
import time
import urllib.error
import urllib.request
import zipfile
from typing import Callable, Dict
from pathlib import Path


def binutils_url(tag):
    uname = platform.uname()
    system = uname.system.lower()
    arch = uname.machine.lower()
    if system == "darwin":
        system = "macos"
        arch = "universal"
    elif arch == "amd64":
        arch = "x86_64"

    repo = "https://github.com/encounter/gc-wii-binutils"
    return f"{repo}/releases/download/{tag}/{system}-{arch}.zip"


def compilers_url(tag: str) -> str:
    return f"https://files.decomp.dev/compilers_{tag}.zip"


def dtk_url(tag: str) -> str:
    uname = platform.uname()
    suffix = ""
    system = uname.system.lower()
    if system == "darwin":
        system = "macos"
    elif system == "windows":
        suffix = ".exe"
    arch = uname.machine.lower()
    if arch == "amd64":
        arch = "x86_64"

    repo = "https://github.com/encounter/decomp-toolkit"
    return f"{repo}/releases/download/{tag}/dtk-{system}-{arch}{suffix}"


def objdiff_cli_url(tag: str) -> str:
    uname = platform.uname()
    suffix = ""
    system = uname.system.lower()
    if system == "darwin":
        system = "macos"
    elif system == "windows":
        suffix = ".exe"
    arch = uname.machine.lower()
    if arch == "amd64":
        arch = "x86_64"

    repo = "https://github.com/encounter/objdiff"
    return f"{repo}/releases/download/{tag}/objdiff-cli-{system}-{arch}{suffix}"


def sjiswrap_url(tag: str) -> str:
    repo = "https://github.com/encounter/sjiswrap"
    return f"{repo}/releases/download/{tag}/sjiswrap-windows-x86.exe"


def wibo_url(tag: str) -> str:
    uname = platform.uname()
    arch = uname.machine.lower()
    system = uname.system.lower()
    if system == "darwin":
        arch = "macos"

    repo = "https://github.com/decompals/wibo"
    return f"{repo}/releases/download/{tag}/wibo-{arch}"


TOOLS: Dict[str, Callable[[str], str]] = {
    "binutils": binutils_url,
    "compilers": compilers_url,
    "dtk": dtk_url,
    "objdiff-cli": objdiff_cli_url,
    "sjiswrap": sjiswrap_url,
    "wibo": wibo_url,
}


def download(url, response, output) -> None:
    if url.endswith(".zip"):
        data = io.BytesIO(response.read())
        with zipfile.ZipFile(data) as f:
            f.extractall(output)
        # Make all files executable
        for root, _, files in os.walk(output):
            for name in files:
                os.chmod(os.path.join(root, name), 0o755)
        output.touch(mode=0o755)  # Update dir modtime
    else:
        # Never truncate an executable another worktree may be running (ETXTBSY).
        # A failed download leaves the installed tool intact.
        output = Path(output).resolve()
        mode = stat.S_IMODE(output.stat().st_mode) if output.exists() else 0o644
        temporary = None
        try:
            with tempfile.NamedTemporaryFile(dir=output.parent, prefix=output.name + ".",
                                             delete=False) as f:
                temporary = Path(f.name)
                shutil.copyfileobj(response, f)
            temporary.chmod(mode | stat.S_IEXEC)
            temporary.replace(output)
        finally:
            if temporary is not None:
                temporary.unlink(missing_ok=True)


# Transient failures (gateway timeouts, rate limits, dropped connections) are
# retried with capped exponential backoff; anything else fails immediately.
ATTEMPTS = 4
RETRY_BASE_DELAY = 2.0
RETRY_MAX_DELAY = 15.0
TIMEOUT = 60
TRANSIENT_HTTP_CODES = {408, 429}


def is_transient(error: BaseException) -> bool:
    if isinstance(error, urllib.error.HTTPError):
        return error.code in TRANSIENT_HTTP_CODES or 500 <= error.code < 600
    if isinstance(error, urllib.error.URLError):
        # Certificate and DNS-configuration errors are not fixed by waiting.
        return isinstance(error.reason, (TimeoutError, ConnectionError))
    # Raised while reading the body: the connection dropped mid-download.
    return isinstance(error, (http.client.IncompleteRead, TimeoutError, ConnectionError))


def retry_delay(attempt: int) -> float:
    return min(RETRY_BASE_DELAY * 2 ** (attempt - 1), RETRY_MAX_DELAY)


def fetch(url, output, *, context=None, attempts=ATTEMPTS, sleep=time.sleep) -> None:
    """Download url into output, retrying transient failures.

    download() replaces files atomically, so a failed attempt leaves any
    installed tool intact.
    """
    req = urllib.request.Request(url, headers={"User-Agent": "Mozilla/5.0"})
    for attempt in range(1, attempts + 1):
        try:
            with urllib.request.urlopen(req, timeout=TIMEOUT, context=context) as response:
                download(url, response, output)
            return
        except Exception as e:
            if attempt == attempts or not is_transient(e):
                raise
            delay = retry_delay(attempt)
            print(f"Download attempt {attempt}/{attempts} of {url} failed: {e!r}; "
                  f"retrying in {delay:g}s", file=sys.stderr)
            sleep(delay)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("tool", help="Tool name")
    parser.add_argument("output", type=Path, help="output file path")
    parser.add_argument("--tag", help="GitHub tag", required=True)
    args = parser.parse_args()

    url = TOOLS[args.tool](args.tag)
    output = Path(args.output)

    print(f"Downloading {url} to {output}")
    try:
        fetch(url, output)
    except urllib.error.URLError as e:
        if str(e).find("CERTIFICATE_VERIFY_FAILED") == -1:
            raise e
        try:
            import certifi
            import ssl
        except ImportError:
            print(
                '"certifi" module not found. Please install it using "python -m pip install certifi".'
            )
            return

        fetch(url, output, context=ssl.create_default_context(cafile=certifi.where()))


if __name__ == "__main__":
    main()
