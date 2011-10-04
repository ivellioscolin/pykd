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
        
    def testCreateByName( self ):
        """ creating typeInfo by the type name """
        ti1 = target.module.type( "structTest" )
        ti2 = target.module.type( "classChild" )
        
    def testGetField( self ):
        """ get field of the complex type """
        ti1 = target.module.type( "structTest" )
        self.assertNotEqual( None, ti1.m_field0 )  # exsisting field
        try: ti1.m_field4                          # non-exsisting field
        except pykd.DiaException: pass   
        
    def testName( self ):
        ti1 = target.module.type( "classChild" )
        self.assertEqual( "classChild", ti1.name() )
        self.assertEqual( "Int", ti1.m_childField.name() )        
        self.assertEqual( "structTest", ti1.m_childField3.name() )
        
    def testOffset( self ):
        ti1 = target.module.type( "structTest" )
        self.assertEqual( 0, ti1.m_field0.offset() )
        self.assertEqual( 4, ti1.m_field1.offset() )
        self.assertEqual( 12, ti1.m_field2.offset() )        
        self.assertEqual( 14, ti1.m_field3.offset() )              
       
