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

    def testNoBreakpoint(self):
        processId = pykd.startProcess( target.appPath + " breakhandlertest" )
        with testutils.ContextCallIt( testutils.KillProcess(processId) ) as killStartedProcess :
            pykd.go()
            self.assertEqual( pykd.executionStatus.NoDebuggee, pykd.go() )

    def testSetBp(self):
        processId = pykd.startProcess( target.appPath + " breakhandlertest" )
        targetModule = pykd.module( target.moduleName )
        targetModule.reload()
        with testutils.ContextCallIt( testutils.KillProcess(processId) ) as killStartedProcess :
            pykd.go()
            pykd.setBp( targetModule.CdeclFunc )
            self.assertEqual( pykd.executionStatus.Break, pykd.go() )

    def testRemoveBp(self):
        processId = pykd.startProcess( target.appPath + " breakhandlertest" )
        targetModule = pykd.module( target.moduleName )
        targetModule.reload()
        with testutils.ContextCallIt( testutils.KillProcess(processId) ) as killStartedProcess :
            pykd.go()
            bp = pykd.setBp(  targetModule.CdeclFunc )
            bp.remove()
            self.assertEqual( pykd.executionStatus.NoDebuggee, pykd.go() )

    def testBreakCallback(self):
        processId = pykd.startProcess( target.appPath + " breakhandlertest" )
        targetModule = pykd.module( target.moduleName )
        targetModule.reload()
        with testutils.ContextCallIt( testutils.KillProcess(processId) ) as killStartedProcess :
            pykd.go()
            breakCount = callCounter(stopOnBreak)
            pykd.setBp( targetModule.CdeclFunc, breakCount )
            self.assertEqual( pykd.executionStatus.Break, pykd.go() )
            self.assertEqual( 1, breakCount.count )

    def testBpScope(self):

        def setBpFunc():
           #breakpoint must be set until remove method will be called explicitly
           pykd.setBp(targetModule.CdeclFunc)

        processId = pykd.startProcess( target.appPath + " breakhandlertest" )
        targetModule = pykd.module( target.moduleName )
        targetModule.reload()
        with testutils.ContextCallIt( testutils.KillProcess(processId) ) as killStartedProcess :
            pykd.go()
            setBpFunc()
            self.assertEqual( pykd.executionStatus.Break, pykd.go() )
            

    def testNoBreakCallback(self):
        processId = pykd.startProcess( target.appPath + " breakhandlertest" )
        targetModule = pykd.module( target.moduleName )
        targetModule.reload()
        with testutils.ContextCallIt( testutils.KillProcess(processId) ) as killStartedProcess :
            pykd.go()

            breakCount = callCounter(continueOnBreak)

            pykd.setBp( targetModule.CdeclFunc, breakCount )

            self.assertEqual( pykd.executionStatus.NoDebuggee, pykd.go() )

            self.assertEqual( 1, breakCount.count )

    def testBreakpointHandler(self):

        class BreakpointHandler( pykd.eventHandler ):

            def __init__(self):
                super(BreakpointHandler, self).__init__()
                self.count = 0

            def onBreakpoint( self, bpid_):
                self.count = self.count + 1

        processId = pykd.startProcess( target.appPath + " breakhandlertest" )
        targetModule = pykd.module( target.moduleName )
        targetModule.reload()
        with testutils.ContextCallIt( testutils.KillProcess(processId) ) as killStartedProcess :

            pykd.go()

            handler = BreakpointHandler()

            pykd.setBp( targetModule.CdeclFunc )

            self.assertEqual( pykd.executionStatus.Break, pykd.go() )

            self.assertEqual( 1, handler.count )

    def testBreakpointClass(self):

        class MyBreakpoint(pykd.breakpoint):
             def __init__(self,offset):
                 super(MyBreakpoint, self).__init__(offset)
                 self.count = 0

             def onHit(self):
                 self.count = self.count + 1

        processId = pykd.startProcess( target.appPath + " breakhandlertest" )
        targetModule = pykd.module( target.moduleName )
        targetModule.reload()
        with testutils.ContextCallIt( testutils.KillProcess(processId) ) as killStartedProcess :

            pykd.go()

            bp = MyBreakpoint( targetModule.CdeclFunc )

            self.assertEqual( pykd.executionStatus.Break, pykd.go() )

            self.assertEqual( 1, bp.count )


    def testBreakpointCondition(self):

        def makebpcallback(n):

            def bpcallback():
                if pykd.getParam("b") > n:
                    return pykd.eventResult.Break
                return pykd.eventResult.Proceed
            return bpcallback

        processId = pykd.startProcess( target.appPath + " breakhandlertest" )
        targetModule = pykd.module( target.moduleName )
        targetModule.reload()
        with testutils.ContextCallIt( testutils.KillProcess(processId) ) as killStartedProcess :
            pykd.go()
            bp = pykd.setBp( targetModule.CdeclFunc, makebpcallback(1) )
            self.assertEqual( pykd.executionStatus.Break, pykd.go() )


        processId = pykd.startProcess( target.appPath + " breakhandlertest" )
        targetModule = pykd.module( target.moduleName )
        targetModule.reload()
        with testutils.ContextCallIt( testutils.KillProcess(processId) ) as killStartedProcess :
            pykd.go()
            bp = pykd.setBp( targetModule.CdeclFunc, makebpcallback(100) )
            self.assertEqual( pykd.executionStatus.NoDebuggee, pykd.go() )


    def testBreakpointEnum(self):
        processId = pykd.startProcess( target.appPath + " breakhandlertest" )
        targetModule = pykd.module( target.moduleName )
        targetModule.reload()
        with testutils.ContextCallIt( testutils.KillProcess(processId) ) as killStartedProcess :
            pykd.go()
            
            pykd.setBp( targetModule.CdeclFunc)
            pykd.setBp( targetModule.CdeclFunc + 1)
            pykd.setBp( targetModule.CdeclFunc + 2)

            self.assertEqual(3, pykd.getNumberBreakpoints());
            bpLst = [pykd.getBp(i) for i in xrange(3)]
            self.assertEqual(3, len(bpLst))
            map( lambda bp: bp.remove(), bpLst)
            self.assertEqual(0, pykd.getNumberBreakpoints());

