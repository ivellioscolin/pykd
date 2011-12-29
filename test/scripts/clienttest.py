
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

    def testNumberProcessors( self ):
        """Number of processors can not be 0"""
        self.assertNotEqual( 0, pykd.getNumberProcessors() )

    def testPageSize( self ):
        """Size of memory page must be >= 4kb"""
        self.assertTrue( pykd.getPageSize() >= 4*1024 )

    def testCurrentThreadContext( self ):
        """Some check of current thread context content"""
        ctx = pykd.getContext()
#        for reg in ctx:
#            regName = ""
#            try:
#                regName = pykd.diaI386Regs[ reg[0] ]
#            except KeyError:
#                regName = pykd.diaAmd64Regs[ reg[0] ]
#            pykd.dprint( "\n" + regName + ": 0x%x " % reg[1])
        self.assertNotEqual( 0, len(ctx) )
        self.assertNotEqual( 0, ctx.ip() )
        self.assertNotEqual( 0, ctx.csp() )

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
