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
        for i in xrange(threadNumber):
            thread = proc.thread(i)
            self.assertNotEqual(0, thread.systemID )
            self.assertNotEqual(0, thread.teb )

    def testEnumProcesses(self):
        processNumber = pykd.targetProcess.getNumber()
        for i in xrange(processNumber):
            proc = pykd.targetProcess.getProcess(i)
            self.assertNotEqual(0, proc.systemID)
            self.assertNotEqual(0, proc.peb)

    def testSetCurrentThread(self):
        proc = pykd.targetProcess.getCurrent()
        threadNumber = proc.getNumberThreads()
        self.assertLess(0, threadNumber)
        for i in xrange(threadNumber):
            thread = proc.thread(i)
            thread.setCurrent()






