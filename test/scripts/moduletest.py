#
#
#

import unittest
import target
import pykd
import re

class ModuleTest( unittest.TestCase ):

    def testCtor( self ):
        self.assertEqual( target.module.name(), pykd.module(target.module.begin() ).name() )
        self.assertEqual( target.module.name(), pykd.module(target.module.name() ).name() )
         
    def testName( self ):
        self.assertEqual( target.moduleName, target.module.name() )
        
    def testSize( self ):
        self.assertNotEqual( 0, target.module.size() )
        self.assertTrue( pykd.isValid( target.module.begin() + target.module.size() - 1) )
         
    def testBegin( self ):
        self.assertNotEqual( 0, target.module.begin() )
        self.assertEqual( target.module.begin(), target.module )
        self.assertEqual( target.module.begin() + 100, target.module + 100 )
             
    def testEnd( self ):
        self.assertEqual( target.module.size(), target.module.end() - target.module.begin() )
        self.assertTrue( pykd.isValid( target.module.end() - 1) )
         
    def testPdb( self ):
       self.assertNotEqual( "", target.module.symfile() )
         
    def testImage( self ):
       self.assertEqual( target.module.name() + ".exe", target.module.image() )

    def testFindModule( self ):
        self.assertRaises( pykd.BaseException, pykd.module, target.module.begin() - 0x10 )

        self.assertNotEqual( None, pykd.module( target.module.begin() ) )
        self.assertNotEqual( None, pykd.module( target.module.begin() + 0x10) )

        self.assertRaises( pykd.BaseException, pykd.module, target.module.end() )
        self.assertRaises( pykd.BaseException, pykd.module, target.module.end() + 0x10 )

    def testSymbol( self ):
        self.assertEqual( target.module.rva("FuncWithName0"), target.module.offset("FuncWithName0") - target.module.begin() )
        self.assertEqual( target.module.rva("FuncWithName0"), target.module.FuncWithName0 - target.module.begin() )
        self.assertEqual( target.module.rva("FuncWithName0"), pykd.getOffset( target.module.name() + "!FuncWithName0") - target.module.begin() )
        
    def testFindSymbol( self ):
        self.assertEqual( "FuncWithName0", target.module.findSymbol( target.module.offset("FuncWithName0") ) )
        self.assertEqual( "_FuncWithName2", target.module.findSymbol( target.module.offset("_FuncWithName2") ) )
        
        
    def testType( self ):
        self.assertEqual( "structTest", target.module.type("structTest").name() );
        self.assertEqual( "structTest", target.module.type("g_structTest").name() );
        
    def testSourceFile( self ):
        fileName = pykd.getSourceFile(target.module.FuncWithName0 )
        self.assertTrue( re.search('targetapp\\.cpp', fileName ) )
        fileName, lineNo, displacement = pykd.getSourceLine( target.module.FuncWithName0 + 2)
        self.assertEqual( 382, lineNo )
        self.assertTrue( re.search('targetapp\\.cpp', fileName ) )
        self.assertEqual( 2, displacement )
        fileName, lineNo, displacement = pykd.getSourceLine()
        self.assertEqual( 624, lineNo )


