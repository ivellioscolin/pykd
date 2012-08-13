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
  
def getTestSuite( singleName = "" ):
    if singleName == "":
        return unittest.TestSuite(
           [
                unittest.TestLoader().loadTestsFromTestCase( target.TargetTest ),
                unittest.TestLoader().loadTestsFromTestCase( intbase.IntBaseTest ),
                unittest.TestLoader().loadTestsFromTestCase( moduletest.ModuleTest ),
                unittest.TestLoader().loadTestsFromTestCase( memtest.MemoryTest ),
                unittest.TestLoader().loadTestsFromTestCase( typeinfo.TypeInfoTest ),
                unittest.TestLoader().loadTestsFromTestCase( typedvar.TypedVarTest ),
                unittest.TestLoader().loadTestsFromTestCase( regtest.CpuRegTest ),
            ] ) 
    else:
       return unittest.TestSuite( unittest.TestLoader().loadTestsFromName( singleName ) )

if __name__ == "__main__":

    print "\nTesting PyKd ver. " + pykd.version
    
    target.appPath = sys.argv[1]
    target.moduleName = os.path.splitext(os.path.basename(target.appPath))[0]
    #print "Test module: %s" % target.appPath

    processId = pykd.startProcess( target.appPath )
    target.module = pykd.module( target.moduleName )
    target.module.reload();
    
    pykd.go()
   
    unittest.TextTestRunner(stream=sys.stdout, verbosity=2).run( getTestSuite() )

    pykd.killProcess( processId )











#import target
#import moduletest

#import basetest
#import typeinfo 
#import regtest
#import moduletest
#import diatest
#import dbgcmd
#import clienttest
#import eventtest
#import typedvar
#import memtest
#import intbase
#import synsymtest
#import ehloadtest
#import thrdctxtest
#import localstest
#import ehexcepttest

#def getTestSuite( singleName = "" ):
#    if singleName == "":
#        return unittest.TestSuite(
#           [
#                unittest.TestLoader().loadTestsFromTestCase( target.TargetTest ),
#                unittest.TestLoader().loadTestsFromTestCase( moduletest.ModuleTest ),
                
                
#               unittest.TestLoader().loadTestsFromTestCase( basetest.BaseTest ),
#               unittest.TestLoader().loadTestsFromTestCase( moduletest.ModuleTest ),
#               unittest.TestLoader().loadTestsFromTestCase( diatest.DiaTest ),
#               unittest.TestLoader().loadTestsFromTestCase( typeinfo.TypeInfoTest ),
#               unittest.TestLoader().loadTestsFromTestCase( typedvar.TypedVarTest ),
#               unittest.TestLoader().loadTestsFromTestCase( dbgcmd.DbgcmdTest ),
#               unittest.TestLoader().loadTestsFromTestCase( clienttest.DbgClientTest ),
#               unittest.TestLoader().loadTestsFromTestCase( eventtest.EventTest ),
#               unittest.TestLoader().loadTestsFromTestCase( memtest.MemoryTest ),
#               unittest.TestLoader().loadTestsFromTestCase( intbase.IntBaseTest ),
#               unittest.TestLoader().loadTestsFromTestCase( synsymtest.SynSymTest ),
#               unittest.TestLoader().loadTestsFromTestCase( thrdctxtest.ThreadContextTest ),
#               unittest.TestLoader().loadTestsFromTestCase( regtest.CpuRegTest ),
#           ] ) 
#    else:
#       return unittest.TestSuite( unittest.TestLoader().loadTestsFromName( singleName ) )

#def getNewProcessTestSuite():
#    return unittest.TestSuite(
#        [
#            unittest.TestLoader().loadTestsFromTestCase( ehloadtest.EhLoadTest ),
#            # unittest.TestLoader().loadTestsFromTestCase( localstest.LocalVarsTest ),
#            # unittest.TestLoader().loadTestsFromTestCase( ehexcepttest.EhExceptionBreakpointTest ),
#        ] )


#if __name__ == "__main__":

#    print "\nTesting PyKd ver. " + pykd.version
    
#    target.appPath = sys.argv[1]
    
#    unittest.TextTestRunner(stream=sys.stdout, verbosity=2).run( getTestSuite() )
    
    
    
    
    
    
    

    #target.appPath = sys.argv[1]

    #target.moduleName = os.path.splitext(os.path.basename(target.appPath))[0]
    #print "Test module: %s" % target.appPath

    #pykd.startProcess( target.appPath )

    #target.module = pykd.loadModule( target.moduleName )
    #target.module.reload();

    #pykd.go()

    #print ""

    #oneProcessTests = getTestSuite()
    
    #oneProcessTests = getTestSuite( "diatest.DiaTest.testFind" )
    #oneProcessTests = getTestSuite( "typedvar.TypedVarTest.testTypeVarArg" )
    #oneProcessTests = getTestSuite( "typeinfo.TypeInfoTest.testCreateByName" )
    #oneProcessTests = getTestSuite( "typedvar.TypedVarTest.testBitField" )

    #unittest.TextTestRunner(stream=sys.stdout, verbosity=2).run( oneProcessTests )

    #pykd.killProcess()

    #unittest.TextTestRunner(stream=sys.stdout, verbosity=2).run( getNewProcessTestSuite() )

    #raw_input("\npress return\n")
