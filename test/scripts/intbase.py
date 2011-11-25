
import unittest
import target
from pykd import intBase

class IntBaseTest( unittest.TestCase ):

    def testCtor( self ):
        a = intBase(0xFF)
        a = intBase(0xFFFF)
        a = intBase(0xFFFFFFFF)
        a = intBase(0x8000000000000000)
        a = intBase(0xFFFFFFFFFFFFFFFF)
        a = intBase(-20)
        a = intBase(-2000)
        a = intBase(-200000)
        a = intBase(-20000000000) 
        a = intBase(-0xFFFFFFFFFFFFFFFF )
        a = intBase( True )
        
    def testEq( self ):
        self.assertTrue( 0xFF == intBase(0xFF) and intBase(0xFF) == 0xFF )
        self.assertTrue( 0xFFFF == intBase(0xFFFF) and 0xFFFF == intBase(0xFFFF) )
        self.assertTrue( 0xFFFFFFFF ==  intBase(0xFFFFFFFF) and  intBase(0xFFFFFFFF) == 0xFFFFFFFF )
        self.assertTrue( 0x8000000000000000 == intBase(0x8000000000000000) )
        self.assertTrue( 0xFFFFFFFFFFFFFFFF ==  intBase(0xFFFFFFFFFFFFFFFF) )
        self.assertTrue( -20 == intBase(-20) )
        self.assertTrue( -2000 == intBase(-2000) )
        self.assertTrue( -20000000000 == intBase(-20000000000) )         
        self.assertTrue( -0x8000000000000000 == intBase(-0x8000000000000000) )
        self.assertTrue( intBase(0x20L) == intBase(0x20) )
        self.assertTrue( True == intBase(True) )
        self.assertTrue( False == intBase(0) )
        self.assertTrue( True == intBase(1) )
        
    def testNe( self ):
        self.assertTrue( 0xFE != intBase(0xFF) )
        self.assertTrue( 0xFF00 !=  intBase(0xFFFF) )
        self.assertTrue( 0xFFFFFF88 != intBase(0xFFFFFFFF) )
        self.assertTrue( 0x8000000000000000 - 1 != intBase(0x8000000000000000) )
        self.assertTrue( 0xFFFFFFFFFFFFFFFF - 1 !=  intBase(0xFFFFFFFFFFFFFFFF) )
        self.assertTrue( -20 + 1 != intBase(-20) )        
        self.assertTrue( -2000 + 1 != intBase(-2000) )        
        self.assertTrue( -20000000000 + 1 != intBase(-20000000000) )
        self.assertTrue( -0x8000000000000000 - 1 != intBase(-0x8000000000000000) )        
        
    def testLtGt( self ):
        self.assertTrue( 0xFE < intBase(0xFF) and intBase(0xFE) < 0xFF )
        self.assertFalse( -99 < intBase(-100) and intBase(-99) < - 100 )
        self.assertTrue( 0xFFFFFFFFFFFFFFFE < intBase(0xFFFFFFFFFFFFFFFF) )
        self.assertFalse(0xFFFFFFFFFFFFFFFF < intBase(0xFFFFFFFFFFFFFFFE) )
        self.assertTrue( intBase(0xFFFFFFFFFFFFFFFE) < 0xFFFFFFFFFFFFFFFF )
        
    def testLeGe( self ):
        self.assertTrue( 0xFE <= intBase(0xFF) and intBase(0xFE) <= 0xFF )
        self.assertTrue( 0xFF <= intBase(0xFF) )
        self.assertFalse( -99 <= intBase(-100) and intBase(-99) <= - 100 )
        self.assertTrue( 0xFFFFFFFFFFFFFFFE <= intBase(0xFFFFFFFFFFFFFFFF) )
        self.assertFalse(0xFFFFFFFFFFFFFFFF <= intBase(0xFFFFFFFFFFFFFFFE) )
        self.assertTrue( intBase(0xFFFFFFFFFFFFFFFF) <= 0xFFFFFFFFFFFFFFFF )
                
    def testAdd( self ):
        self.assertEqual( 10, intBase(5) + 5 )
        self.assertEqual( 10, 5 + intBase(5) )
        a = 10
        a += intBase(10)
        self.assertEqual( 20, a )
        self.assertEqual( -20, intBase(-10) + (-10) )
        self.assertEqual( 10, intBase(-10) + 20 )
        self.assertEqual( 0x7fffffffffffffff + 1, intBase(0x7fffffffffffffff) + 1)
        self.assertEqual( -0x8000000000000000 + 10, intBase(-0x8000000000000000) + 10 )
        self.assertEqual( 0, intBase(-0x8000000000000000) + 0x8000000000000000 )

    def testSub( self ):
        self.assertEqual( 0, intBase(5) - 5 )
        self.assertEqual( 10, 15 - intBase(5) )
        a = 10
        a -= intBase(5)
        self.assertEqual( 5, a )
        self.assertEqual( -20, intBase(-10) -10 )
        self.assertEqual( -10, 10 - intBase(20) )
        self.assertEqual( -0xFFFFFFFF - 1, intBase(-0xFFFFFFFF) - 1 )
        
    def testMul( self ):
        self.assertEqual( 4, intBase(2) * 2 )
        self.assertEqual( 4, 2 * intBase(2) )
        self.assertEqual( -4, 2 * intBase(-2) )
        self.assertEqual( 4, -2 * intBase(-2) )
        self.assertEqual( 0x7fffffffffffffff * 2, intBase(0x7fffffffffffffff) * 2)
        self.assertEqual( 0x80000000*2, intBase(0x80000000)*2 )
        self.assertEqual( -0x80000000*2, 2 * intBase(-0x80000000))
        
    def testDiv( self ):
        self.assertEqual( 1, intBase(2) / 2 )
        self.assertEqual( 2, 5 / intBase(2) )
        self.assertEqual( -1, 2 / intBase(-2) )
        self.assertEqual( 1, -2 / intBase(-2) )
        
        try: 
            -2 / intBase(0)
            self.assertTrue( False )
        except ZeroDivisionError:
            self.assertTrue( True )

        try:
            intBase(2)/0
            self.assertTrue( False )
        except ZeroDivisionError:
            self.assertTrue( True )
            
    def testMod( self ):
        self.assertEqual( 1, intBase(3) % 2 )            
        self.assertEqual( 0, intBase(3) % 3 )            
        self.assertEqual( 1, 3 % intBase(2) )            
        self.assertEqual( 0, 3 % intBase(3) )          
        
    def testShift( self ):
        self.assertEqual( 0xFFFFFFFF >> 8, intBase(0xFFFFFFFF) >> 8 )
        self.assertEqual( 0x00FFFFFF << 8, intBase(0x00FFFFFF) << 8 )
        self.assertEqual( 0xFFFFFFFF >> 8, 0xFFFFFFFF >> intBase(8) )
        self.assertEqual( 0x00FFFFFF << 8, 0x00FFFFFF << intBase(8) )
    
    def testAnd( self ):
        self.assertEqual( 0xFFFFFFFF & 0xFFFF, intBase(0xFFFFFFFF) & 0xFFFF )
        self.assertEqual( 0xFFFFFFFF & 0xFFFF, 0xFFFFFFFF & intBase(0xFFFF) )
        self.assertEqual( -0xFFFFFFFF & 0xFFFF, intBase(-0xFFFFFFFF) & 0xFFFF )
        self.assertEqual( -0xFFFFFFFF & 0xFFFF, -0xFFFFFFFF & intBase(0xFFFF) )
        
    def testOr( self ):
        self.assertEqual( 0xFFFF0000 | 0xFFFF, intBase(0xFFFF0000) | 0xFFFF )
        self.assertEqual( 0xFFFF0000 | 0xFFFF, 0xFFFF0000 | intBase(0xFFFF) )
        self.assertEqual( -0xFFFF0000 | 0xFFFF, intBase(-0xFFFF0000) | 0xFFFF )
        self.assertEqual( -0xFFFF0000 | 0xFFFF, -0xFFFF0000 | intBase(0xFFFF) )        
        
    def testXor( self ):
        self.assertEqual( 0xFFFFFFFF ^ 0xFFFF, intBase(0xFFFFFFFF) ^ 0xFFFF )
        self.assertEqual( 0xFFFFFFFF ^ 0xFFFF, 0xFFFFFFFF ^ intBase(0xFFFF) )
        self.assertEqual( -0xFFFFFFFF ^ 0xFFFF, intBase(-0xFFFFFFFF) ^ 0xFFFF )
        self.assertEqual( -0xFFFFFFFF ^ 0xFFFF, -0xFFFFFFFF ^ intBase(0xFFFF) )        
        
    def testUnary( self ):
        self.assertEqual( -0xFFFFFFFF, -intBase(0xFFFFFFFF) )
        self.assertEqual( 0xFFFFFFFF, +intBase(0xFFFFFFFF) )
        self.assertEqual( 0, ~intBase(0xFFFFFFFF) )
    
            