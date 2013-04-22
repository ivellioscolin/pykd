"""Public microsoft symbols tests"""

import unittest
import pykd
import os

class PeFileAsDumpLoader:
    """Load/unload PE-file from System as crash dump file"""
    def __init__(self, fileName):
        self._fileName = fileName
        self._loaded = False

    def __enter__(self):
        pykd.loadDump(self._fileName)
        self._loaded = True

    def __exit__(self, exc_type, exc_value, exc_tb):
        if self._loaded:
            pykd.detachProcess()
            self._loaded = False

class MsPdbTest(unittest.TestCase):
    """Public Microsoft symbols tests"""

    def testFindMethodOffset(self):
        """Lookup method offset by name"""
        with PeFileAsDumpLoader(  os.environ["WINDIR"] + r"\System32\ole32.dll" ) as loadedDump:
            self.assertNotEqual( 0, pykd.getOffset("ole32!CPackagerMoniker::AddRef") )
