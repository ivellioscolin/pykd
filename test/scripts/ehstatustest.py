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
        
    
    def onExecutionStatusChange(self, executionStatus):
        if executionStatus == pykd.executionStatus.Break:
            self.breakCount = self.breakCount + 1
        if executionStatus == pykd.executionStatus.Go:
            self.goCount = self.goCount + 1


class EhStatusTest(unittest.TestCase):
    """Execution status event test"""

    def testException(self):
        """Start new process and track exceptions"""
        _locProcessId = pykd.startProcess( target.appPath + " -testChangeStatus" )
        with testutils.ContextCallIt( testutils.KillProcess(_locProcessId) ) as killStartedProcess :
        
            pykd.go() #skip initial break
        
            statusChangeHandler = StatusChangeHandler()
 
            pykd.go()
            pykd.go()

            self.assertEqual( 2, statusChangeHandler.breakCount )
            self.assertEqual( statusChangeHandler.breakCount, statusChangeHandler.goCount )
            