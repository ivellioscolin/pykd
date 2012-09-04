#
#
#

import sys
import os
import unittest

# Dynamically append current pykd.pyd path to PYTHONPATH
sys.path.insert(0, os.path.dirname(sys.argv[1]))

import pykd

import target

import intbase
import memtest
import moduletest
import typeinfo 
import typedvar
import regtest

class StartProcessWithoutParamsTest(unittest.TestCase):
    def testStart(self):
        target.processId = pykd.startProcess( target.appPath )
        target.module = pykd.module( target.moduleName )
        target.module.reload();
        pykd.go()

class TerminateProcessTest(unittest.TestCase):
    def testKill(self):
        pykd.killProcess( target.processId )

def getTestSuite( singleName = "" ):
    if singleName == "":
        return unittest.TestSuite(
           [
                unittest.TestLoader().loadTestsFromTestCase( StartProcessWithoutParamsTest ),
                unittest.TestLoader().loadTestsFromTestCase( target.TargetTest ),
                # *** Test without start/kill new processes
                unittest.TestLoader().loadTestsFromTestCase( intbase.IntBaseTest ),
                unittest.TestLoader().loadTestsFromTestCase( moduletest.ModuleTest ),
                unittest.TestLoader().loadTestsFromTestCase( memtest.MemoryTest ),
                unittest.TestLoader().loadTestsFromTestCase( typeinfo.TypeInfoTest ),
                unittest.TestLoader().loadTestsFromTestCase( typedvar.TypedVarTest ),
                unittest.TestLoader().loadTestsFromTestCase( regtest.CpuRegTest ),
                # *** 
                unittest.TestLoader().loadTestsFromTestCase( TerminateProcessTest ),
            ] ) 
    else:
       return unittest.TestSuite( unittest.TestLoader().loadTestsFromName( singleName ) )

if __name__ == "__main__":

    print "\nTesting PyKd ver. " + pykd.version
    
    target.appPath = sys.argv[1]
    target.moduleName = os.path.splitext(os.path.basename(target.appPath))[0]
    #print "Test module: %s" % target.appPath

    unittest.TextTestRunner(stream=sys.stdout, verbosity=2).run( getTestSuite() )
