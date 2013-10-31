import unittest
import pykd
import target

class StackTest(unittest.TestCase):
    
    def setUp(self):
        self.processId = pykd.startProcess( target.appPath + " stacktest" )
        pykd.go() # skip initial breakpoint

    def tearDown(self):
        pykd.killProcess( self.processId )

    def testGetStack(self):

        expectedStack = [ 'targetapp!stackTestRun2',
                  'targetapp!stackTestRun1',
                  'targetapp!stackTestRun',
                  'targetapp!wmain',
                  'targetapp!__tmainCRTStartup',
                  'targetapp!wmainCRTStartup',
                  'kernel32!BaseThreadInitThunk',
                  'ntdll!RtlUserThreadStart' ]

        realStack = []
        for frame in pykd.getStack():
            moduleName, symbolName, disp = pykd.findSymbolAndDisp( frame.ip )
            realStack.append( "%s!%s" % ( moduleName, symbolName ) )

        self.assertEqual( expectedStack, realStack )


