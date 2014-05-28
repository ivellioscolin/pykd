"""Exception event test"""

import unittest
import target
import pykd
import testutils


class ExceptionTest(unittest.TestCase):
    """Exception event test"""

    def testExceptionHandler(self):

        class ExceptionHandler(pykd.eventHandler):
           def __init__(self):
               pykd.eventHandler.__init__(self)

           def onException(self, exceptInfo):
                self.exceptInfo = exceptInfo
                return pykd.eventResult.Break

        processId = pykd.startProcess( target.appPath + " exception" )
        with testutils.ContextCallIt( testutils.KillProcess(processId) ) as killStartedProcess :
            exceptionHandler = ExceptionHandler()

            self.assertEqual( pykd.Break, pykd.go() )
            self.assertEqual( 0xC0000005, exceptionHandler.exceptInfo.exceptionCode) #0xC0000005 = Access violation

    def testSecondChance(self):

        class ExceptionHandler(pykd.eventHandler):
           def __init__(self):
               pykd.eventHandler.__init__(self)

           def onException(self, exceptInfo):
                self.exceptInfo = exceptInfo
                return pykd.eventResult.Proceed

        processId = pykd.startProcess( target.appPath + " exception" )
        with testutils.ContextCallIt( testutils.KillProcess(processId) ) as killStartedProcess :
            exceptionHandler = ExceptionHandler()
            self.assertEqual( pykd.Break, pykd.go() )
            self.assertEqual( True, exceptionHandler.exceptInfo.firstChance)
            self.assertEqual( pykd.Break, pykd.go() )
            self.assertEqual( False, exceptionHandler.exceptInfo.firstChance)
