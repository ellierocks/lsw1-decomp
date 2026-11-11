import contextlib
import http.client
import io
import ssl
import sys
import tempfile
import unittest
import urllib.error
from pathlib import Path
from unittest.mock import Mock, patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import download_tool

URL = 'https://example.invalid/releases/download/1.0/wibo-x86_64'


def http_error(code):
    return urllib.error.HTTPError(URL, code, 'fixture', {}, None)


class Response(io.BytesIO):
    """urlopen() result: a readable context manager."""


class DroppedResponse:
    """A response whose connection drops after the first chunk."""

    def __init__(self):
        self.sent = False

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        return False

    def read(self, size=-1):
        if not self.sent:
            self.sent = True
            return b'partial'
        raise http.client.IncompleteRead(b'', 100)


class FetchTests(unittest.TestCase):
    def setUp(self):
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        self.dir = Path(tmp.name)
        self.output = self.dir / 'wibo'
        self.sleep = Mock()
        self.errors = io.StringIO()
        quiet = contextlib.redirect_stderr(self.errors)
        quiet.__enter__()
        self.addCleanup(quiet.__exit__, None, None, None)

    def fetch(self, responses):
        with patch.object(download_tool.urllib.request, 'urlopen', side_effect=responses) as urlopen:
            try:
                download_tool.fetch(URL, self.output, sleep=self.sleep)
            finally:
                self.urlopen = urlopen

    def test_succeeds_after_two_gateway_timeouts(self):
        self.fetch([http_error(504), http_error(504), Response(b'tool')])
        self.assertEqual(self.output.read_bytes(), b'tool')
        self.assertTrue(self.output.stat().st_mode & 0o100)
        self.assertEqual(self.urlopen.call_count, 3)
        self.assertEqual([c.args[0] for c in self.sleep.call_args_list], [2.0, 4.0])
        self.assertEqual(self.urlopen.call_args.kwargs['timeout'], download_tool.TIMEOUT)
        self.assertEqual(self.errors.getvalue().count('retrying in'), 2)

    def test_gives_up_after_attempt_cap(self):
        with self.assertRaises(urllib.error.HTTPError) as raised:
            self.fetch([http_error(503)] * download_tool.ATTEMPTS)
        self.assertEqual(raised.exception.code, 503)
        self.assertEqual(self.urlopen.call_count, download_tool.ATTEMPTS)
        self.assertEqual(self.sleep.call_count, download_tool.ATTEMPTS - 1)
        self.assertTrue(all(c.args[0] <= download_tool.RETRY_MAX_DELAY for c in self.sleep.call_args_list))
        self.assertFalse(self.output.exists())

    def test_client_errors_are_not_retried(self):
        for code in (404, 403):
            with self.subTest(code=code), self.assertRaises(urllib.error.HTTPError):
                self.fetch([http_error(code), Response(b'tool')])
            self.assertEqual(self.urlopen.call_count, 1)
        self.sleep.assert_not_called()

    def test_certificate_failure_is_left_to_certifi_fallback(self):
        error = urllib.error.URLError(ssl.SSLCertVerificationError('CERTIFICATE_VERIFY_FAILED'))
        with self.assertRaises(urllib.error.URLError):
            self.fetch([error, Response(b'tool')])
        self.assertEqual(self.urlopen.call_count, 1)

    def test_connection_reset_and_timeout_are_retried(self):
        self.fetch([urllib.error.URLError(ConnectionResetError()), urllib.error.URLError(TimeoutError()),
                    TimeoutError(), Response(b'tool')])
        self.assertEqual(self.output.read_bytes(), b'tool')
        self.assertEqual(self.sleep.call_count, 3)

    def test_failed_download_keeps_installed_tool(self):
        self.output.write_bytes(b'installed')
        self.output.chmod(0o755)
        with self.assertRaises(http.client.IncompleteRead):
            self.fetch([DroppedResponse() for _ in range(download_tool.ATTEMPTS)])
        self.assertEqual(self.output.read_bytes(), b'installed')
        self.assertEqual(sorted(p.name for p in self.dir.iterdir()), ['wibo'])
        self.assertEqual(self.urlopen.call_count, download_tool.ATTEMPTS)

    def test_dropped_download_recovers_on_retry(self):
        self.output.write_bytes(b'installed')
        self.fetch([DroppedResponse(), Response(b'tool')])
        self.assertEqual(self.output.read_bytes(), b'tool')
        self.assertEqual(sorted(p.name for p in self.dir.iterdir()), ['wibo'])


class MainTests(unittest.TestCase):
    def test_certificate_failure_retries_with_certifi_context(self):
        try:
            import certifi  # noqa: F401
        except ImportError:
            self.skipTest('certifi not installed')
        error = urllib.error.URLError(ssl.SSLCertVerificationError('CERTIFICATE_VERIFY_FAILED'))
        argv = ['download_tool.py', 'wibo', 'out/wibo', '--tag', '1.0.0']
        with patch.object(sys, 'argv', argv), patch.object(download_tool, 'fetch', side_effect=[error, None]) as fetch, \
                contextlib.redirect_stdout(io.StringIO()):
            download_tool.main()
        self.assertEqual(fetch.call_count, 2)
        self.assertNotIn('context', fetch.call_args_list[0].kwargs)
        self.assertIsInstance(fetch.call_args_list[1].kwargs['context'], ssl.SSLContext)


if __name__ == '__main__':
    unittest.main()
