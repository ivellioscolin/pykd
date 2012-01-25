"""Local variables tests"""

import unittest
import target
import pykd

class LocalVarsTest(unittest.TestCase):
    def testLocalVariable(self):
        """Start new process and test local variables"""

        newClnt = pykd.createDbgClient()
        newClnt.startProcess( target.appPath + " -testEnumWindows" )

        newClnt.go() # initial breakpoint -> wmain
        newClnt.go() # wmain -> targetapp!EnumWindowsProc1
        # pykd.dprint( "\n" + newClnt.dbgCommand("u") )

        locals = newClnt.getLocals()

        self.assertNotEqual( 0, locals["hWindow"] )
        self.assertEqual( pykd.DataIsParam, locals["hWindow"].dataKind() )

        self.assertEqual( 6, locals["lParam"] )
        self.assertEqual( pykd.DataIsParam, locals["lParam"].dataKind() )

        self.assertNotEqual( 0, locals["dwProccessId"] )
        self.assertEqual( pykd.DataIsLocal, locals["dwProccessId"].dataKind() )

        self.assertNotEqual( 0, locals["staticVar"] )
        self.assertEqual( pykd.DataIsStaticLocal, locals["staticVar"].dataKind() )

        self.assertEqual( locals["dwProccessId"] + 1, locals["staticVar"] )

        newClnt.go() # targetapp!EnumWindowsProc1 -> targetapp!functionCalledFromEnumWindowsProc1

        # get local variables from previous stack frame
        prevLocals = newClnt.getCurrentStack()[1].getLocals()

        self.assertEqual( len(prevLocals), len(locals) )
        for varName in locals.iterkeys():
            self.assertEqual( prevLocals[varName], locals[varName] )

        newClnt.go() # targetapp!EnumWindowsProc1 -> targetapp!EnumWindowsProc2
        locals = newClnt.getLocals()
        self.assertEqual( len(locals), 2 )
        locValues = locals.values()
        self.assertTrue( locValues[0] == 7 or locValues[1] == 7 )


