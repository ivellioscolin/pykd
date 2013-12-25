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

    def testGetParams(self):
        expectedParams = ["a", "b", "c"]
        self.assertEqual( expectedParams, [ name for name, param in pykd.getParams()]  )
        self.assertEqual( 10, dict(pykd.getParams())["a"].deref() )
        self.assertEqual( 10, pykd.getParam("a").deref() )
         
        self.assertEqual( 10, pykd.getStack()[1].params["a"] )
        self.assertEqual( 10, pykd.getStack()[1].getParam("a") )
        self.assertEqual( 10, dict( pykd.getStack()[1].getParams() )["a"] )
