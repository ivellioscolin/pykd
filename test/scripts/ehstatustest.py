"""Execution status event test"""

import unittest
import target
import pykd
import testutils

class StatusChangeHandler(pykd.eventHandler):

    def __init__(self):
        pykd.eventHandler.__init__(self)
        self.breakCount = 0
        self.goCount = 0
        self.noDebuggee = 0

    def onExecutionStatusChange(self, executionStatus):
        if executionStatus == pykd.executionStatus.Break:
            self.breakCount += 1
        if executionStatus == pykd.executionStatus.Go:
            self.goCount += 1
        if executionStatus == pykd.executionStatus.NoDebuggee:
            self.noDebuggee += 1


class EhStatusTest(unittest.TestCase):
    """Execution status event test"""

    def testException(self):
        """Start new process and track exceptions"""
        _locProcessId = pykd.startProcess( target.appPath + " -testChangeStatus" )
        with testutils.ContextCallIt( testutils.KillProcess(_locProcessId) ) as killStartedProcess :

            pykd.go() #skip initial break

            statusChangeHandler = StatusChangeHandler()

            self.assertRaises(pykd.WaitEventException, testutils.infGo)

            self.assertEqual( 2, statusChangeHandler.breakCount )
            self.assertEqual( 1, statusChangeHandler.noDebuggee )
            self.assertEqual( statusChangeHandler.breakCount + statusChangeHandler.noDebuggee , statusChangeHandler.goCount )
