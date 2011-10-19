#
#
#

import unittest
import target
import pykd

class TypedVarTest( unittest.TestCase ):

    def testCtor( self ):
        try: pykd.typedVar()
        except RuntimeError: pass
        
        try: pykd.typedVar( "structTest", target.module.g_structTest )
        except RuntimeError: pass        
        
        tv = target.module.typedVar( "structTest", target.module.g_structTest )
        tv = target.module.typedVar( "g_structTest" )
        
    def testGetAddress( self ):
        tv = target.module.typedVar( "structTest", target.module.g_structTest )
        self.assertEqual( tv.getAddress(), target.module.g_structTest )
        
    def testGetSize( self ):
        tv1 = target.module.typedVar( "structTest", target.module.g_structTest )
        self.assertEqual( 16, tv1.sizeof() )
        #tv2 = target.module.typedVar( "structTest[]", target.module.g_testArray )
        #self.assertEqual( tv1.sizeof()*2, tv2.sizeof() )        
