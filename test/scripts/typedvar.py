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
        self.assertEqual( 20, tv1.sizeof() )
        #tv2 = target.module.typedVar( "structTest[2]", target.module.g_testArray )
        #self.assertEqual( tv1.sizeof()*2, tv2.sizeof() )

    def testByAddress( self ):
        tv1 = target.module.typedVar( "structTest", target.module.g_structTest )
        tv2 = target.module.typedVar( tv1.getAddress() )
        self.assertEqual( tv2.getAddress(), tv1.getAddress() )

    def testStruct(self):
        tv1 = target.module.typedVar( "structTest", target.module.g_structTest )
        self.assertEqual( 0, tv1.m_field0 )
        self.assertEqual( 500, tv1.m_field1 )
        self.assertEqual( True, tv1.m_field2 )
        self.assertEqual( 1, tv1.m_field3 )

    def testPtrField(self):
        tv = target.module.typedVar( "g_structTest" )
        self.assertEqual( 0, tv.m_field4 )

        tv1 = target.module.typedVar( "g_structTest1" )
        self.assertEqual( tv.getAddress(), tv1.m_field4 )

    def testFieldOffset(self):
        tv = target.module.typedVar( "g_structTest" )
        self.assertEqual( 0, tv.m_field0.offset() )
        self.assertEqual( 4, tv.m_field1.offset() )
        self.assertEqual( 16, tv.m_field4.offset() )
