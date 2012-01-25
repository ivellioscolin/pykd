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
        return pykd.DEBUG_STATUS_BREAK

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

        return pykd.DEBUG_STATUS_BREAK

class BpHandlerTestResult:
    """ Breakpoint handler test results"""
    def __init__(self):
        self.wasCodeBp = None
        self.wasDataBp = None

bpHandlerTestResult = BpHandlerTestResult()

def codeBpHandler(bpId):
    """ Handler of software breakpoint """
    bpHandlerTestResult.wasCodeBp = bpId
    return pykd.DEBUG_STATUS_NO_CHANGE

def dataBpHandler(bpId):
    """ Handler of hardware breakpoint """
    bpHandlerTestResult.wasDataBp = bpId
    return pykd.DEBUG_STATUS_NO_CHANGE

class EhExceptionBreakpointTest(unittest.TestCase):
    """Unit tests of exceptions end breakpoint handling"""

    def testBreakpointException(self):
        """Start new process and track exceptions/breakpoints"""
        testClient = pykd.createDbgClient()
        testClient.startProcess( target.appPath + " -testExceptions" )

        targetMod = testClient.loadModule( "targetapp" )

        bpIdSoftware = testClient.setBp( targetMod.offset("changeValueForAccessTesting"),
                                         codeBpHandler )

        allBp = testClient.getAllBp()
        self.assertEqual( 1, len( allBp ) )
        self.assertTrue( bpIdSoftware in allBp )

        hwBpIsNotSet = True
# kd> ba e 1 <some_address>
#         ^ Unable to set breakpoint error
# The system resets thread contexts after the process
# breakpoint so hardware breakpoints cannot be set.
# Go to the executable's entry point and set it then.

        breakExceptionHandler = BreakExceptionHandler( testClient )
        while not breakExceptionHandler.wasSecondChance:
            testClient.go()
            if hwBpIsNotSet:
                hwBpIsNotSet = False
                bpIdHwExecute = testClient.setBp( targetMod.offset("readValueForAccessTesting"),
                                                  1, pykd.DEBUG_BREAK_EXECUTE )

                bpIdHwWrite = testClient.setBp( targetMod.offset("g_valueForAccessTesting1"),
                                                1, pykd.DEBUG_BREAK_WRITE, dataBpHandler )

                bpIdHwRead = testClient.setBp( targetMod.offset("g_valueForAccessTesting2"),
                                               1, pykd.DEBUG_BREAK_READ )

                self.assertTrue( bpIdSoftware != bpIdHwExecute and 
                                 bpIdHwExecute != bpIdHwWrite and
                                 bpIdHwWrite != bpIdHwRead )

                allBp = testClient.getAllBp()
                self.assertEqual( 4, len( allBp ) )

                self.assertTrue( bpIdSoftware in allBp )
                self.assertTrue( bpIdHwExecute in allBp )
                self.assertTrue( bpIdHwWrite in allBp )
                self.assertTrue( bpIdHwRead in allBp )


        self.assertEqual( [2, 3], breakExceptionHandler.firstChanceAccessAddresses  )

        self.assertEqual( 3, breakExceptionHandler.bpCount ) # main and 2 in doExeptions

        self.assertEqual( [3, ], breakExceptionHandler.secondChanceAccessAddresses )

        self.assertTrue( breakExceptionHandler.wasBreakpoint )

        self.assertTrue( bpIdSoftware in breakExceptionHandler.wasBreakpointIds )
        self.assertTrue( bpIdHwExecute in breakExceptionHandler.wasBreakpointIds )
        self.assertTrue( bpIdHwWrite in breakExceptionHandler.wasBreakpointIds )
        self.assertTrue( bpIdHwRead in breakExceptionHandler.wasBreakpointIds )

        testClient.removeBp(bpIdHwRead)
        self.assertEqual( 3, len( testClient.getAllBp() ) )

        testClient.removeBp()
        self.assertEqual( 0, len( testClient.getAllBp() ) )

        self.assertEqual( bpHandlerTestResult.wasCodeBp, bpIdSoftware )
        self.assertTrue( bpHandlerTestResult.wasDataBp, bpIdHwWrite )
