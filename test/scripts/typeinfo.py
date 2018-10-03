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
        self.assertEqual( "Int1B", pykd.typeInfo( "Int1B" ).name() )	
 
    def testCreateByName( self ):
        """ creating typeInfo by the type name """
        self.assertEqual( "Int4B*", target.module.type("Int4B*").name() )
        self.assertEqual( "structTest", target.module.type( "structTest" ).name() )
        self.assertEqual( "structTest**", target.module.type( "structTest**" ).name() )
        self.assertEqual( "Int4B[2][3]", target.module.type("Int4B[2][3]").name() )
        self.assertEqual( "Int4B(*[4])[2][3]", target.module.type("Int4B(*[4])[2][3]").name() )
        self.assertEqual( "Int4B(*)[2][3]", target.module.type("Int4B((*))[2][3]").name() )
        self.assertEqual( "Int4B*", pykd.typeInfo("Int4B*").name() )

    def testCreateBySymbol(self):
        """ creating typeInfo by the symbol name """
        self.assertEqual( "structTest[2]", target.module.type("g_testArray").name() )
        self.assertEqual( "Int4B[2][3]", target.module.type("intMatrix").name() )
        self.assertEqual( "structTest*", target.module.type("g_structTestPtr").name() )
        self.assertEqual( "structTest**", target.module.type("g_structTestPtrPtr").name() )
        self.assertEqual( "Char*[2]", target.module.type("strArray").name() )
        self.assertEqual( "Char*(*)[2]", target.module.type("ptrStrArray").name() )
        self.assertEqual( "Int4B(*[4])[2][3]", target.module.type("arrIntMatrixPtrs").name() )
        self.assertEqual( "Int4B(*)[2][3]", target.module.type("ptrIntMatrix").name() )

    def testGetField( self ):
        """ get field of the complex type """
        ti1 = target.module.type( "structTest" )
        self.assertTrue( "UInt4B", ti1.m_field0.name() )
        self.assertTrue( "UInt4B", ti1["m_field0"].name() )
        self.assertTrue( hasattr( ti1, "m_field0" ) )
        self.assertFalse( hasattr( ti1, "not_exists" ) )
        self.assertRaises( AttributeError, lambda t: t.not_exists, ti1) # non-exsisting field
        self.assertRaises( KeyError, lambda t: t["not_exists"], ti1) # non-exsisting field


    def testBaseTypes( self ):
        self.assertEqual("Int1B", pykd.typeInfo( "Int1B" ).name() )
        self.assertEqual("Int2B", pykd.typeInfo( "Int2B" ).name() )
        self.assertEqual("Int4B", pykd.typeInfo( "Int4B" ).name() )
        self.assertEqual("Int8B", pykd.typeInfo( "Int8B" ).name() )
        self.assertEqual("UInt1B", pykd.typeInfo( "UInt1B" ).name() )
        self.assertEqual("UInt2B", pykd.typeInfo( "UInt2B" ).name() )
        self.assertEqual("UInt4B", pykd.typeInfo( "UInt4B" ).name() )
        self.assertEqual("UInt8B", pykd.typeInfo( "UInt8B" ).name() )

        self.assertEqual("Long", pykd.typeInfo( "Long" ).name() )
        self.assertEqual("ULong", pykd.typeInfo( "ULong" ).name() )
        self.assertEqual("Bool", pykd.typeInfo( "Bool" ).name() )
        self.assertEqual("Char", pykd.typeInfo("Char").name() )
        self.assertEqual("WChar", pykd.typeInfo("WChar").name() )

        self.assertEqual( 1, pykd.typeInfo("Int1B").size() )
        self.assertEqual( 1, pykd.typeInfo("UInt1B").size() )
        self.assertEqual( 2, pykd.typeInfo("Int2B").size() )
        self.assertEqual( 2, pykd.typeInfo("UInt2B").size() )
        self.assertEqual( 4, pykd.typeInfo("Int4B").size() )
        self.assertEqual( 4, pykd.typeInfo("UInt4B").size() )
        self.assertEqual( 8, pykd.typeInfo("Int8B").size() )
        self.assertEqual( 8, pykd.typeInfo("UInt8B").size() )

        self.assertEqual( 4, pykd.typeInfo("Long" ).size() )
        self.assertEqual( 4, pykd.typeInfo("ULong" ).size() )
        self.assertEqual( 1, pykd.typeInfo("Bool" ).size() )
        self.assertEqual( 1, pykd.typeInfo("Char").size() )
        self.assertEqual( 2, pykd.typeInfo("WChar").size() )

        try:
            self.assertEqual("Int9B", pykd.typeInfo( "Int9B" ).name() )
        except pykd.SymbolException:
            pass
            
    def testBaseTypePtr(self):
        self.assertEqual("Int1B*", pykd.typeInfo( "Int1B*" ).name() )
        self.assertEqual("Int1B", pykd.typeInfo( "Int1B*" ).deref().name() )
        
    def testBaseTypeArray(self):
        self.assertEqual("Int4B[20]", pykd.typeInfo( "Int4B[20]" ).name() )

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
        self.assertEqual( 0, ti1.fieldOffset("m_field0") )
        self.assertEqual( 4, ti1.fieldOffset("m_field1") )
        self.assertEqual( 12, ti1.fieldOffset("m_field2") )
        self.assertEqual( 14, ti1.fieldOffset("m_field3") )

        ti2 = target.module.type( "unionTest" )
        self.assertEqual( 0, ti2.fieldOffset("m_value") )
        self.assertEqual( 0, ti2.fieldOffset("m_structValue") )

        ti3 = target.module.type( "structWithNested" )
        self.assertEqual( ti3.fieldOffset( "m_unnameStruct"), ti3.fieldOffset( "m_unnameStruct.m_field2" ) )

    def testSize( self ):
        ti1 = target.module.type( "structTest" )
        self.assertEqual( 16 + pykd.ptrSize(), ti1.size() )
        self.assertEqual( pykd.ptrSize(), target.module.type("structTest**").size() )
        self.assertEqual( pykd.sizeof("structTest"), target.module.type("structTest").size() )
        self.assertEqual( pykd.sizeof("structTest**"), target.module.type("structTest**").size() )
        self.assertEqual( pykd.sizeof("Int1B"), pykd.typeInfo("Int1B").size() )

    def testBitField( self ):
        ti = target.module.type( "g_structWithBits" )
        self.assertEqual( 0, ti.fieldOffset("m_bit6_8") )
        self.assertEqual( 4, ti.m_bit6_8.size() )
        self.assertEqual( "UInt4B:3", ti.m_bit6_8.name() )
        self.assertEqual( 3, ti.m_bit6_8.bitWidth() )
        self.assertEqual( 6, ti.m_bit6_8.bitOffset() )

    def testEnum(self):
        ti = target.module.type("enumType")
        self.assertTrue( hasattr( ti, "TWO" ) )
        self.assertEqual( 4, ti.TWO.size() )

        ti = target.module.type("classChild")
        self.assertEqual( "enumType", ti.m_enumField.name() )

    def testPtr(self):
        self.assertEqual( "UInt8B*", target.module.type( "pbigValue" ).name() )
        self.assertEqual( "structTest*", target.module.type( "structTest*" ).name() )
        self.assertEqual( "UInt2B*", pykd.typeInfo( "UInt2B*" ).name() )
        self.assertEqual( "Void*",  target.module.type( "voidPtr" ).name() )
        self.assertEqual( "Void*[3]", target.module.type( "voidPtrArray" ).name())
        #self.assertEqual( "<function>*", target.module.type( "g_ptrToFunction" ).name())
        #self.assertEqual( "<function>*[4]", target.module.type( "g_arrOfPtrToFunc" ).name())

    def testUnion(self):
        ti = target.module.type("unionTest")
        self.assertEqual( 0, ti.fieldOffset("m_doubleValue") )
        self.assertEqual( 0, ti.fieldOffset("m_value") )
        self.assertEqual( 0, ti.fieldOffset("m_structValue") )
        self.assertEqual( ti.size(), ti.m_structValue.size() )

    def testDeref(self):
        ti = pykd.typeInfo("Int1B*")
        self.assertEqual( "Int1B", ti.deref().name() )

        ti = target.module.type("structTest*")
        self.assertEqual( "structTest", ti.deref().name() )

        ti =  pykd.typeInfo("structTest[2]")
        self.assertRaises( pykd.TypeException, ti.deref );

        ti = target.module.type("classChild")
        self.assertRaises( pykd.TypeException, ti.deref );

    def testNestedStruct( self ):
        ti = target.module.type("structWithNested")
        self.assertTrue( hasattr( ti, "m_field" ) )
        self.assertTrue( hasattr( ti, "m_field3" ) )
        self.assertTrue( hasattr( ti, "m_unnameStruct" ) )
        self.assertTrue( hasattr( ti.m_unnameStruct, "m_field2" ) )
        self.assertFalse( hasattr( ti, "m_field2" ) )
        self.assertFalse( hasattr( ti, "m_nestedFiled" ) )
        ti = target.module.type("structWithNested::Nested")
        self.assertTrue( hasattr( ti, "m_nestedFiled" ) )

    def testPrint(self):
        self.assertTrue( str(target.module.type( "ucharVar" ) ) )
        self.assertTrue( str(target.module.type( "ushortVar" ) ) )
        self.assertTrue( str(target.module.type( "ulongVar" ) ) )
        self.assertTrue( str(target.module.type( "ulonglongVar" ) ) )
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
#        self.assertTrue( str(target.module.type( "g_listHead" ) ) )
#        self.assertTrue( str(target.module.type( "g_voidPtr" ) ) )
#        self.assertTrue( str(target.module.type( "g_arrOfPtrToFunc" ) ) )
#        self.assertTrue( str(target.module.type( "g_unTypedPtrToFunction" ) ) )

    def testTypedef(self):
        self.assertEqual( "structTest", pykd.typeInfo( "g_structTypeDef" ).name() )
        self.assertEqual( "structTest", pykd.typeInfo( "structTestTypeDef" ).name() )

    def testStaticField(self):
        ti = pykd.typeInfo( "g_classChild" )
        self.assertNotEqual( 0, ti.staticOffset( "m_staticField" ) )
        self.assertNotEqual( 0, ti.staticOffset("m_stdstr") )
        if not ti.staticOffset("m_staticConst"):
            self.assertFalse( "MS DIA bug: https://connect.microsoft.com/VisualStudio/feedback/details/737430" )

    def testVfnTable(self):
        ti = pykd.typeInfo( "g_classChild" )
        self.assertTrue( hasattr( ti, "__VFN_table" ) )

    def testUdtSubscribe(self):
        ti = pykd.typeInfo( "g_virtChild" )
        self.assertEqual( 6, len(ti) )
        for field in ti:
             str( field )

    def testEnumSubscribe(self):
        ti = pykd.typeInfo( "enumType" )
        self.assertEqual( 3, len(ti) )
        self.assertEqual( [ 1, 2, 3], [ int(field) for field in ti ] )
        self.assertEqual( [ ( "ONE", 1), ("TWO", 2), ("THREE", 3) ], ti.fields() )

    def testStructNullSize(self):
        ti = target.module.type("structNullSize")
        self.assertEqual( 0, len(ti) )

    def testDerefName(self):
        entry = pykd.typedVar("g_listHead").flink
        self.assertEqual( "listEntry*", entry.type().name() )

    def testPtrTo(self):
        ti = pykd.typeInfo("UInt8B").ptrTo()
        self.assertTrue( "UInt8B*", ti.name() )
        self.assertNotEqual( 0, ti.size() )

    def testArrayOf(self):
        ti = pykd.typeInfo("UInt8B").arrayOf(10)
        self.assertTrue( "UInt8B[10]", ti.name() )
        
    def testCompareWihNone(self):
        ti = pykd.typeInfo("UInt8B")
        if ti == None:
            pass
        if ti != None:
            pass
        if not ti:
            pass
        if ti:
            pass

    def testFunction(self):
        functype = target.module.typedVar( "CdeclFuncPtr" ).type().deref()
        self.assertTrue( functype.isFunction() )

        functype = target.module.typedVar( "g_variadicFuncPtr" ).type().deref()
        self.assertTrue( functype.isFunction() )

    def testFunctionArgs(self):
        functype = target.module.typedVar( "CdeclFuncPtr" ).type().deref()
        self.assertEqual( [ arg.name() for arg in functype ], ["Int4B", "Float"] )

        functype = target.module.typedVar( "g_variadicFuncPtr" ).type().deref()
        self.assertEqual( [ arg.name() for arg in functype ], ["Int4B", "NoType"] )

    def testFunctionCallConv(self):
        functype = target.module.typedVar( "CdeclFuncPtr" ).type().deref()
        self.assertEqual( functype.getCallingConvention(), pykd.callingConvention.NearC )

    def testFunctionThis(self):
        functype = target.module.typedVar( "MethodPtr" ).type().deref()
        self.assertEqual( [ arg.name() for arg in functype ], ["FuncTestClass*"] )

        functype = target.module.typedVar( "CdeclStaticMethodPtr" ).type().deref()
        self.assertEqual( [ arg.name() for arg in functype ], [] )

    def testFunctionName(self):
        functype = target.module.typedVar( "CdeclFuncPtr" ).type().deref()
        self.assertEqual(functype.name(), "Void(__cdecl)(Int4B, Float)")

        functype = target.module.typedVar( "MethodPtr" ).type().deref()
        self.assertEqual(functype.name(), "Void(__thiscall FuncTestClass::)()")

        functype = target.module.typedVar( "ArrayOfCdeclFuncPtr" ).type()[0].deref()
        self.assertEqual(functype.name(), "Void(__cdecl)(Int4B, Float)")

        functype = target.module.typedVar( "ArrayOfMethodPtr" ).type()[0].deref()
        self.assertEqual(functype.name(), "Void(__thiscall FuncTestClass::)()")

        functype = target.module.typedVar( "g_variadicFuncPtr" ).type().deref()
        self.assertEqual(functype.name(), "Void(__cdecl)(Int4B, ...)")

    def testFunctionPtrName(self):
        funcptrtype = target.module.typedVar( "CdeclFuncPtr" ).type()
        self.assertEqual(funcptrtype.name(), "Void(__cdecl*)(Int4B, Float)")

        functype = target.module.typedVar( "MethodPtr" ).type()
        self.assertEqual(functype.name(), "Void(__thiscall FuncTestClass::*)()")

        funcptrtype = target.module.typedVar( "g_variadicFuncPtr" ).type()
        self.assertEqual(funcptrtype.name(), "Void(__cdecl*)(Int4B, ...)")

    def testFunctionArrName(self):
        funcptrtype = target.module.typedVar( "ArrayOfCdeclFuncPtr" ).type()
        self.assertEqual(funcptrtype.name(), "Void(__cdecl*[3])(Int4B, Float)")

        functype = target.module.typedVar( "ArrayOfMethodPtr" ).type()
        self.assertEqual(functype.name(), "Void(__thiscall FuncTestClass::*[2])()")

    def testDir(self):
        ti = target.module.type("structTest")
        self.assertEqual(5, len(dir(ti)))
        self.assertTrue("m_field3" in dir(ti))
        self.assertFalse("m_field33" in dir(ti))

    def testEnumTypes(self):
        lst = target.module.enumTypes()
        self.assertNotEqual([], lst)
        lst = target.module.enumTypes("structTest")
        self.assertEqual(["structTest"], lst)
        lst = target.module.enumTypes("NonExsistType")
        self.assertEqual([],lst)

    def testArrayOverflow(self):
        self.assertRaises(pykd.TypeException, pykd.baseTypes.UInt8B.arrayOf, 0xFFFFFFFFFFFFFFFF)

    def testMethodCount(self):
        self.assertEqual( 14, target.module.type("classChild").getNumberMethods() )
        
    def testGetMethod(self):
        self.assertEqual( "Int4B(__thiscall classChild::)(Int4B)", target.module.type("classChild").method("childMethod").name() )
        self.assertEqual( "Int4B(__thiscall classChild::)(Int4B)", target.module.type("classChild").childMethod.name() )
        self.assertEqual( "Int4B(__thiscall classChild::)(Int4B)", target.module.type("classChild").method(1).name() )

    def testMethods(self):
        self.assertEqual( 14, len(target.module.type("classChild").methods()))

    def testGetBaseClass(self):
        classChild = target.module.type("classChild")
        self.assertEqual( ["classBase1", "classBase2"], [ classChild.baseClass(i).name() for i in range(classChild.getNumberBaseClasses()) ] )
        self.assertEqual( ["classBase1", "classBase2"], [ name for name, _, _ in classChild.baseClasses()] )

    def testGetBaseClassOffset(self):
        classChild = target.module.type("classChild")
        self.assertEqual( classChild.baseClassOffset(0), classChild.baseClassOffset('classBase1'))
        self.assertEqual(classChild.baseClassOffset(1), classChild.baseClassOffset('classBase2'))


    def  testPdbTypeProvider(self):
        pdb = target.module.symfile()
        typeProvider = pykd.getTypeInfoProviderFromPdb(pdb)
        self.assertEqual("structTest", typeProvider.getTypeByName("structTest").name())
        self.assertEqual("structTest", typeProvider.structTest.name())
        self.assertEqual(22, len(list(typeProvider.typeIterator("*struct*"))))

    def testScopeName(self):
        self.assertEqual( target.module.name(), pykd.typeInfo( "structTest" ).scopeName() )
        self.assertEqual( target.module.name(), pykd.typeInfo( "structWithNested::Nested" ).scopeName() )
        


