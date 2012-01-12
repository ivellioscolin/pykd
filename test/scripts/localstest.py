"""Local variables tests"""

import unittest
import target
import pykd

class LocalVarsTest(unittest.TestCase):
    def testLocalVariable(self):
        """Start new process and test local variables"""

        testClient = pykd.createDbgClient()
        testClient.startProcess( target.appPath + " -testEnumWindows" )

        testClient.go() # initial breakpoint -> wmain
        testClient.go() # wmain -> targetapp!EnumWindowsProc1
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

        self.assertEqual( locals["dwProccessId"] + 1, locals["staticVar"] )

        testClient.go() # targetapp!EnumWindowsProc1 -> targetapp!EnumWindowsProc2
        locals = testClient.getLocals()
        self.assertEqual( len(locals), 2 )
        locValues = locals.values()
        self.assertTrue( locValues[0] == 7 or locValues[1] == 7 )

