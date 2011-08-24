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
        self.assertEqual( hasattr( ti, "field1" ), True )
        self.assertEqual( hasattr( ti, "field2" ), True )
        self.assertEqual( hasattr( ti, "field3" ), True )
        
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

