
import unittest
import target
import pykd

class DbgClientTest( unittest.TestCase ):

    def testGetDebuggeeType( self ):
        c, q = pykd.getDebuggeeType()
        self.assertEqual( c, pykd.DEBUG_CLASS_USER_WINDOWS )
        self.assertEqual( q, pykd.DEBUG_USER_WINDOWS_PROCESS )
        
    def testIsKernelDebugging( self ):
        self.assertFalse( pykd.isKernelDebugging() )
        
    def testIsDumpAnalyzing( self ):
        self.assertFalse( pykd.isDumpAnalyzing() )
        
    def testExecutionStatus( self ):
        self.assertEqual( pykd.DEBUG_STATUS_BREAK, pykd.getExecutionStatus() )
        pykd.setExecutionStatus( pykd.DEBUG_STATUS_GO )
        pykd.waitForEvent()
        self.assertEqual( pykd.DEBUG_STATUS_BREAK, pykd.getExecutionStatus() )
        
    def testPdbFile( self ):
        self.assertNotEqual( '', pykd.getPdbFile( target.module.begin() ) )

    def testProcessorMode( self ):
        self.assertNotEqual( '', pykd.getProcessorMode() )
        self.assertNotEqual( '', pykd.getProcessorType() )

    def testThreadList( self ):
        self.assertNotEqual( 0, len(pykd.getThreadList()) )
        
    def testSymbolsPath( self ):
        self.assertNotEqual( '', pykd.symbolsPath() )
