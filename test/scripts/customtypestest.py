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
