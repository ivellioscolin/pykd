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
import basetest
import typeinfo 
import regtest
import moduletest
import diatest
import dbgcmd
import clienttest
import eventtest
import typedvar
import memtest

def getTestSuite( singleName = "" ):
    if singleName == "":
        return unittest.TestSuite(
           [   unittest.TestLoader().loadTestsFromTestCase( basetest.BaseTest ),
               unittest.TestLoader().loadTestsFromTestCase( moduletest.ModuleTest ),
               unittest.TestLoader().loadTestsFromTestCase( diatest.DiaTest ),
               unittest.TestLoader().loadTestsFromTestCase( typeinfo.TypeInfoTest ),
               unittest.TestLoader().loadTestsFromTestCase( typedvar.TypedVarTest ),
               unittest.TestLoader().loadTestsFromTestCase( dbgcmd.DbgcmdTest ),
               unittest.TestLoader().loadTestsFromTestCase( clienttest.DbgClientTest ),
               unittest.TestLoader().loadTestsFromTestCase( eventtest.EventTest ),
               unittest.TestLoader().loadTestsFromTestCase( memtest.MemoryTest )
           ] ) 
    else:
       return unittest.TestSuite( unittest.TestLoader().loadTestsFromName( singleName ) )


if __name__ == "__main__":

    targetAppPath = sys.argv[1]
   
    target.moduleName = os.path.splitext(os.path.basename(targetAppPath))[0]
    print "\nTest module: %s" % targetAppPath
  
    pykd.startProcess( targetAppPath )

    target.module = pykd.loadModule( target.moduleName )
    target.module.reload();
    
    suite = getTestSuite()
    #suite = getTestSuite( "memtest.MemoryTest.testPtrRead" )
   
    unittest.TextTestRunner(stream=sys.stdout, verbosity=2).run( suite )
    
    #a = raw_input("\npress return\n")