#
#
#

import unittest
import target
import pykd

class BaseTest( unittest.TestCase ):
      
    def testImport( self ):
        self.assertNotEqual( None, pykd.module )
        self.assertNotEqual( None, pykd.dbgClient )

