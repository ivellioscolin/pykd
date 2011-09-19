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
import regtest
import moduletest

def getTestSuite( singleName = "" ):
    if singleName == "":
        return unittest.TestSuite(
           [   unittest.TestLoader().loadTestsFromTestCase( basetest.BaseTest ),
               unittest.TestLoader().loadTestsFromTestCase( moduletest.ModuleTest ),
#               unittest.TestLoader().loadTestsFromTestCase( typeinfo.TypeInfoTest ),
#               unittest.TestLoader().loadTestsFromTestCase( regtest.CpuRegTest )
           ] ) 
    else:
       return unittest.TestSuite( unittest.TestLoader().loadTestsFromName( singleName ) )


if __name__ == "__main__":

    targetAppPath = sys.argv[1]
   
    target.moduleName = os.path.splitext(os.path.basename(targetAppPath))[0]
    print "\nTest module: %s" % targetAppPath
    
    dbg = pykd.dbgClient()
   
    dbg.startProcess( targetAppPath )
#    pykd.go()

    target.module = dbg.loadModule( target.moduleName )
    
    suite = getTestSuite()
   
    unittest.TextTestRunner(stream=sys.stdout, verbosity=2).run( suite )
