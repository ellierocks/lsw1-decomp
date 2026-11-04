#!/usr/bin/env python3
"""Use origin, retrying GitHub SSH public-key failures with the current gh login.

No remote or credential configuration is persisted. Other failures are returned intact.
"""
import re
import subprocess
import sys


def run(args):
    result = subprocess.run(['git', *args], capture_output=True, text=True)
    if result.returncode and 'Permission denied (publickey)' in result.stderr:
        remote = subprocess.run(['git', 'remote', 'get-url', *(['--push'] if args[0] == 'push' else []),
                                 'origin'], capture_output=True, text=True)
        match = re.fullmatch(r'(?:git@github.com:|ssh://git@github.com/)([^\s]+)', remote.stdout.strip())
        if remote.returncode == 0 and match:
            url = 'https://github.com/' + match[1]
            print('GitHub SSH key unavailable; retrying with the current gh login.', file=sys.stderr)
            result = subprocess.run(['git', '-c', 'credential.helper=', '-c',
                'credential.helper=!gh auth git-credential', '-c', 'url.' + url + '.insteadOf=' + remote.stdout.strip(), *args], capture_output=True, text=True)
    sys.stdout.write(result.stdout)
    sys.stderr.write(result.stderr)
    return result.returncode


if __name__ == '__main__':
    if len(sys.argv) < 2 or sys.argv[1] not in ('fetch', 'push'):
        sys.exit('usage: git_origin.py {fetch|push} [git arguments]')
    sys.exit(run(sys.argv[1:]))
