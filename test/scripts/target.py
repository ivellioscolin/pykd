#
#
#

import unittest
import pykd

appPath = None
module = None
moduleName = None
processId = None

class TargetTest( unittest.TestCase ):

    def testStartStop(self):
        _locProcessId = pykd.startProcess( appPath )
        pykd.killProcess( _locProcessId )



#module = None
#moduleName = None
#dbgext = None
#appPath = None
