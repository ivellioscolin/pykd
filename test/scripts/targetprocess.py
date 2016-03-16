import unittest
import pykd
import target
import time

class ProcessTest(unittest.TestCase):

    def setUp(self):
        pykd.startProcess( target.appPath )
        pykd.startProcess( target.appPath )
        pykd.startProcess( target.appPath )

    def tearDown(self):
        pykd.killAllProcesses()

    def testGetCurrentProcess(self):
        proc = pykd.targetProcess.getCurrent()
        self.assertNotEqual(0, proc.systemID )
        self.assertNotEqual(0, proc.peb )

    def testEnumThreads(self):
        proc = pykd.targetProcess.getCurrent()
        threadNumber = proc.getNumberThreads()
        self.assertLess(0, threadNumber)
        for i in range(threadNumber):
            thread = proc.getThread(i)
            self.assertNotEqual(0, thread.systemID )
            self.assertNotEqual(0, thread.teb )

    def testEnumProcesses(self):
        processNumber = pykd.targetProcess.getNumber()
        for i in range(processNumber):
            proc = pykd.targetProcess(i)
            self.assertNotEqual(0, proc.systemID)
            self.assertNotEqual(0, proc.peb)

    def testSetCurrentThread(self):
        proc = pykd.targetProcess.getCurrent()
        threadNumber = proc.getNumberThreads()
        self.assertLess(0, threadNumber)
        for i in range(threadNumber):
            thread = proc.getThread(i)
            thread.setCurrent()

    def testGetBreakpoint(self):
        proc = pykd.targetProcess.getCurrent()
        self.assertEqual(0, proc.getNumberBreakpoints())






