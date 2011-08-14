#
#
#

import unittest
import pykd
import target


class TypeInfoTest( unittest.TestCase ):

    def testBasicTypes(self):
        print "Testing basic types"
        self.assertEqual( pykd.char_t.name(), "char" )       
        self.assertEqual( pykd.char_t.size(), 1 ) 
        self.assertEqual( pykd.uchar_t.name(), "unsigned char" )       
        self.assertEqual( pykd.uchar_t.size(), 1 ) 
        
    def testSimpleStruct(self):
        print "Testing simple struct"
        ti = pykd.typeInfo( target.moduleName, "Type1" )
        self.assertEqual( hasattr( ti, "field1" ), True )
        self.assertEqual( hasattr( ti, "field2" ), True )
        self.assertEqual( hasattr( ti, "field3" ), True )
        
        tv = pykd.typedVar( ti, target.module.var1 )
        self.assertEqual( tv.field1, -121 )
        self.assertEqual( tv.field2, 220 )
#        self.assertLess( tv.field3 - 1.0095, 0.0001 )
        
    def testEnumField(self):
        print "Testing enum field"
        ti = pykd.typeInfo( target.moduleName, "Type2" )
        self.assertEqual( hasattr( ti, "field1" ), True )
        
        tv = pykd.typedVar( ti, target.module.var2 )
        self.assertEqual( tv.field1, 100 )
        
    def testNamespace(self):
        print "Testing namespace"
        ti1 = pykd.typeInfo( target.moduleName, "Namespace1::Class1" )
        ti2 = pykd.typeInfo( target.moduleName, "Namespace1::Namespace2::Class2" )
        var3 = pykd.typedVar( ti1, pykd.getOffset( target.moduleName, "Namespace1::var3" ) )
        var4 = pykd.typedVar( ti1, pykd.getOffset( target.moduleName, "Namespace1::Namespace2::var4" ) )
        self.assertEqual( var3.m_field1, 50 )



