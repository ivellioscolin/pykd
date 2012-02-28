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
import intbase
import synsymtest
import ehloadtest
import thrdctxtest
import localstest
import ehexcepttest

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
               unittest.TestLoader().loadTestsFromTestCase( memtest.MemoryTest ),
               unittest.TestLoader().loadTestsFromTestCase( intbase.IntBaseTest ),
               unittest.TestLoader().loadTestsFromTestCase( synsymtest.SynSymTest ),
               unittest.TestLoader().loadTestsFromTestCase( thrdctxtest.ThreadContextTest ),
               unittest.TestLoader().loadTestsFromTestCase( ehloadtest.EhLoadTest ),
               unittest.TestLoader().loadTestsFromTestCase( localstest.LocalVarsTest ),
               unittest.TestLoader().loadTestsFromTestCase( ehexcepttest.EhExceptionBreakpointTest ),
               unittest.TestLoader().loadTestsFromTestCase( regtest.CpuRegTest ),
           ] ) 
    else:
       return unittest.TestSuite( unittest.TestLoader().loadTestsFromName( singleName ) )


if __name__ == "__main__":

    print "\nTesting PyKd ver. " + pykd.version

    target.appPath = sys.argv[1]

    target.moduleName = os.path.splitext(os.path.basename(target.appPath))[0]
    print "Test module: %s" % target.appPath

    pykd.startProcess( target.appPath )

    target.module = pykd.loadModule( target.moduleName )
    target.module.reload();

    pykd.go()

    print ""

    suite = getTestSuite()
    #suite = getTestSuite( "typedvar.TypedVarTest.testTypeVarArg" )
    #suite = getTestSuite( "typeinfo.TypeInfoTest.testBitField" )

    unittest.TextTestRunner(stream=sys.stdout, verbosity=2).run( suite )

    #raw_input("\npress return\n")
