#
#
#

import sys
import os
import unittest

# Dynamically append current pykd.pyd path to PYTHONPATH
sys.path.append(os.path.dirname(sys.argv[1]))
import pykd

import target
import basetest
import typeinfo     


if __name__ == "__main__":

    targetAppPath = sys.argv[1]
    
    target.moduleName = os.path.splitext(os.path.basename(targetAppPath))[0]
    print "\nTest module: %s" % targetAppPath
    
    pykd.startProcess( targetAppPath )
    pykd.go()

    target.module = pykd.loadModule( target.moduleName )
   
    suite = unittest.TestSuite( [
            unittest.TestLoader().loadTestsFromTestCase( basetest.BaseTest ),
            unittest.TestLoader().loadTestsFromTestCase( typeinfo.TypeInfoTest ),
        ] )

    unittest.TextTestRunner().run( suite )
