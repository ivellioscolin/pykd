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
        tv2 = target.module.typedVar( "structTest[2]", target.module.g_testArray )
        self.assertEqual( tv1.sizeof()*2, tv2.sizeof() )

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

    def testArrayField(self):
        tv = target.module.typedVar( "g_struct3" )
        self.assertEqual( 2, len(tv.m_arrayField) )
        self.assertEqual( 0, tv.m_arrayField[0] )
        self.assertEqual( 2, tv.m_arrayField[1] )
        self.assertEqual( 3, tv.m_noArrayField )
        self.assertNotEqual( -1, tv.m_arrayField[0] )
        self.assertNotEqual( 0, tv.m_noArrayField )    
        
    def testGlobalVar(self):
        self.assertEqual( 4, target.module.typedVar( "g_ulongValue" ) )
        self.assertEqual( 0x80000000, target.module.typedVar( "ulongArray" )[3] )
        self.assertEqual( 0x8000000000000000, target.module.typedVar( "ulonglongArray" )[3] )
        self.assertEqual( -100000,  target.module.typedVar( "longArray" )[3])
        self.assertEqual( -10000000000, target.module.typedVar( "longlongArray" )[4])
        self.assertEqual( target.module.g_structTest, target.module.typedVar( "g_structTestPtr" ) )

    def testContainingRecord(self):
        off1 = target.module.type( "structTest" ).m_field2.offset()
        off2 = target.module.offset( "g_structTest" )
        tv = target.module.containingRecord( off2 + off1, "structTest", "m_field2" )
        self.assertEqual( True, tv.m_field2 )
        
    def testBitField(self):
        tv = target.module.typedVar("g_structWithBits")
        self.assertEqual( 4, tv.m_bit0_4 )
