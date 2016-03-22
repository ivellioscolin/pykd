""" breakpoints """

import unittest
import pykd
import target
import testutils

class callCounter(object):
    def __init__(self,func):
        self.count = 0
        self.func = func
    def __call__(self):
        self.count = self.count+1
        return self.func()

def stopOnBreak():
    return pykd.eventResult.Break

def continueOnBreak():
    return pykd.eventResult.Proceed


class BreakpointTest( unittest.TestCase ):

    def setUp(self):
        pykd.startProcess( target.appPath + " breakhandlertest" )
        self.targetModule = pykd.module( target.moduleName )
        self.targetModule.reload()
        pykd.go()

    def tearDown(self):
        self.taregtModule = None
        pykd.killAllProcesses()
 
    def testNoBreakpoint(self):
        self.assertEqual( pykd.executionStatus.NoDebuggee, pykd.go() )

    def testSetBp(self):
        bp = pykd.setBp( self.targetModule.CdeclFunc )
        self.assertEqual( pykd.executionStatus.Break, pykd.go() )

    def testRemoveBp(self):
        bp = pykd.setBp( self.targetModule.CdeclFunc )
        bp.remove()
        self.assertEqual( pykd.executionStatus.NoDebuggee, pykd.go() )

    def testDeleteBp(self):
        bp = pykd.setBp( self.targetModule.CdeclFunc )
        del bp
        self.assertEqual( pykd.executionStatus.NoDebuggee, pykd.go() )

    def testRemoveByIndex(self):
        bp1 = pykd.setBp( self.targetModule.CdeclFunc )
        bp2 = pykd.getBp(0)
        bp2.remove()
        self.assertEqual( pykd.executionStatus.NoDebuggee, pykd.go() )

    def testRemoveAllBp(self):
          bp1 =  pykd.setBp( self.targetModule.CdeclFunc )
          bp2 =  pykd.setBp( self.targetModule.CdeclFunc + 1)
          bp3 =  pykd.setBp( self.targetModule.CdeclFunc + 2 )
          self.assertEqual( 3, pykd.getNumberBreakpoints() )
          pykd.removeAllBp()
          self.assertEqual( 0, pykd.getNumberBreakpoints() )
          self.assertEqual( pykd.executionStatus.NoDebuggee, pykd.go() )

    def testBreakCallback(self):
        breakCount = callCounter(stopOnBreak)
        bp = pykd.setBp( self.targetModule.CdeclFunc, breakCount )
        self.assertEqual( pykd.executionStatus.Break, pykd.go() )
        self.assertEqual( 1, breakCount.count )

    def testNoBreakCallback(self):

        breakCount = callCounter(continueOnBreak)
        bp = pykd.setBp( self.targetModule.CdeclFunc, breakCount )
        self.assertEqual( pykd.executionStatus.NoDebuggee, pykd.go() )
        self.assertEqual( 1, breakCount.count )

    def testBreakpointHandler(self):

        class BreakpointHandler( pykd.eventHandler ):

            def __init__(self):
                super(BreakpointHandler, self).__init__()
                self.count = 0

            def onBreakpoint( self, bpid_):
                self.count = self.count + 1
                return True

        handler = BreakpointHandler()
        bp = pykd.setBp( self.targetModule.CdeclFunc )
        self.assertEqual( pykd.executionStatus.Break, pykd.go() )
        self.assertEqual( 1, handler.count )

    def testBreakpointClass(self):

        class MyBreakpoint(pykd.breakpoint):
             def __init__(self,offset):
                 super(MyBreakpoint, self).__init__(offset)
                 self.count = 0

             def onHit(self):
                 self.count = self.count + 1
                 return True

        bp = MyBreakpoint( self.targetModule.typedVar("CdeclFunc").getDebugStart() )
        self.assertEqual( pykd.executionStatus.Break, pykd.go() )
        self.assertEqual( 1, bp.count )


    def testBreakpointCondition1(self):

        def makebpcallback(n):

            def bpcallback():
                if pykd.getParam("b") > n:
                    return pykd.eventResult.Break
                return pykd.eventResult.Proceed
            return bpcallback

        bp = pykd.setBp( self.targetModule.typedVar("CdeclFunc").getDebugStart(), makebpcallback(1) )
        self.assertEqual( pykd.executionStatus.Break, pykd.go() )


    def testBreakpointCondition2(self):

        def makebpcallback(n):

            def bpcallback():
                if pykd.getParam("b") > n:
                    return pykd.eventResult.Break
                return pykd.eventResult.Proceed
            return bpcallback

        bp = pykd.setBp(self.targetModule.typedVar("CdeclFunc").getDebugStart(), makebpcallback(100) )
        self.assertEqual( pykd.executionStatus.NoDebuggee, pykd.go() )


    def testBreakpointEnum(self):

        b1 = pykd.setBp( self.targetModule.CdeclFunc)
        b2 = pykd.setBp( self.targetModule.CdeclFunc + 1)
        b3 = pykd.setBp( self.targetModule.CdeclFunc + 2)

        self.assertEqual(3, pykd.getNumberBreakpoints());
        bpLst = [pykd.getBp(i) for i in range(3)]
        self.assertEqual(3, len(bpLst))
        for bp in bpLst: bp.remove()

        self.assertEqual(0, pykd.getNumberBreakpoints());

    def testBreakpointChangeHandler(self):

        class BreakpointChangeHandler( pykd.eventHandler ):
            
            def __init__(self):
                pykd.eventHandler.__init__(self)
                self.count = 0

            def onChangeBreakpoints(self):
                 self.count += 1

        handler = BreakpointChangeHandler()
        bp = pykd.setBp(self.targetModule.CdeclFunc)
        bp.remove()
        self.assertEqual(0, pykd.getNumberBreakpoints())
        self.assertEqual(4, handler.count)

    def testLambdaBpContinue(self):
        bp1 = pykd.setBp(self.targetModule.CdeclFunc, lambda : None )
        bp2 = pykd.setBp(self.targetModule.CdeclFunc, lambda : False )
        bp3 = pykd.setBp(self.targetModule.CdeclFunc, lambda : 0 )
        self.assertEqual( pykd.executionStatus.NoDebuggee, pykd.go() )

    def testLambdaBpBreak(self):
        bp2 = pykd.setBp(self.targetModule.CdeclFunc, lambda : True )
        self.assertEqual( pykd.executionStatus.Break, pykd.go() )

    def testBpCommand(self):
        pykd.dbgCommand("bp 0x100")
        self.assertEqual(1, pykd.getNumberBreakpoints())
        bp = pykd.getBp(0)
        self.assertEqual(0x100, bp.getOffset())

