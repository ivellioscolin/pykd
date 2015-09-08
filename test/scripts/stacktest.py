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

        expectedStack = [
                  'targetapp!stackTestClass::stackMethod',
                  'targetapp!stackTestRun2',
                  'targetapp!stackTestRun1',
                  'targetapp!stackTestRun']

        realStack = []
        for frame in pykd.getStack():
            moduleName, symbolName, disp = pykd.findSymbolAndDisp( frame.ip )
            realStack.append( "%s!%s" % ( moduleName, symbolName ) )

        self.assertEqual( expectedStack, realStack[0:4])

    def testGetParams(self):

        frame0 = pykd.getFrame()
        self.assertEqual( ["this", "a", "b"], [ name for name, param in frame0.getParams() ] )


        frame1 = pykd.getStack()[1]
        self.assertEqual( ["a", "b", "c"], [ name for name, param in frame1.getParams() ] )
        self.assertEqual( 10, frame1.getParam("a").deref() )
        self.assertEqual( 10, frame1.params["a"].deref() )

        frame2 = pykd.getStack()[2]
        self.assertEqual( 10, frame2.params["a"] )
        self.assertEqual( 10, frame2.getParam("a") )

    def testGetLocals(self):
        expectedLocals = ["localDouble", "localFloat", "localChars"]
        frame2 = pykd.getStack()[2]
        self.assertEqual( expectedLocals, [name for name, param in frame2.getLocals() ] )
        self.assertEqual( 0.0, frame2.locals["localDouble"] )

    def testGetParamsNoSymbol(self):
        topFrame = pykd.getStack()[-1]
        self.assertEqual(0, len(topFrame.getParams()))

    def testGetLocalsNoSymbol(self):
        topFrame = pykd.getStack()[-1]
        self.assertEqual(0, len(topFrame.getLocals()))


