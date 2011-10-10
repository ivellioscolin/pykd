
import unittest
import target
import pykd

class DbgcmdTest( unittest.TestCase ):

    def testDbgCommand( self ):
        self.assertNotEqual( "", pykd.dbgCommand("lm") )

