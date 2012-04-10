#
#
#

import unittest
import target
import pykd

class TypeInfoTest( unittest.TestCase ):

    def testCtor( self ):
        self.assertEqual( "structTest", pykd.typeInfo( "structTest" ).name() )
        self.assertEqual( "structTest", pykd.typeInfo( target.moduleName + "!structTest" ).name() )
        self.assertEqual( "structTest", pykd.typeInfo( "g_structTest" ).name() )
        self.assertEqual( "structTest", pykd.typeInfo( target.moduleName + "!g_structTest" ).name() )
 
    def testCreateByName( self ):
        """ creating typeInfo by the type name """
        self.assertEqual( "structTest", target.module.type( "structTest" ).name() )
        self.assertEqual( "structTest**", target.module.type( "structTest**" ).name() )
        self.assertEqual( "Int4B[2][3]", target.module.type("Int4B[2][3]").name() )
        self.assertEqual( "Int4B(*[4])[2][3]", target.module.type("Int4B(*[4])[2][3]").name() )
        self.assertEqual( "Int4B(*)[2][3]", target.module.type("Int4B((*))[2][3]").name() )

    def testCreateBySymbol(self):
        """ creating typeInfo by the symbol name """
        self.assertEqual( "structTest[2]", target.module.type("g_testArray").name() )
        self.assertEqual( "Int4B[2][3]", target.module.type("intMatrix").name() )
        self.assertEqual( "structTest*", target.module.type("g_structTestPtr").name() )
        self.assertEqual( "structTest**", target.module.type("g_structTestPtrPtr").name() )
        self.assertEqual( "Char*[2]", target.module.type("strArray").name() )
        self.assertEqual( "Char*(*)[2]", target.module.type("ptrStrArray").name() )
        self.assertEqual( "Int4B(*[4])[2][3]", target.module.type("arrIntMatrixPtrs").name() )
        self.assertEqual( "Int4B(*)[2][3]", target.module.type("ptrIntMatrix1").name() )


    def testGetField( self ):
        """ get field of the complex type """
        ti1 = target.module.type( "structTest" )
        self.assertTrue( hasattr( ti1, "m_field0" ) )
        try: hasattr(ti1, "m_field4" )                        # non-exsisting field
        except pykd.BaseException: pass   

    def testBaseTypes( self ):

        self.assertEqual("Int1B", target.module.type( "Int1B" ).name() )
        self.assertEqual("Int2B", target.module.type( "Int2B" ).name() )
        self.assertEqual("Int4B", target.module.type( "Int4B" ).name() )
        self.assertEqual("Int8B", target.module.type( "Int8B" ).name() )
        self.assertEqual("UInt1B", target.module.type( "UInt1B" ).name() )
        self.assertEqual("UInt2B", target.module.type( "UInt2B" ).name() )
        self.assertEqual("UInt4B", target.module.type( "UInt4B" ).name() )
        self.assertEqual("UInt8B", target.module.type( "UInt8B" ).name() )

        self.assertEqual("Long", target.module.type( "Long" ).name() )
        self.assertEqual("ULong", target.module.type( "ULong" ).name() )
        self.assertEqual("Bool", target.module.type( "Bool" ).name() )
        self.assertEqual("Char", target.module.type("Char").name() )
        self.assertEqual("WChar", target.module.type("WChar").name() )

        self.assertEqual( 1, target.module.type("Int1B").size() )
        self.assertEqual( 1, target.module.type("UInt1B").size() )
        self.assertEqual( 2, target.module.type("Int2B").size() )
        self.assertEqual( 2, target.module.type("UInt2B").size() )
        self.assertEqual( 4, target.module.type("Int4B").size() )
        self.assertEqual( 4, target.module.type("UInt4B").size() )
        self.assertEqual( 8, target.module.type("Int8B").size() )
        self.assertEqual( 8, target.module.type("UInt8B").size() )

        self.assertEqual( 4, target.module.type("Long" ).size() )
        self.assertEqual( 4, target.module.type("ULong" ).size() )
        self.assertEqual( 1, target.module.type("Bool" ).size() )
        self.assertEqual( 1, target.module.type("Char").size() )
        self.assertEqual( 2, target.module.type("WChar").size() )

        try:
            self.assertEqual("Int9B", target.module.type( "Int9B" ).name() )
        except pykd.SymbolException:
            pass

    def testName( self ):
        ti1 = target.module.type( "classChild" )
        self.assertEqual( "classChild", ti1.name() )
        self.assertEqual( "Int4B", ti1.m_childField.name() )
        self.assertEqual( "structTest", ti1.m_childField3.name() )
        self.assertEqual( "structTest", target.module.type("g_structTest").name() )
        
    def testVarName( self ):
        self.assertEqual( "structTest", target.module.type( "g_structTest").name() )
        self.assertRaises( pykd.TypeException, target.module.type, "g_testArray[0]" )
        self.assertRaises( pykd.TypeException, target.module.type, "*g_structTestPtr" )

    def testOffset( self ):
        ti1 = target.module.type( "structTest" )
        self.assertEqual( 0, ti1.m_field0.offset() )
        self.assertEqual( 4, ti1.m_field1.offset() )
        self.assertEqual( 12, ti1.m_field2.offset() )
        self.assertEqual( 14, ti1.m_field3.offset() )
        
    def testSize( self ):
        ti1 = target.module.type( "structTest" )
        self.assertEqual( 16 + pykd.ptrSize(), ti1.size() )
        self.assertEqual( pykd.ptrSize(), target.module.type("structTest**").size() )

    def testBitField( self ):
        ti = target.module.type( "g_structWithBits" )
        self.assertEqual( 0, ti.m_bit6_7.offset() )
        self.assertEqual( 4, ti.m_bit6_7.size() )
        self.assertEqual( "ULong:2", ti.m_bit6_7.name() )
        self.assertEqual( 2, ti.m_bit6_7.bitWidth() )
        self.assertEqual( 6, ti.m_bit6_7.bitOffset() )

    def testEnum(self):
        ti = target.module.type("enumType")
        self.assertTrue( hasattr( ti, "TWO" ) )
        self.assertEqual( 4, ti.TWO.size() )
        
        ti = target.module.type("classChild")
        self.assertEqual( "enumType", ti.m_enumField.name() )
        
    def testPtr(self):
        self.assertEqual( "listStruct1*", target.module.type( "g_listHead1" ).name() )
        self.assertEqual( "listStruct1*[2]", target.module.type( "g_arrOfListStruct1" ).name())
        self.assertEqual( "Void*",  target.module.type( "g_voidPtr" ).name() )
        self.assertEqual( "Void*[3]", target.module.type( "g_arrOfVoidPtr" ).name())
        self.assertEqual( "<function>*", target.module.type( "g_ptrToFunction" ).name())
        self.assertEqual( "<function>*[4]", target.module.type( "g_arrOfPtrToFunc" ).name())

    def testUnion(self):
        ti = target.module.type("unionTest")
        self.assertEqual( 0, ti.m_doubleValue.offset() )
        self.assertEqual( 0, ti.m_bits.offset() )
        self.assertEqual( ti.size(), ti.m_doubleValue.size() )

    def testAsMap(self):
        ti = target.module.type("enumType")
        self.assertEqual( { 1 : "ONE", 2 : "TWO", 3 : "THREE" }, ti.asMap() )

    def testDeref(self):
        ti = target.module.type("listStruct1")
        self.assertEqual( "listStruct1", ti.next.deref().name() )

        ti = target.module.type("listStruct1*")
        self.assertEqual( "listStruct1", ti.deref().name() )

        ti = target.module.type("classChild")
        self.assertRaises( pykd.BaseException, ti.deref );

    def testNestedStruct( self ):
        ti = target.module.type("StructWithNested")
        self.assertTrue( hasattr( ti, "m_field" ) )
        self.assertFalse( hasattr( ti, "m_nestedFiled" ) )

        ti = target.module.type("StructWithNested::Nested")
        self.assertTrue( hasattr( ti, "m_nestedFiled" ) )
        
    def testPrint(self):
        self.assertTrue( str(target.module.type( "g_ucharValue" ) ) )
        self.assertTrue( str(target.module.type( "g_ushortValue" ) ) )
        self.assertTrue( str(target.module.type( "g_ulongValue" ) ) )
        self.assertTrue( str(target.module.type( "g_ulonglongValue" ) ) )
        self.assertTrue( str(target.module.type( "g_structWithBits" ) ) )
        self.assertTrue( str(target.module.type( "g_structTest" ) ) )
        self.assertTrue( str(target.module.type( "g_structTest1" ) ) )
        self.assertTrue( str(target.module.type( "g_testArray" ) ) )
        self.assertTrue( str(target.module.type( "g_structTestPtr" ) ) )
        self.assertTrue( str(target.module.type( "g_structTestPtrPtr" ) ) )
        self.assertTrue( str(target.module.type( "longlongArray" ) ) )
        self.assertTrue( str(target.module.type( "intMatrix4" ) ) )
        self.assertTrue( str(target.module.type( "ptrIntMatrix" ) ) )
        self.assertTrue( str(target.module.type( "g_classChild" ) ) )
        self.assertTrue( str(target.module.type( "g_struct3" ) ) )
        self.assertTrue( str(target.module.type( "g_listHead" ) ) )
        self.assertTrue( str(target.module.type( "g_voidPtr" ) ) )
        self.assertTrue( str(target.module.type( "g_arrOfPtrToFunc" ) ) )
        self.assertTrue( str(target.module.type( "g_unTypedPtrToFunction" ) ) )
        
    def testTypedef(self):
        self.assertEqual( "structTest", pykd.typeInfo( "g_structTypeDef" ).name() )
        self.assertEqual( "structTest", pykd.typeInfo( "structTestTypeDef" ).name() )

