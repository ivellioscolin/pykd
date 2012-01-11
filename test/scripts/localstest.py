"""Local variables tests"""

import unittest
import target
import pykd

class LocalVarsTest(unittest.TestCase):
    def testLocalVariable(self):
        """Start new process and break in targetapp!EnumWindowsProc"""

        testClient = pykd.createDbgClient()
        testClient.startProcess( target.appPath + " -testEnumWindows" )

        testClient.go() # initial breakpoint -> wmain
        testClient.go() # wmain -> targetapp!EnumWindowsProc
        # pykd.dprint( "\n" + testClient.dbgCommand("u") )

        locals = testClient.getLocals()

        self.assertNotEqual( 0, locals["hWindow"] )
        self.assertEqual( pykd.DataIsParam, locals["hWindow"].dataKind() )

        self.assertEqual( 6, locals["lParam"] )
        self.assertEqual( pykd.DataIsParam, locals["lParam"].dataKind() )

        self.assertNotEqual( 0, locals["dwProccessId"] )
        self.assertEqual( pykd.DataIsLocal, locals["dwProccessId"].dataKind() )

        self.assertNotEqual( 0, locals["staticVar"] )
        self.assertEqual( pykd.DataIsStaticLocal, locals["staticVar"].dataKind() )

        self.assertEqual( locals["dwProccessId"], locals["staticVar"] )
