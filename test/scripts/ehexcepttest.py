"""Debug events handler: test exceptions and debug breaks"""

import unittest
import target
import pykd

class BreakExceptionHandler(pykd.eventHandler):
    """Track load/unload module implementation"""
    def __init__(self, client):
        pykd.eventHandler.__init__(self, client)

        self.client = client

        self.wasSecondChance = False

        self.wasBreakpoint = False
        self.wasBreakpointIds = []

        self.bpLastModuleName = ""
        self.bpCount = 0

        self.firstChanceAccessAddresses = []
        self.secondChanceAccessAddresses = []

    def onBreakpoint(self, bpId):
        """Breakpoint handler"""
        self.wasBreakpoint = True
        self.wasBreakpointIds.append( bpId )
        return pykd.DEBUG_STATUS_NO_CHANGE

    def onException(self, exceptParams):
        """Exception handler"""

        self.wasSecondChance = not exceptParams["FirstChance"]

        if exceptParams["Code"] == pykd.EXCEPTION_ACCESS_VIOLATION:
            if self.wasSecondChance:
                self.secondChanceAccessAddresses.append( exceptParams["Parameters"][1] )
            else:
                self.firstChanceAccessAddresses.append( exceptParams["Parameters"][1] )
        elif exceptParams["Code"] == pykd.EXCEPTION_BREAKPOINT:
            self.bpCount += 1

        return pykd.DEBUG_STATUS_BREAK if self.wasSecondChance else pykd.DEBUG_STATUS_NO_CHANGE

class EhExceptionBreakpointTest(unittest.TestCase):
    """Unit tests of exceptions end breakpoint handling"""

    def testBreakpointException(self):
        """Start new process and track exceptions/breakpoints"""
        testClient = pykd.createDbgClient()
        testClient.startProcess( target.appPath + " -testExceptions" )

        targetMod = testClient.loadModule( "targetapp" )

        bpIdSoftware = testClient.setBp( targetMod.offset("changeValueForAccessTesting") )

        bpIdHwExecute = testClient.setBp( targetMod.offset("readValueForAccessTesting"),
                                          1,
                                          pykd.DEBUG_BREAK_EXECUTE )

        bpIdHwWrite = testClient.setBp( targetMod.offset("g_valueForAccessTesting1"),
                                        1,
                                        pykd.DEBUG_BREAK_WRITE )

        bpIdHwRead = testClient.setBp( targetMod.offset("g_valueForAccessTesting2"),
                                       1,
                                       pykd.DEBUG_BREAK_READ )

        self.assertEqual( 4, len( testClient.getAllBp() ) )

        breakExceptionHandler = BreakExceptionHandler( testClient )
        while not breakExceptionHandler.wasSecondChance:
            testClient.go()

        self.assertTrue( breakExceptionHandler.wasBreakpoint )

        self.assertEqual( [2, 3], breakExceptionHandler.firstChanceAccessAddresses  )

        self.assertEqual( 2, breakExceptionHandler.bpCount ) # main and doExeptions

        self.assertEqual( [3, ], breakExceptionHandler.secondChanceAccessAddresses )

        self.assertTrue( bpIdSoftware in breakExceptionHandler.wasBreakpointIds )

        testClient.removeBp(bpIdHwRead)
        self.assertEqual( 3, len( testClient.getAllBp() ) )

        testClient.removeBp()
        self.assertEqual( 0, len( testClient.getAllBp() ) )
