"""Local variables tests"""

import unittest
import target
import pykd
import testutils


def testEnumWindowsProc1Locals(testCase, locals):
    testCase.assertNotEqual( 0, locals["hWindow"] )
    DataIsParam = 3
    testCase.assertEqual( DataIsParam, locals["hWindow"].dataKind() )

    testCase.assertEqual( 6, locals["lParam"] )
    testCase.assertEqual( DataIsParam, locals["lParam"].dataKind() )

    DataIsLocal = 1
    testCase.assertNotEqual( 0, locals["dwProccessId"] )
    testCase.assertEqual( DataIsLocal, locals["dwProccessId"].dataKind() )

    DataIsStaticLocal = 2
    testCase.assertNotEqual( 0, locals["staticVar"] )
    testCase.assertEqual( DataIsStaticLocal, locals["staticVar"].dataKind() )

    testCase.assertEqual( locals["dwProccessId"] + 1, locals["staticVar"] )

class LocalVarsTest(unittest.TestCase):
    def testLocalVariable(self):
        """Start new process and test local variables"""
        _locProcessId = pykd.startProcess( target.appPath + " -testEnumWindows" )
        with testutils.ContextCallIt( testutils.KillProcess(_locProcessId) ) as killStartedProcess :
            pykd.go() # initial breakpoint -> wmain
            pykd.go() # wmain -> targetapp!EnumWindowsProc1

            testEnumWindowsProc1Locals(self, pykd.getLocals())

            pykd.go() # targetapp!EnumWindowsProc1 -> targetapp!functionCalledFromEnumWindowsProc1
            testEnumWindowsProc1Locals(self, pykd.getCurrentStack()[1].getLocals())

            pykd.go() # targetapp!EnumWindowsProc1 -> targetapp!EnumWindowsProc2
            locals = pykd.getLocals()
            self.assertEqual( len(locals), 2 )
            locValues = locals.values()
            self.assertTrue( locValues[0] == 7 or locValues[1] == 7 )
