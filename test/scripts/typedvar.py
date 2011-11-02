#
#
#

import unittest
import target
import pykd

class TypedVarTest( unittest.TestCase ):

    def testCtor( self ):
        tv = target.module.typedVar( "structTest", target.module.g_structTest )
        tv = target.module.typedVar( "g_structTest" )
        
    def testGetAddress( self ):
        tv = target.module.typedVar( "structTest", target.module.g_structTest )
        self.assertEqual( tv.getAddress(), target.module.g_structTest )
        
    def testGetSize( self ):
        tv1 = target.module.typedVar( "structTest", target.module.g_structTest )
        self.assertEqual( 16, tv1.sizeof() )
        #tv2 = target.module.typedVar( "structTest[2]", target.module.g_testArray )
        #self.assertEqual( tv1.sizeof()*2, tv2.sizeof() )        
        
    def testStruct(self):
        tv1 = target.module.typedVar( "structTest", target.module.g_structTest )
        self.assertEqual( 0, tv1.m_field0 + 0 )
        self.assertEqual( 500, tv1.m_field1 )
        self.assertEqual( True, tv1.m_field2 )
        self.assertEqual( 1, tv1.m_field3 )