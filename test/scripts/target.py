#
#
#

import unittest
import pykd

appPath = None
module = None
moduleName = None

class TargetTest( unittest.TestCase ):

    def testStartStop(self):
        processId = pykd.startProcess( appPath )
        pykd.killProcess( processId )



#module = None
#moduleName = None
#dbgext = None
#appPath = None
