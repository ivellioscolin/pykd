"""Exception event test"""

import unittest
import target
import pykd
import testutils

class ExceptionHandler(pykd.eventHandler):
    """Track load/unload module implementation"""
    def __init__(self):
        pykd.eventHandler.__init__(self)
        self.accessViolationOccured = False

    def onException(self, exceptInfo):
        """Exception handler"""
        self.accessViolationOccured = exceptInfo.ExceptionCode == 0xC0000005

        if self.accessViolationOccured:
            self.param0 = exceptInfo.Parameters[0]
            self.param1 = exceptInfo.Parameters[1]
            return pykd.eventResult.Break

        return pykd.eventResult.NoChange

class EhExceptionTest(unittest.TestCase):
    """Exception event test"""

    def testException(self):
        """Start new process and track exceptions"""
        _locProcessId = pykd.startProcess( target.appPath + " -testAccessViolation" )
        with testutils.ContextCallIt( testutils.KillProcess(_locProcessId) ) as killStartedProcess :
            exceptionHandler = ExceptionHandler()

            while not exceptionHandler.accessViolationOccured:
                pykd.go()

            self.assertEqual( pykd.lastEvent(), pykd.eventType.Exception )

            self.assertTrue( exceptionHandler.accessViolationOccured )
            self.assertEqual( exceptionHandler.param0, 1 )  # write
            self.assertEqual( exceptionHandler.param1, 6 )  # addr

            exceptInfo = pykd.lastException()
            self.assertEqual( exceptInfo.ExceptionCode, 0xC0000005 )
            self.assertEqual( exceptionHandler.param0, exceptInfo.Parameters[0] )
            self.assertEqual( exceptionHandler.param1, exceptInfo.Parameters[1] )
