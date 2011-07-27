#
#
#

import unittest
import target

class BaseTest( unittest.TestCase ):

    def test1( self ):
        self.assertNotEqual( target.module, None )
   
    
    