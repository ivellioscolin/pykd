"""Public microsoft symbols tests"""

import unittest
import pykd
import os

class PeFileAsDumpLoader:
    """Load/unload PE-file from System as crash dump file"""
    def __init__(self, fileName):
        self._fileName = fileName

    def __enter__(self):
        pykd.loadDump(self._fileName)

    def __exit__(self, exc_type, exc_value, exc_tb):
        pykd.detachProcess()

class MsPdbTest(unittest.TestCase):
    """Public Microsoft symbols tests"""

    def testSymbolNameAddress(self):
        """Lookup symbol by name/address"""
        with PeFileAsDumpLoader( os.path.join(os.environ["WINDIR"], r"System32\ole32.dll") ):
            mod = pykd.module("ole32")
            print "\n" + str( mod )

            targetSymAddr = mod.offset("CPackagerMoniker::Create")
            self.assertNotEqual( 0, targetSymAddr )
            self.assertEqual( "CPackagerMoniker::Create", mod.findSymbol(targetSymAddr) )

            targetSymAddr = mod.offset("CoInitialize")
            self.assertNotEqual( 0, targetSymAddr )
            self.assertEqual( "CoInitialize", mod.findSymbol(targetSymAddr) )

        with PeFileAsDumpLoader( os.path.join(os.environ["WINDIR"], r"System32\authz.dll") ):
            mod = pykd.module("authz")
            print "\n" + str( mod )

            targetSymAddr = mod.offset("AuthzpDefaultAccessCheck")
            self.assertNotEqual( 0, targetSymAddr )
            self.assertEqual( "AuthzpDefaultAccessCheck", mod.findSymbol(targetSymAddr) )

            targetSymAddr = mod.offset("AuthzAccessCheck")
            self.assertNotEqual( 0, targetSymAddr )
            self.assertEqual( "AuthzAccessCheck", mod.findSymbol(targetSymAddr) )
