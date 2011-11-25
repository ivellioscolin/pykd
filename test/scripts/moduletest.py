#
#
#

import unittest
import target
import pykd

class ModuleTest( unittest.TestCase ):
    
    def testCtor( self ):
        " module class can not be created direct """
        try: pykd.module()
        except RuntimeError: pass         
         
    def testName( self ):
        self.assertEqual( target.moduleName, target.module.name() )
         
    def testSize( self ):
        self.assertNotEqual( 0, target.module.size() )
         
    def testBegin( self ):
        self.assertNotEqual( 0, target.module.begin() )
             
    def testEnd( self ):
        self.assertEqual( target.module.size(), target.module.end() - target.module.begin() )
         
    def testPdb( self ):
        self.assertNotEqual( "", target.module.pdb() )
         
    def testImage( self ):
        self.assertEqual( target.module.name() + ".exe", target.module.image() )

    def testFindModule( self ):
    
        try: 
            pykd.loadModule( target.module.begin() - 0x10 )
            self.assertTrue( False )
        except pykd.BaseException: 
			self.assertTrue( True )

        self.assertNotEqual( None, pykd.loadModule( target.module.begin() ) )
        self.assertNotEqual( None, pykd.loadModule( target.module.begin() + 0x10) )

        try: 
            pykd.loadModule( target.module.end() )
            self.assertTrue( False )
        except pykd.BaseException: 
            self.assertTrue( True )
 
        try: 
            pykd.loadModule( target.module.end() + 0x10 )
            self.assertTrue( False )
        except pykd.BaseException: 
            self.assertTrue( True )

    def testSymbol( self ):
        self.assertEqual( target.module.rva("FuncWithName0"), target.module.offset("FuncWithName0") - target.module.begin() )
        self.assertEqual( target.module.rva("FuncWithName0"), target.module.FuncWithName0 - target.module.begin() )
        
    def testType( self ):
        self.assertEqual( "structTest", target.module.type("structTest").name() );
        self.assertEqual( "structTest", target.module.type("g_structTest").name() );
