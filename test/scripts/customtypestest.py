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


