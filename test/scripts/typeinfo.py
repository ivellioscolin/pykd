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

