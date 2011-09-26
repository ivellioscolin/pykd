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
    
        try: pykd.findModule( target.module.begin() - 0x10 )
        except pykd.BaseException: pass
        #self.assertRaises( pykd.BaseException, pykd.findModule, target.module.begin() - 0x10 ) 

        self.assertNotEqual( None, pykd.findModule( target.module.begin() ) )
        self.assertNotEqual( None, pykd.findModule( target.module.begin() + 0x10) )

        try: pykd.findModule( target.module.end() )
        except pykd.BaseException: pass
 
        try: pykd.findModule( target.module.end() + 0x10)
        except pykd.BaseException: pass

    def testSymbols( self ):
        syms = target.module.symbols()
        self.assertNotEqual( 0, len( syms ) )
