#
#
#

import unittest
import target
import pykd

class TypeInfoTest( unittest.TestCase ):

    def testCtor( self ):
        """ typeInfo class can not be created direct """
        try: pykd.typeInfo()
        except RuntimeError: pass
        
        
    def testBaseTypes(self):
        self.assertEqual("Int1B", target.module.type( "Int1B" ).name() )
        self.assertEqual("Int2B", target.module.type( "Int2B" ).name() )
        self.assertEqual("Int4B", target.module.type( "Int4B" ).name() )
        self.assertEqual("Int8B", target.module.type( "Int8B" ).name() )
        self.assertEqual("UInt1B", target.module.type( "UInt1B" ).name() )
        self.assertEqual("UInt2B", target.module.type( "UInt2B" ).name() )
        self.assertEqual("UInt4B", target.module.type( "UInt4B" ).name() )
        self.assertEqual("UInt8B", target.module.type( "UInt8B" ).name() )

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
        self.assertEqual( 1, target.module.type("Char").size() )
        self.assertEqual( 2, target.module.type("WChar").size() )
        self.assertEqual( 2, target.module.type("Int2B").size() )        
        self.assertEqual( 2, target.module.type("UInt2B").size() )          
        self.assertEqual( 4, target.module.type("Int4B").size() )        
        self.assertEqual( 4, target.module.type("UInt4B").size() )          
        self.assertEqual( 8, target.module.type("Int8B").size() )        
        self.assertEqual( 8, target.module.type("UInt8B").size() )            

    def testName( self ):
        ti1 = target.module.type( "classChild" )
        self.assertEqual( "classChild", ti1.name() )
        self.assertEqual( "Int4B", ti1.m_childField.name() )
        self.assertEqual( "structTest", ti1.m_childField3.name() )
        self.assertEqual( "structTest", target.module.type("g_structTest").name() )
        
    def testOffset( self ):
        ti1 = target.module.type( "structTest" )
        self.assertEqual( 0, ti1.m_field0.offset() )
        self.assertEqual( 4, ti1.m_field1.offset() )
        self.assertEqual( 12, ti1.m_field2.offset() )
        self.assertEqual( 14, ti1.m_field3.offset() )
        
    def testSize( self ):
        ti1 = target.module.type( "structTest" )
        self.assertEqual( 20, ti1.size() )
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
        
    def testUnion(self):
        ti = target.module.type("unionTest")
        self.assertEqual( 0, ti.m_doubleValue.offset() )
        self.assertEqual( 0, ti.m_bits.offset() )
        self.assertEqual( ti.size(), ti.m_doubleValue.size() )
