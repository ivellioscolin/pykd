
import unittest
import target
import pykd

class OutHandler( pykd.eventHandler ):

    def __init__(self):
        pykd.eventHandler.__init__(self)
        self.out_counter=0
        self.verbose_counter = 0
        
    def onDebugOutput(self, text, mask):
        if mask == pykd.outputFlag.Normal:
            self.out_counter += 1
        elif mask == pykd.outputFlag.Verbose:
            self.verbose_counter += 1    

class OutputHandlerTest( unittest.TestCase ):

    def setUp(self):
        self.oldMask = pykd.getOutputMask()
        self.handler = OutHandler()     

    def tearDown(self):
        pykd.killAllProcesses()
        pykd.setOutputMask(self.oldMask)

    def testDebugOutput(self):
        pykd.startProcess( target.appPath )
        self.assertTrue( 0 < self.handler.out_counter)
        self.assertFalse( 0 < self.handler.verbose_counter)

    def testDebugOutputMask(self):
        pykd.setOutputMask(pykd.outputFlag.Normal | pykd.outputFlag.Verbose)
        pykd.startProcess( target.appPath )
        self.assertTrue( 0 < self.handler.out_counter)
        self.assertTrue( 0 < self.handler.verbose_counter)