"""Custom types tests"""

import unittest
import target
import pykd

class CustomTypesTest(unittest.TestCase):
    def testCommonStruct(self):

        tb = pykd.typeBuilder()
   
        mySubStruct =tb.createStruct("MySubCustomStruct")
        mySubStruct.append( "m_uint1", tb.UInt1B )
        mySubStruct.append( "m_uint2", tb.UInt2B )

        mySubUnion = tb.createUnion("MySubCustomUnion")
        mySubUnion.append( "m_uint1", tb.UInt1B )
        mySubUnion.append( "m_uint2", tb.UInt2B )

        myType = tb.createStruct("MyCustomStruct")
        myType.append( "m_uint1", tb.UInt1B  )
        myType.append( "m_uint4", tb.UInt4B )
        myType.append( "m_uint2", tb.UInt2B )
        myType.append( "m_struct", mySubStruct )
        myType.append( "m_union", mySubUnion )
        myType.append( "m_uint8", tb.UInt8B )

        self.assertTrue( myType.size() != 0 )
        self.assertTrue( myType.size() >= myType.fieldOffset("m_uint8") + myType.m_uint8.size() )
        
        self.assertTrue( myType.fieldOffset("m_uint1") == 0 )

        self.assertTrue( myType.fieldOffset("m_uint1") < myType.fieldOffset("m_uint4") )
        self.assertTrue( myType.fieldOffset("m_uint1") + myType.m_uint1.size() <= myType.fieldOffset("m_uint4") )

        self.assertTrue( myType.fieldOffset("m_uint4") < myType.fieldOffset("m_uint2") )
        self.assertTrue( myType.fieldOffset("m_uint4") + myType.m_uint4.size() <= myType.fieldOffset("m_uint2") )

        self.assertTrue( myType.fieldOffset("m_uint2") < myType.fieldOffset("m_struct") )
        self.assertTrue( myType.fieldOffset("m_uint2") + myType.m_uint2.size() <= myType.fieldOffset("m_struct") )

        self.assertTrue( myType.fieldOffset("m_struct") < myType.fieldOffset("m_union") )
        self.assertTrue( myType.fieldOffset("m_struct") + myType.m_struct.size() <= myType.fieldOffset("m_union") )

        # print myType

    def testCommonUnion(self):

        tb = pykd.typeBuilder()
        
        myType = tb.createUnion("MyCustomStruct")
        myType.append( "m_uint1", tb.UInt1B )
        myType.append( "m_uint4", tb.UInt4B )
        myType.append( "m_uint2", tb.UInt2B )

        self.assertFalse( myType.size() == 0 )
        self.assertTrue( myType.fieldOffset("m_uint1") == 0 )
        self.assertTrue( myType.fieldOffset("m_uint4") == 0 )
        self.assertTrue( myType.fieldOffset("m_uint2") == 0 )

    def testEmptyType(self):
        tb = pykd.typeBuilder()

        myEmptyStruct1 = tb.createStruct("EmptyStruct1")
        self.assertEqual( 0, myEmptyStruct1.size() )

        myEmptyStruct1.append("m_emptyStruct2", tb.createStruct("EmptyStruct2"))
        self.assertEqual( 0, myEmptyStruct1.size() )

        myEmptyUnion1 = tb.createUnion("EmptyUnion1")
        self.assertEqual( 0, myEmptyUnion1.size() )

        myEmptyStruct1.append("m_emptyUnion2", myEmptyUnion1)
        self.assertEqual( 0, myEmptyStruct1.size() )

        myEmptyUnion1.append("m_emptyStruct3", tb.createStruct("EmptyStruct3"))
        self.assertEqual( 0, myEmptyUnion1.size() )

    def testDupFieldName(self):
    
        tb = pykd.typeBuilder()
    
        myType = tb.createStruct("MyCustomStruct")
        exceptionRised = False
        myType.append( "m_uint1", tb.UInt1B )
        try:
            myType.append( "m_uint1", tb.UInt1B )
        except pykd.TypeException:
            exceptionRised = True
        self.assertTrue(exceptionRised)

        myType = tb.createUnion("MyCustomStruct")
        exceptionRised = False
        myType.append( "m_uint1", tb.UInt1B )
        try:
            myType.append( "m_uint1", tb.UInt1B )
        except pykd.TypeException:
            exceptionRised = True
        self.assertTrue(exceptionRised)
        
    def testBasicType(self):
        tb = pykd.typeBuilder()
        self.assertEqual( 1, tb.UInt1B.size() )
        self.assertEqual( 2, tb.UInt2B.size() )
        self.assertEqual( 4, tb.UInt4B.size() )
        self.assertEqual( 8, tb.UInt8B.size() )
        self.assertEqual( 1, tb.Int1B.size() )
        self.assertEqual( 2, tb.Int2B.size() )
        self.assertEqual( 4, tb.Int4B.size() )
        self.assertEqual( 8, tb.Int8B.size() )
        self.assertEqual( 1, tb.Bool.size() )
        self.assertEqual( 1, tb.Char.size() )
        self.assertEqual( 2, tb.WChar.size() )
        self.assertEqual( 4, tb.Long.size() )
        self.assertEqual( 4, tb.ULong.size() )

    def testVoidPtr(self):
        self.assertEqual( 4, pykd.typeBuilder(4).VoidPtr.size() )
        self.assertEqual( 8, pykd.typeBuilder(8).VoidPtr.size() )
        self.assertEqual( pykd.ptrSize(), pykd.typeBuilder().VoidPtr.size() )
