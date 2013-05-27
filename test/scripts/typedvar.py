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

        tv = pykd.typedVar( "structTest", target.module.g_structTest )
        tv = pykd.typedVar( target.moduleName + "!structTest", target.module.g_structTest )

        structTest = target.module.type( "structTest" )
        tv = pykd.typedVar( structTest, target.module.g_structTest )

        tv = pykd.typedVar( "g_structTest" )
        tv = pykd.typedVar( target.moduleName + "!g_structTest" )

    def testBaseTypes(self):
        self.assertEqual( 1, target.module.typedVar( "ucharVar" ) )
        self.assertEqual( 2, target.module.typedVar( "ushortVar" ) )
        self.assertEqual( 4, target.module.typedVar( "ulongVar" ) )
        self.assertEqual( 8, target.module.typedVar( "ulonglongVar" ) )
        self.assertEqual( -1, target.module.typedVar( "charVar" ) )
        self.assertEqual( -2, target.module.typedVar( "shortVar" ) )
        self.assertEqual( -4, target.module.typedVar( "longVar" ) )
        self.assertEqual( -8, target.module.typedVar( "longlongVar" ) )

    def testPtrTo(self):
        tvBaseType = pykd.typedVar( pykd.typeInfo("UInt8B").ptrTo(), target.module.offset("pbigValue") )
        self.assertEqual( target.module.typedVar( "g_ulonglongValue" ), tvBaseType.deref() )

        tvDiaStruct = pykd.typedVar( target.module.type("structTest").ptrTo(), target.module.offset("g_structTestPtr") )
        self.assertEqual( 500, tvDiaStruct.deref().m_field1 )

        customStructTest = pykd.typeBuilder().createStruct("customStructTest", 4)
        customStructTest.append("m_field0", pykd.typeInfo("UInt4B"))
        customStructTest.append("m_field1", pykd.typeInfo("UInt8B"))
        tvCustomStruct = pykd.typedVar( customStructTest.ptrTo(), target.module.offset("g_structTestPtr") )
        self.assertEqual( 500, tvCustomStruct.deref().m_field1 )

    def testArrayOf(self):
        arrayType = pykd.typeInfo("UInt8B").arrayOf(5)
        arrayVar = pykd.typedVar( arrayType, target.module.offset("ulonglongArray") )
        self.assertEqual( 0xFF, arrayVar[1] )
        self.assertEqual( 0xFFFFFFFFFFFFFFFF, arrayVar[4] )        

        arrayStructType = pykd.typeInfo("structTest").arrayOf(2) 
        arrayStructVar = pykd.typedVar( arrayStructType, target.module.offset("g_testArray") )
        self.assertEqual( True, arrayStructVar[0].m_field2 )
        self.assertEqual( 1, arrayStructVar[1].m_field3 ) 

    def testConst(self):
        self.assertEqual( True, target.module.typedVar( "g_constBoolValue" ) )
        self.assertEqual( 0x5555, target.module.typedVar( "g_constNumValue" ) )
        self.assertEqual( 3, target.module.typedVar( "g_constEnumThree" ) )
        self.assertEqual( 0xffffff, target.module.typedVar( "g_constUlong" ) )
        self.assertEqual( 0xffffff000000, target.module.typedVar( "g_constUlonglong" ) )

    def testGetAddress( self ):
        tv = target.module.typedVar( "structTest", target.module.g_structTest )
        self.assertEqual( tv.getAddress(), target.module.g_structTest )

    def testGetSize( self ):
        tv1 = target.module.typedVar( "structTest", target.module.g_structTest )
        self.assertEqual( 16 + pykd.ptrSize(), tv1.sizeof() )
        tv2 = target.module.typedVar( "structTest[2]", target.module.g_testArray )
        self.assertEqual( tv1.sizeof()*2, tv2.sizeof() )

        self.assertEqual( pykd.sizeof("g_structTest"), tv1.sizeof() )
        self.assertEqual( pykd.sizeof("g_testArray"), tv2.sizeof() )
        self.assertEqual( pykd.sizeof("ucharVar"), 1 )

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
        self.assertEqual( 0, tv.fieldOffset("m_field0") )
        self.assertEqual( 4, tv.fieldOffset("m_field1") )
        self.assertEqual( 16, tv.fieldOffset("m_field4") )

    def testArrayField(self):
        tv = target.module.typedVar( "g_structWithArray" )
        self.assertEqual( 2, len(tv.m_arrayField) )
        self.assertEqual( 0, tv.m_arrayField[0] )
        self.assertEqual( 2, tv.m_arrayField[1] )
        self.assertEqual( 3, tv.m_noArrayField )
        self.assertNotEqual( -1, tv.m_arrayField[0] )
        self.assertNotEqual( 0, tv.m_noArrayField )
        try:
            tv.m_arrayField[len(tv.m_arrayField)]
            self.assertTrue(False)
        except IndexError:
            self.assertTrue(True)

    #def testArrayFieldSlice(self):
    #    tv = target.module.typedVar( "g_structWithArray" )
    #    self.assertEqual( [ 0, 2 ], tv.m_arrayField[0:2] )

    #def testArrayFieldSliceNegative(self):
    #    tv = target.module.typedVar( "g_structWithArray" )
    #    self.assertEqual( 2, tv.m_arrayField[-1] )

    def testGlobalVar(self):
        self.assertEqual( 10002000, target.module.typedVar( "ulongVar" ) )
        self.assertEqual( 0x80000000, target.module.typedVar( "ulongArray" )[3] )
        self.assertEqual( 0x8000000000000000, target.module.typedVar( "ulonglongArray" )[3] )
        self.assertEqual( 0x7FFFFFFF, target.module.typedVar( "longArray" )[3])
        self.assertEqual( -1, target.module.typedVar( "longlongArray" )[4])
        self.assertEqual( target.module.g_structTest, target.module.typedVar( "g_structTestPtr" ) )

    def testContainingRecord(self):
        off1 = target.module.type( "structTest" ).fieldOffset("m_field2")
        off2 = target.module.offset( "g_structTest" )
        tv = target.module.containingRecord( off2 + off1, "structTest", "m_field2" )
        self.assertEqual( True, tv.m_field2 )

    def testBitField(self):
        tv = target.module.typedVar("g_structWithBits")
        self.assertEqual( 4, tv.m_bit0_4 )
        self.assertEqual( 1, tv.m_bit5 )
        self.assertEqual( 5, tv.m_bit6_8 )
        tv = target.module.typedVar("g_structWithSignBits")
        self.assertEqual( 4, tv.m_bit0_4 )
        self.assertEqual( -1, tv.m_bit5 )
        self.assertEqual( -3, tv.m_bit6_8 )

    def testTypedVarList(self):
        tvl = target.module.typedVarList( target.module.g_listHead, "listStruct", "listEntry" )
        self.assertEqual( 3, len( tvl ) )
        self.assertEqual( [1,2,3], [ tv.num for tv in tvl ] )

        tvl = pykd.typedVarList( target.module.g_listHead, target.module.type("listStruct"), "listEntry" )
        self.assertEqual( 3, len( tvl ) )
        self.assertEqual( [1,2,3], [ tv.num for tv in tvl ] )

        tvl = pykd.typedVarList( target.module.g_listHead, target.module.type("listStruct"), "listEntry.Flink" )
        self.assertEqual( 3, len( tvl ) )
        self.assertEqual( [1,2,3], [ tv.num for tv in tvl ] )

        tvl = target.module.typedVarList( target.module.g_listHead1, "listStruct1", "next" )
        self.assertEqual( 3, len( tvl ) )
        self.assertEqual( [100,200,300], [ tv.num for tv in tvl ] )

        tvl = pykd.typedVarList( target.module.g_listHead1, target.module.type("listStruct1"), "next" )
        self.assertEqual( 3, len( tvl ) )
        self.assertEqual( [100,200,300], [ tv.num for tv in tvl ] )

        tvl = pykd.typedVarList( target.module.g_childListHead, target.module.type("ChildEntryTest"), "m_next" )
        self.assertEqual( 3, len( tvl ) )
        self.assertEqual( [1000,2000,3000], [ tv.m_someBaseFiled2 for tv in tvl ] )
        self.assertEqual( [1001,2001,3001], [ tv.m_childFiled1 for tv in tvl ] )

        tvl1 = target.module.typedVarList( target.module.g_listHead, "listStruct", "listEntry" )
        tvl2 = pykd.typedVarList( target.module.g_listHead, target.moduleName + "!listStruct", "listEntry" )
        self.assertEqual( tvl1, tvl2 )

    def testTypedVarArray(self):
        tvl = target.module.typedVarArray( target.module.g_testArray, "structTest", 2 )
        self.assertEqual( 2, len( tvl ) )
        self.assertEqual( 500, tvl[0].m_field1 )
        self.assertEqual( False, tvl[1].m_field2 )

        tvl = pykd.typedVarArray( target.module.g_testArray, target.module.type("structTest"), 2 )
        self.assertEqual( 2, len( tvl ) )
        self.assertEqual( 1, tvl[0].m_field3 )
        self.assertEqual( 0, tvl[1].m_field4 )
        
        tvl1 = target.module.typedVarArray( target.module.g_testArray, "structTest", 2 )
        tvl2 = pykd.typedVarArray( target.module.g_testArray, target.moduleName + "!structTest", 2 )
        self.assertEqual( tvl1, tvl2 )

    def testEqual(self):
        tv1 = target.module.typedVar("g_structTest")
        tv2 = target.module.typedVar("intMatrix")
        self.assertEqual( tv1.m_field3, tv2[0][1] )

    def testEnum(self):
        tv = target.module.typedVar("g_constEnumThree")
        self.assertEqual( 3, tv )
        self.assertEqual( target.module.type("enumType").THREE, tv )

    def testIndex(self):
        ind  = target.module.typedVar( "ucharVar" )
        self.assertEqual( 5, [0,5,10][ind] )
      
        self.assertTrue( ind in [0,1,2] )
    
        tv = target.module.typedVar( "g_structWithArray" )
        self.assertEqual( 2, tv.m_arrayField[ind] )
        
        ind = target.module.typedVar( "ulongValue" )
        self.assertEqual( 4, ind )
        self.assertTrue( ind in { 1 : "1", 4 : "2" } )
        self.assertEqual( "2", { 1 : "1", 4 : "2" }[ind] )

    def testDeref(self):
        tv = target.module.typedVar( "g_structTest1" )
        self.assertEqual( target.module.g_structTest, tv.m_field4.deref().getAddress() )

        tv = target.module.typedVar( "g_structTest" )
        self.assertEqual( 0, tv.m_field4.deref().getAddress() )

        try:
            tv.m_field1.deref()
            self.assertTrue(False)
        except pykd.TypeException: 
            pass
            
    def testSkipDeref(self):
        tv = target.module.typedVar( "g_structTest1" )
        self.assertEqual( tv.m_field4.deref().m_field1, tv.m_field4.m_field1 )

    def testUnNamedStruct(self):
        tv = target.module.typedVar( "g_unNamedStruct" )
        self.assertEqual( 4, tv.m_fieldNestedStruct )
        self.assertEqual( 5, tv.m_fieldOfUnNamed )

    def testPointerToFunction(self):
        tv1 = target.module.typedVar( "g_unTypedPtrToFunction" )

        # if debug: g_unTypedPtrToFunction point to jmp EnumWindowsProc2 (e9 xxxxxxxx)
        self.assertTrue( ( target.module.offset("EnumWindowsProc2") == tv1 ) or
                         ( 0xE9 == pykd.ptrByte( long(tv1) ) ) )

        tv2 = target.module.typedVar( "g_unTypedPtrToFunction" )
        self.assertEqual( tv1, tv2 )

        self.assertRaises( pykd.TypeException, tv1.deref )
        self.assertRaises( pykd.TypeException, tv2.deref )

    def testTypeVarArg(self):
        tv1 = target.module.typedVar( "structTest", target.module.g_structTest )
        tv2 = target.module.typedVar( "structTest", tv1 )
        self.assertEqual( tv1, tv2 )  
        self.assertTrue( tv1 )

    def testPrint(self):
        self.assertTrue( str(target.module.typedVar( "ucharVar" ) ) )
        self.assertTrue( str(target.module.typedVar( "ushortVar" ) ) )
        self.assertTrue( str(target.module.typedVar( "ulongVar" ) ) )
        self.assertTrue( str(target.module.typedVar( "ulonglongVar" ) ) )
        self.assertTrue( str(target.module.typedVar( "g_structWithBits" ) ) )
        self.assertTrue( str(target.module.typedVar( "g_structTest" ) ) )
        self.assertTrue( str(target.module.typedVar( "g_structTest1" ) ) )
        self.assertTrue( str(target.module.typedVar( "g_testArray" ) ) )
        self.assertTrue( str(target.module.typedVar( "g_structTestPtr" ) ) )
        self.assertTrue( str(target.module.typedVar( "g_structTestPtrPtr" ) ) )
        self.assertTrue( str(target.module.typedVar( "longlongArray" ) ) )
        self.assertTrue( str(target.module.typedVar( "intMatrix4" ) ) )
        self.assertTrue( str(target.module.typedVar( "ptrIntMatrix" ) ) )
        self.assertTrue( str(target.module.typedVar( "g_classChild" ) ) )
        self.assertTrue( str(target.module.typedVar( "g_struct3" ) ) )
        self.assertTrue( str(target.module.typedVar( "g_listHead" ) ) )
        self.assertTrue( str(target.module.typedVar( "g_voidPtr" ) ) )
        self.assertTrue( str(target.module.typedVar( "g_arrOfPtrToFunc" ) ) )
        self.assertTrue( str(target.module.typedVar( "g_unTypedPtrToFunction" ) ) )
        
    def testNotValidPrint(self):
        types = ("structTest", "ULong[100]", "ULong*" )
        for ti in types:
            self.assertTrue( str(pykd.typedVar( target.module.type(ti), 0 ) ) )

    def testStaticField(self):
        tv = pykd.typedVar( "g_classChild" )
        self.assertEqual( 200, tv.m_staticField )
        self.assertEqual( 100, tv.m_staticConst )

    def testAmbiguousFieldAccess(self):
        derivedFiledVal = pykd.loadCStr( pykd.typedVar( "g_fieldSameNameStruct" ).m_field )
        self.assertEqual( derivedFiledVal, "toaster" )

    def testDiamondVirtualInherit(self):
        tv = pykd.typedVar( "g_virtChild" )
        self.assertEqual( -100, tv.m_baseField )

    def testDinkumwareMap(self):
        g_map = target.module.typedVar( "g_map" )
        self.assertEqual( 1, g_map._Mysize )

    def testUdtSubscribe(self):
        tv = pykd.typedVar( "g_virtChild" )
        self.assertEqual( 5, len(tv) )
        fieldName, fieldVal = tv[4]
        self.assertEqual( fieldName, "m_baseField" )
        self.assertEqual( fieldVal, tv.m_baseField )
        for field in tv:
             str( field )

    def testDeadlockList(self):
       lst = []
       entry = pykd.typedVar("entry1").Flink
       for i in range( 0, 100000 ):
           lst.append(entry)
           entry = entry.deref().Flink

    def testWrongArgs(self):
        self.assertRaises( pykd.TypeException, pykd.typedVar, None, 0 )
        self.assertRaises( pykd.TypeException, pykd.typedVarList, target.module.g_listHead1, None, "next" )
        self.assertRaises( pykd.TypeException, pykd.typedVarArray, target.module.g_testArray, None, 2 )
        self.assertRaises( pykd.TypeException, pykd.containingRecord, target.module.offset( "g_structTest" ), None, "m_field2" )
