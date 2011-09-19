#
#
#

import unittest
import target
import pykd

class ModuleTest( unittest.TestCase ):
    
    def testCtor( self ):
         self.assertRaises( RuntimeError, pykd.module )
         
    def testName( self ):
         self.assertEqual( target.moduleName, target.module.name() )
         
    def testSize( self ):
         self.assertNotEqual( 0, target.module.size() )
         
    def testBegin( self ):
         self.assertNotEqual( 0, target.module.begin() )
             
    def testEnd( self ):
         self.assertEqual( target.module.size(), target.module.end() - target.module.begin() )
