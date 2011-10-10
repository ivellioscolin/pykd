
import unittest
import target
import pykd

class DbgcmdTest( unittest.TestCase ):

    def testDbgCommand( self ):
        self.assertNotEqual( "", pykd.dbgCommand("lm") )
        
#    def testDbgExt( self ):
#        #ext = pykd.loadExt( "ext" )
#        #self.assertNotEqual( "", ext.call("help", "") )

    def testExpr( self ):
        self.assertEqual( 8, pykd.expr( "poi(targetapp!g_ulonglongValue)" ) )
    
