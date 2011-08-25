#
#
#

import unittest
import pykd
import target


class TypeInfoTest( unittest.TestCase ):

    def testBasicTypes(self):
        self.assertEqual( pykd.char_t.name(), "char" )       
        self.assertEqual( pykd.char_t.size(), 1 ) 
        self.assertEqual( pykd.uchar_t.name(), "unsigned char" )       
        self.assertEqual( pykd.uchar_t.size(), 1 ) 
        
    def testSimpleStruct(self):
        ti = pykd.typeInfo( target.moduleName, "Type1" )
        self.assertTrue( hasattr( ti, "field1" ) )
        self.assertTrue( hasattr( ti, "field2" ) )
        self.assertTrue( hasattr( ti, "field3" ) )
        
        tv = pykd.typedVar( ti, target.module.var1 )
        self.assertEqual( tv.field1, -121 )
        self.assertEqual( tv.field2, 220 )
#        self.assertLess( tv.field3 - 1.0095, 0.0001 )
        
    def testEnumField(self):
        ti = pykd.typeInfo( target.moduleName, "Type2" )
        self.assertEqual( hasattr( ti, "field1" ), True )
        
        tv = pykd.typedVar( ti, target.module.var2 )
        self.assertEqual( tv.field1, 100 )
        
    def testNamespace(self):
        ti1 = pykd.typeInfo( target.moduleName, "Namespace1::Class1" )
        ti2 = pykd.typeInfo( target.moduleName, "Namespace1::Namespace2::Class2" )
        var3 = pykd.typedVar( ti1, pykd.getOffset( target.moduleName, "Namespace1::var3" ) )
        var4 = pykd.typedVar( ti1, pykd.getOffset( target.moduleName, "Namespace1::Namespace2::var4" ) )
        self.assertEqual( var3.m_field1, 50 )

    def testTemplates(self):
        ti3 = pykd.typeInfo( target.moduleName, "Namespace3::Class3<int>" )
        var5 = pykd.typedVar( ti3, pykd.getOffset( target.moduleName, "Namespace3::var5" ) )
        self.assertEqual( var5.m_field1, 5 )
        
    def testNestedStruct(self):
        ti4 = pykd.typeInfo( target.moduleName,  "Type4" )
        self.assertTrue( hasattr( ti4, "field1" ) )
        self.assertTrue( hasattr( ti4, "field2" ) )
        self.assertTrue( hasattr( ti4, "field3" ) )
        self.assertTrue( hasattr( ti4, "field4" ) )
        self.assertTrue( hasattr( ti4, "field4" ) )
        self.assertTrue( hasattr( ti4.field4, "field41" ) )
        
    def testPtrField(self):
        v6 = pykd.typedVar( target.moduleName, "Type6", pykd.getOffset( target.moduleName, "var6" ) )
        self.assertEqual( v6.field1, 10 )
        self.assertEqual( v6.field2.field1, 10 )
        self.assertEqual( v6.field2.field2, 20 )
        self.assertNotEqual( v6.field2, 0 )
        self.assertEqual( v6.field3[0].field1, 10 )
        self.assertEqual( v6.field3[1].field2, 20 )
        
    def testArrayField(self):
        v7 = pykd.typedVar( target.moduleName, "Type7", pykd.getOffset( target.moduleName, "var7" ) )
        self.assertEqual( v7.field1[1].field1, 10 )
        self.assertEqual( v7.field1[5].field2, 20 )
        self.assertEqual( v7.field2[1][0].field1, 10 )
        self.assertEqual( v7.field2[0][1].field2, 20 )
