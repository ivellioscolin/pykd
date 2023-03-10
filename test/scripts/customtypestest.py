"""Custom types tests"""

import unittest
import target
import pykd
from pykd import baseTypes

class CustomTypesTest(unittest.TestCase):
    def testCommonStruct(self):
        mySubStruct =pykd.createStruct("MySubCustomStruct")
        mySubStruct.append( "m_uint1", baseTypes.UInt1B )
        mySubStruct.append( "m_uint2", baseTypes.UInt2B )

        mySubUnion = pykd.createUnion("MySubCustomUnion")
        mySubUnion.append( "m_uint1", baseTypes.UInt1B )
        mySubUnion.append( "m_uint2", baseTypes.UInt2B )

        myType =pykd.createStruct("MyCustomStruct")
        myType.append( "m_uint1", baseTypes.UInt1B  )
        myType.append( "m_uint4", baseTypes.UInt4B )
        myType.append( "m_uint2", baseTypes.UInt2B )
        myType.append( "m_struct", mySubStruct )
        myType.append( "m_union", mySubUnion )
        myType.append( "m_uint8", baseTypes.UInt8B )

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

    def testCommonUnion(self):

        myType = pykd.createUnion("MyCustomStruct")
        myType.append( "m_uint1", baseTypes.UInt1B )
        myType.append( "m_uint4", baseTypes.UInt4B )
        myType.append( "m_uint2", baseTypes.UInt2B )

        self.assertFalse( myType.size() == 0 )
        self.assertTrue( myType.fieldOffset("m_uint1") == 0 )
        self.assertTrue( myType.fieldOffset("m_uint4") == 0 )
        self.assertTrue( myType.fieldOffset("m_uint2") == 0 )

    def testEmptyType(self):
    
        myEmptyStruct1 = pykd.createStruct("EmptyStruct1")
        self.assertEqual( 0, myEmptyStruct1.size() )

        myEmptyStruct1.append("m_emptyStruct2", pykd.createStruct("EmptyStruct2"))
        self.assertEqual( 0, myEmptyStruct1.size() )

        myEmptyUnion1 = pykd.createUnion("EmptyUnion1")
        self.assertEqual( 0, myEmptyUnion1.size() )

        myEmptyStruct1.append("m_emptyUnion2", myEmptyUnion1)
        self.assertEqual( 0, myEmptyStruct1.size() )

        myEmptyUnion1.append("m_emptyStruct3", pykd.createStruct("EmptyStruct3"))
        self.assertEqual( 0, myEmptyUnion1.size() )

    def testDupFieldName(self):
    
        myType = pykd.createStruct("MyCustomStruct")
        exceptionRised = False
        myType.append( "m_uint1", baseTypes.UInt1B )
        try:
            myType.append( "m_uint1", baseTypes.UInt1B )
        except pykd.TypeException:
            exceptionRised = True
        self.assertTrue(exceptionRised)

        myType = pykd.createUnion("MyCustomStruct")
        exceptionRised = False
        myType.append( "m_uint1", baseTypes.UInt1B )
        try:
            myType.append( "m_uint1", baseTypes.UInt1B )
        except pykd.TypeException:
            exceptionRised = True
        self.assertTrue(exceptionRised)

    def testPtrToCustomType(self):
        mySubStruct =pykd.createStruct("MySubCustomStruct")
        mySubStruct.append( "m_uint1", baseTypes.UInt1B )
        mySubStruct.append( "m_uint2", baseTypes.UInt2B )
        mySubStructPtr = mySubStruct.ptrTo()
        self.assertEqual( pykd.ptrSize(), mySubStructPtr.size() )

    def testAlign(self):
        struct = pykd.createStruct(name ="MyAlignStruct", align=4)
        struct.append( "m_field1", baseTypes.UInt1B )
        self.assertEqual( 1, struct.size() )
        struct.append( "m_field2", baseTypes.UInt1B )
        self.assertEqual( 2, struct.size() )
        struct.append( "m_field3", baseTypes.UInt1B )
        struct.append( "m_field4", baseTypes.UInt2B )
        self.assertEqual( 6, struct.size() )
        struct.append( "m_field5", baseTypes.UInt4B )
        self.assertEqual( 12, struct.size() )
        struct.append( "m_field6", baseTypes.UInt1B )
        self.assertEqual( 16, struct.size() )
        struct.append( "m_field7", baseTypes.UInt1B.arrayOf(5) )
        self.assertEqual( 20, struct.size() )

        struct = pykd.createStruct(name ="MyAlignStruct", align=1)
        struct.append( "m_field1", baseTypes.UInt1B )
        struct.append( "m_field2", baseTypes.UInt8B )
        self.assertEqual( 9, struct.size() )

    def testUnionAlignedSize(self):
        union = pykd.createUnion("MyCustomUnion", align=4)
        union.append( "m_field1", baseTypes.UInt2B )
        self.assertEqual( 2, union.size() )
        union.append( "m_field2", baseTypes.UInt1B.arrayOf(3) )
        self.assertEqual( 4, union.size() )

    def testWi12591(self):
        struct = pykd.createStruct(name ="MyAlignStruct", align=4)
        struct.append( "m_field1", baseTypes.UInt1B )
        struct.append( "m_field2", baseTypes.UInt1B.arrayOf(2) )
        self.assertEqual( struct.size(), 3 )
        self.assertEqual( struct.fieldOffset("m_field2"), 1 )

    def testWi12592(self):
        struct = pykd.createStruct(name ="MyAlignStruct", align=4)
        struct.append( "field1", baseTypes.UInt4B )
        struct.append( "field2", baseTypes.UInt1B )
        self.assertEqual( struct.size(), 8 )
        self.assertEqual( struct.fieldOffset("field2"), 4 )

    def testCustomFunction(self):
        functype = pykd.defineFunction( baseTypes.UInt4B )
        functype.append( "var1", baseTypes.WChar)
        functype.append( "var2", baseTypes.UInt4B.ptrTo() )
        self.assertEqual( "UInt4B(__cdecl)(WChar, UInt4B*)", functype.name() )
