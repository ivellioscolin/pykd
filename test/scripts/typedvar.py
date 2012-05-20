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
        self.assertEqual( 1, target.module.typedVar( "g_ucharValue" ) )
        self.assertEqual( 2, target.module.typedVar( "g_ushortValue" ) )
        self.assertEqual( 4, target.module.typedVar( "g_ulongValue" ) )
        self.assertEqual( 8, target.module.typedVar( "g_ulonglongValue" ) )
        self.assertEqual( -1, target.module.typedVar( "g_charValue" ) )
        self.assertEqual( -2, target.module.typedVar( "g_shortValue" ) )
        self.assertEqual( -4, target.module.typedVar( "g_longValue" ) )
        self.assertEqual( -8, target.module.typedVar( "g_longlongValue" ) )

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
        try:
            tv.m_arrayField[len(tv.m_arrayField)]
            self.assertTrue(False)
        except IndexError:
            self.assertTrue(True)
            
    def testArrayFieldSlice(self): 
        tv = target.module.typedVar( "g_struct3" )
        self.assertEqual( 2, tv.m_arrayField[-1] )
        self.assertEqual( [ 0, 2 ], tv.m_arrayField[0:2] )
        
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
        self.assertEqual( 1, tv.m_bit5 )
        self.assertEqual( 3, tv.m_bit6_7 )

    def testTypedVarList(self):
        tvl = target.module.typedVarList( target.module.g_listHead, "listStruct", "listEntry" )
        self.assertEqual( 3, len( tvl ) )
        self.assertEqual( [1,2,3], [ tv.num for tv in tvl ] )

        tvl = target.module.typedVarList( target.module.g_listHead, target.module.type("listStruct"), "listEntry" )
        self.assertEqual( 3, len( tvl ) )
        self.assertEqual( [1,2,3], [ tv.num for tv in tvl ] )

        tvl = target.module.typedVarList( target.module.g_listHead1, "listStruct1", "next" )
        self.assertEqual( 3, len( tvl ) )
        self.assertEqual( [100,200,300], [ tv.num for tv in tvl ] )

        tvl = target.module.typedVarList( target.module.g_listHead1, target.module.type("listStruct1"), "next" )
        self.assertEqual( 3, len( tvl ) )
        self.assertEqual( [100,200,300], [ tv.num for tv in tvl ] )

        tvl = target.module.typedVarList( target.module.g_childListHead, target.module.type("ChildEntryTest"), "m_next" )
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

        tvl = target.module.typedVarArray( target.module.g_testArray, target.module.type("structTest"), 2 )
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
        tv = target.module.typedVar("g_classChild")
        self.assertEqual( 3, tv.m_enumField )
        self.assertEqual( target.module.type("enumType").THREE, tv.m_enumField )
        
    def testIndex(self):
        ind  = target.module.typedVar( "g_ucharValue" )
        self.assertEqual( 5, [0,5,10][ind] )
      
        self.assertTrue( ind in [0,1,2] )
    
        tv = target.module.typedVar( "g_struct3" )
        self.assertEqual( 2, tv.m_arrayField[ind] )
        
        ind = target.module.typedVar( "g_ulongValue" )
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
        except pykd.BaseException: 
            pass

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
        self.assertTrue( str(target.module.typedVar( "g_ucharValue" ) ) )
        self.assertTrue( str(target.module.typedVar( "g_ushortValue" ) ) )
        self.assertTrue( str(target.module.typedVar( "g_ulongValue" ) ) )
        self.assertTrue( str(target.module.typedVar( "g_ulonglongValue" ) ) )
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

    def testStaticField(self):
        tv = pykd.typedVar( "g_classChild" )
        self.assertEqual( 200, tv.m_staticField )
        self.assertEqual( 100, tv.m_staticConst )

    def testAmbiguousFieldAccess(self):
        derivedFiledVal = pykd.loadCStr( pykd.typedVar( "g_fieldSameNameStruct" ).m_field )
        self.assertEqual( derivedFiledVal, "toaster" )
        print target.module.type("fieldSameNameStruct")
        
    def testDiamondVirtualInherit(self):
        tv = pykd.typedVar( "g_virtChild" )
        print tv
        self.assertEqual( -100, tv.m_baseField )
    
    def testDinkumwareMap(self):
        g_map = target.module.typedVar( "g_map" )
        self.assertEqual( 1, g_map._Mysize )
    
