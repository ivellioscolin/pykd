
import unittest
import target
import pykd

class DbgcmdTest( unittest.TestCase ):

    def testDbgCommand( self ):
        self.assertNotEqual( "", pykd.dbgCommand("lm") )
        
#    def testDbgExt( self ):
#        #ext = pykd.loadExt( "ext" )
#        #self.assertNotEqual( "", ext.call("help", "") )
