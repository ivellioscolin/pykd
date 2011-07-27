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
        
        tv = pykd.typedVar( ti, target.module.var1 )
        self.assertEqual( tv.field1, -121 )
        self.assertEqual( tv.field2, 220 )
        
    def testEnumField(self):
        ti = pykd.typeInfo( target.moduleName, "Type2" )
        self.assertEqual( hasattr( ti, "field1" ), True )
        
        tv = pykd.typedVar( ti, target.module.var2 )
        self.assertEqual( tv.field1, 100 )



