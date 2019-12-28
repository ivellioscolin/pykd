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
        self.assertEqual( 10, target.module.typedVar( "ucharVar" ) )
        self.assertEqual( 1020, target.module.typedVar( "ushortVar" ) )
        self.assertEqual( 10002000, target.module.typedVar( "ulongVar" ) )
        self.assertEqual( 1234567890, target.module.typedVar( "ulonglongVar" ) )
        self.assertEqual( -5, target.module.typedVar( "charVar" ) )
        self.assertEqual( -1020, target.module.typedVar( "shortVar" ) )
        self.assertEqual( -1002000, target.module.typedVar( "longVar" ) )
        self.assertEqual( -1234567890, target.module.typedVar( "longlongVar" ) )

    def testPtrTo(self):
        tvBaseType = pykd.typedVar( pykd.typeInfo("UInt8B").ptrTo(), target.module.offset("pbigValue") )
        self.assertEqual( target.module.typedVar( "bigValue" ), tvBaseType.deref() )

        tvDiaStruct = pykd.typedVar( target.module.type("structTest").ptrTo(), target.module.offset("g_structTestPtr") )
        self.assertEqual( 500, tvDiaStruct.deref().m_field1 )

        customStructTest = pykd.createStruct(name="customStructTest", align=4)
        customStructTest.append("m_field0", pykd.baseTypes.UInt4B)
        customStructTest.append("m_field1", pykd.baseTypes.UInt8B)
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
        self.assertEqual( True, target.module.typedVar( "boolConst" ) )
        self.assertEqual( 0x5555, target.module.typedVar( "ulongConst" ) )
        self.assertEqual( 0xffffff000000, target.module.typedVar( "ulonglongConst" ) )
        self.assertEqual( 3, target.module.typedVar( "g_constEnumThree" ) )

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
        self.assertEqual( 1, tv1["m_field3"] )
        self.assertRaises( AttributeError, lambda t: t.not_exists, tv1) # non-exsisting field
        self.assertRaises( KeyError, lambda t: t["not_exists"], tv1) # non-exsisting field

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
        self.assertEqual( tv + tv.fieldOffset("m_arrayField"),tv.m_arrayField)
        self.assertEqual( 2, len(tv.m_arrayField) )
        self.assertEqual( 0, tv.m_arrayField[0] )
        self.assertEqual( 2, tv.m_arrayField[1] )
        self.assertEqual( 3, tv.m_noArrayField )
        self.assertNotEqual( -1, tv.m_arrayField[0] )
        self.assertNotEqual( 0, tv.m_noArrayField )
        tv.m_arrayField[len(tv.m_arrayField)]

    def testArrayIteration(self):
        tv = target.module.typedVar( "g_structWithArray" )
        self.assertEqual([0, 2], [x for x in tv.m_arrayField])

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
        tvl = target.module.typedVarList( target.module.g_listHead, "listStruct", "next.flink" )
        self.assertEqual( 5, len( tvl ) )
        self.assertEqual( [ i for i in range(5)], [ tv.num for tv in tvl ] )

        tvl = pykd.typedVarList( target.module.g_listHead, target.module.type("listStruct"), "next.flink" )
        self.assertEqual( 5, len( tvl ) )
        self.assertEqual( [ i for i in range(5)], [ tv.num for tv in tvl ] )

        tvl = pykd.typedVarList( target.module.g_listHead, target.module.type("listStruct"), "next.flink" )
        self.assertEqual( 5, len( tvl ) )
        self.assertEqual( [ i for i in range(5)], [ tv.num for tv in tvl ] )

        #tvl = target.module.typedVarList( target.module.g_listHead1, "listStruct1", "next" )
        #self.assertEqual( 3, len( tvl ) )
        #self.assertEqual( [100,200,300], [ tv.num for tv in tvl ] )

        #tvl = pykd.typedVarList( target.module.g_listHead1, target.module.type("listStruct1"), "next" )
        #self.assertEqual( 3, len( tvl ) )
        #self.assertEqual( [100,200,300], [ tv.num for tv in tvl ] )

        #tvl = pykd.typedVarList( target.module.g_childListHead, target.module.type("ChildEntryTest"), "m_next" )
        #self.assertEqual( 3, len( tvl ) )
        #self.assertEqual( [1000,2000,3000], [ tv.m_someBaseFiled2 for tv in tvl ] )
        #self.assertEqual( [1001,2001,3001], [ tv.m_childFiled1 for tv in tvl ] )

        #tvl1 = target.module.typedVarList( target.module.g_listHead, "listStruct", "listEntry" )
        #tvl2 = pykd.typedVarList( target.module.g_listHead, target.moduleName + "!listStruct", "listEntry" )
        #self.assertEqual( tvl1, tvl2 )

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
        ind  = target.module.typedVar( "g_structTest" ).m_field3
        self.assertEqual( 5, [0,5,10][ind] )
      
        self.assertTrue( ind in [0,1,2] )
    
        tv = target.module.typedVar( "g_structWithArray" )
        self.assertEqual( 2, tv.m_arrayField[ind] )
        
        self.assertTrue( ind in { 1 : "1", 4 : "2" } )
        self.assertEqual( "1", { 1 : "1", 4 : "2" }[ind] )

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

    def testFunctionPtr(self):
        funcptr = target.module.typedVar( "CdeclFuncPtr" )
        # CdeclFuncPtr -> jmp     targetapp!CdeclFunc (00bd1ba0)
        # self.assertEqual( funcptr, target.module.CdeclFunc )
        self.assertEqual( funcptr.type().deref().name(), target.module.typedVar("CdeclFunc").type().name() )
        self.assertEqual( funcptr.type().name(), target.module.typedVar("CdeclFunc").type().ptrTo().name() )

    def testFunctionRange(self):
        func1 = target.module.typedVar("CdeclFunc")
        self.assertTrue( func1.getAddress() >= target.module.begin() )
        self.assertTrue( func1.getAddress() + func1.sizeof() <= target.module.end() )

        func2 = target.module.typedVar( target.module.StdcallFunc )
        self.assertTrue( func2.getAddress() >= target.module.begin() )
        self.assertTrue( func2.getAddress() + func2.sizeof() <= target.module.end() )

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
        #self.assertTrue( str(target.module.typedVar( "g_struct3" ) ) )
        self.assertTrue( str(target.module.typedVar( "g_listHead" ) ) )
        self.assertTrue( str(target.module.typedVar( "voidPtr" ) ) )
        #self.assertTrue( str(target.module.typedVar( "g_arrOfPtrToFunc" ) ) )
        #self.assertTrue( str(target.module.typedVar( "g_unTypedPtrToFunction" ) ) )
        
    def testNotValidPrint(self):
        types = ("structTest", "ULong[100]", "ULong*" )
        for ti in types:
            self.assertTrue( str(pykd.typedVar( target.module.type(ti), 0 ) ) )

    def testStaticField(self):
        tv = pykd.typedVar( "g_classChild" )
        self.assertEqual( 200, tv.m_staticField )
        self.assertEqual( 100, tv.m_staticConst )

    def testAmbiguousFieldAccess(self):
       # derivedFiledVal = pykd.loadCStr( pykd.typedVar( "g_fieldSameNameStruct" ).m_field )
       # self.assertEqual( derivedFiledVal, "toaster" )
         self.assertEqual( 678, pykd.typedVar( "g_virtChild" ).m_member )

    def testDiamondVirtualInherit(self):
        tv = pykd.typedVar( "g_virtChild" )
        self.assertEqual( -100, tv.m_baseField )

    def testUdtSubscribe(self):
        tv = pykd.typedVar( "g_virtChild" )
        self.assertEqual( 6, len(tv) )
        fieldName = tv.fieldName(5)
        fieldVal = tv.field(5)

        self.assertEqual( fieldName, "m_baseField" )
        self.assertEqual( fieldVal, tv.m_baseField )

        for field in tv.fields():
            str( field )

    #def testDeadlockList(self):
    #    lst = []
    #    entry = pykd.typedVar("deadlockEntry").flink
    #    for i in xrange( 0, 100000 ):
    #        lst.append(entry)
    #        entry = entry.deref().flink

    def testHresult(self):
        tv = pykd.typedVar( "g_atlException" )
        self.assertEqual( tv.m_hr, 0x8000FFFF )

    def testFunctionDebugRange(self):
        tv = pykd.typedVar( "startChildProcess" )

        self.assertTrue( tv.getDebugStart() >= tv.getAddress() )
        self.assertTrue( tv.getDebugEnd() <= tv.getAddress() + tv.sizeof() )

    def testFields(self):
        tv = pykd.typedVar( "g_classChild")
        self.assertTrue( len(tv.fields())>0 )

    def testDir(self):
        tv = target.module.typedVar( "structTest", target.module.g_structTest )
        self.assertEqual(5, len(dir(tv)))
        self.assertTrue("m_field3" in dir(tv))
        self.assertFalse("m_field33" in dir(tv))

    def testMemoryException(self):
        self.assertRaises(pykd.MemoryException, 
                          int,
                          pykd.typedVar(pykd.baseTypes.UInt4B, 0))
        if pykd.getCPUType() == pykd.CPUType.I386:
            self.assertRaises(pykd.MemoryException, 
                              int,
                              pykd.typedVar(pykd.baseTypes.UInt4B, 0xFFFFFFFF))
        self.assertRaises(pykd.MemoryException, 
                          int,
                          pykd.typedVar(pykd.baseTypes.UInt4B, 0xFFFFFFFFFFFFFFFF))

    def testLocation(self):
        tv = target.module.typedVar( "structTest", target.module.g_structTest )
        self.assertEqual( ( pykd.Location.Memory, tv.getAddress()), tv.getLocation() )

    def testGetAddress(self):
        if not pykd.is64bitSystem():
            tv = target.module.typedVar( "structTest", 0x80000000 )
            self.assertEqual( 0xFFFFFFFF80000000, tv.getAddress() )
            self.assertEqual( 0xFFFFFFFF80000000, tv )

    def testCompare(self):
        self.assertTrue( 10002000 ==  target.module.typedVar( "ulongVar" ) )
        self.assertTrue( 10002001 !=  target.module.typedVar( "ulongVar" ) )
        self.assertTrue( 10002001 > target.module.typedVar( "ulongVar" ) )
        self.assertTrue( 10001999 < target.module.typedVar( "ulongVar" ) )
        self.assertTrue( 10002001 >= target.module.typedVar( "ulongVar" ) )
        self.assertTrue( 10001999 <= target.module.typedVar( "ulongVar" ) )
        self.assertTrue( True if target.module.typedVar( "ulongVar" )  else False )
        self.assertTrue( False if not target.module.typedVar( "ulongVar" ) else True )

 
    def testCompareWihNone(self):
        tv = target.module.typedVar( "structTest", target.module.g_structTest )
        self.assertFalse(tv==None)
        self.assertTrue(tv!=None)

    def testCastTo(self):
        self.assertEqual(0xD2, target.module.typedVar( "ulonglongVar" ).castTo("UInt1B"))
        self.assertEqual(0, target.module.typedVar( "g_structTest" ).castTo("UInt4B"))

    def testCallFunction(self):
        funcptr = target.module.typedVar("StdcallFuncRet");
        self.assertEqual( 200000/10, pykd.callFunctionByPtr( funcptr, 10, 200000 ) )

        funcptr = target.module.typedVar("CdeclFuncLong");
        self.assertEqual( 0xffffff000000 + 5, pykd.callFunctionByPtr( funcptr, target.module.typedVar("ulonglongConst") ) )
        self.assertEqual( 0x7777 + 5, funcptr.call(0x7777) )
        self.assertEqual( 0x11223344556677 + 5, funcptr(0x11223344556677) );

        functype = pykd.defineFunction( pykd.baseTypes.Int4B, pykd.callingConvention.NearStd)
        functype.append("arg1", pykd.baseTypes.Int1B)
        functype.append("arg2", pykd.baseTypes.Long)
        self.assertEqual( 500 / 25, pykd.callFunctionByAddr(functype, target.module.offset("StdcallFuncRet"), 25, 500 ) )
        
        self.assertTrue( target.module.typedVar("OverloadedFunc", "Bool(__cdecl)(Int4B,Int4B)").call(100,299) )

    def testCallFunctionWithTypedVar(self):
        funcptr = target.module.typedVar("StdcallFuncRet");
        ucharVar = target.module.typedVar( "ucharVar" );
        self.assertEqual( 10, ucharVar )
        self.assertEqual( 200000/10, funcptr( ucharVar, 200000 ) )

    def testCallWithWrongArgs(self):
        self.assertRaises( pykd.TypeException, target.module.typedVar("StdcallFuncRet"), *(1,) )
        self.assertRaises( pykd.TypeException, target.module.typedVar("StdcallFuncRet"), *(1,2,3) )
        self.assertRaises( pykd.TypeException, target.module.typedVar("StdcallFuncRet"), *(10, target.module.typedVar("g_classChild") ))

    def testCallMethod(self):
        g_classChild = target.module.typedVar("g_classChild")
        self.assertEqual( 1000*5, g_classChild.method("childMethod").call(10) )
        self.assertEqual( 1000*5, g_classChild.childMethod(10) )

    def testCallStdStr(self):
        g_stdString = target.module.typedVar("g_stdString")
        self.assertEqual( "testString".find('S'), g_stdString.find_first_of(ord('S'), 0) )

    def testCallOverloadMethod(self):
        g_classChild = target.module.typedVar("g_classChild")
        self.assertEqual( 10*10, g_classChild.method("overloadMethod", "Int4B(__thiscall)(Int4B)").call(10))
        self.assertEqual( 5*8, g_classChild.method("overloadMethod", "Int4B(__thiscall)(Int4B,Int4B)").call(5,8))

    def testGetTypedVar(self):
        addr = pykd.getOffset("g_structTest")
        self.assertTrue( None != target.module.type( "structTest" ).getTypedVar(addr) )

    def testByteSequence(self):
        self.assertEqual( 0x44332211, pykd.typedVar("UInt4B", [0x11, 0x22, 0x33, 0x44]) )
        self.assertEqual( -1, pykd.typedVar( pykd.baseTypes.Int4B, [0xFF, 0xFF, 0xFF, 0xFF] ) )
       
    def testRawBytes(self):
        self.assertEqual( [ 0x55, 0x55, 0, 0], target.module.typedVar( "ulongConst" ).rawBytes() )

    def testSetField(self):
        byteseq = [0x55] * 20
        var = target.module.typedVar("structTest", byteseq)
        var.setField("m_field1", 0xFF000000000000AA)
        self.assertEqual( [0xAA, 0, 0, 0, 0, 0, 0, 0xFF], byteseq[4:12] )
        var.m_field3 = 0xAAAA
        self.assertEqual( 0xAAAA, var.m_field3)
        self.assertEqual( 0xAAAA, var.field("m_field3") )

      
    def testAttr(self):
        var = target.module.typedVar("structTest", [0x55] * 20 )
        setattr(var, "m_field1", 11)
        self.assertEqual(11, getattr(var, "m_field1"))
        self.assertRaises(AttributeError, lambda x: getattr(x, "noexists"), var)

    def testEvalPyScope(self):
        var = target.module.typedVar("structTest", [0x55] * 20 )
        self.assertEqual( var.m_field1 * 17, eval("m_field1 * 17", globals(), var) )

    def testEvalExpr(self):
        self.assertEqual( 2+2, pykd.evalExpr("2+2") )
        self.assertEqual( target.module.typedVar("g_structTest").m_field1, pykd.evalExpr("g_structTest.m_field1") )
        self.assertEqual( target.module.typedVar("g_testArray")[1].m_field3, pykd.evalExpr("(g_testArray + 1)->m_field3") )

    def testEvalExprScope(self):

        v1 = target.module.typedVar( "ulongVar" )
        v2 = target.module.typedVar( "g_structTest" )

        scope = { "v1" : v1, "v2" : v2 }
        self.assertEqual( v1 + v2.m_field1, pykd.evalExpr("v1 + v2.m_field1", scope) )

    def testEvalExprScopeLong(self):
        v1 = 100
        v2 = -500
        scope = { "v1" : v1, "v2" : v2 }
        self.assertEqual( v1 + v2, pykd.evalExpr("v1 + v2", scope))
        self.assertEqual( v1 * v2, pykd.evalExpr("v1 * v2", locals()))

    def testEvalExprScopeStruct(self):
        var = pykd.typedVar("g_structTest1")
        self.assertEqual(var.m_field1, pykd.evalExpr("m_field1", var))
        self.assertEqual(var.m_field4.deref().m_field1, pykd.evalExpr("m_field4->m_field1", var))

    def testEvalExprSizeof(self):
        self.assertEqual(4, pykd.evalExpr("sizeof(int)"))
        self.assertEqual(4, pykd.evalExpr("sizeof(int&)"))
        self.assertEqual(pykd.ptrSize() * 10, pykd.evalExpr("sizeof(int*[10])"))
        self.assertEqual(pykd.ptrSize(), pykd.evalExpr("sizeof(int(*)[10])"))

    def testContain(self):
        var = pykd.typedVar("g_structTest")
        self.assertTrue("m_field1" in var)
        self.assertFalse("NotExist" in var)
        self.assertRaises(Exception, lambda var : 2 in var, var)

    def testHasField(self):
        var = pykd.typedVar("g_structTest")
        self.assertTrue(var.hasField("m_field1"))
        self.assertFalse(var.hasField("NotExist"))

    def testHasMethod(self):
        var = pykd.typedVar("g_classChild")
        self.assertTrue(var.hasMethod("childMethod"))
        self.assertTrue(var.hasMethod("staticMethod"))
        self.assertTrue(var.hasMethod("virtMethod1"))
        self.assertFalse(var.hasMethod("notExist"))
        
