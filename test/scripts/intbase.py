
import unittest
import target
from pykd import numVariant

class IntBaseTest( unittest.TestCase ):

    def testCtor( self ):
        a = numVariant(0xFF)
        a = numVariant(0xFFFF)
        a = numVariant(0xFFFFFFFF)
        a = numVariant(0x8000000000000000)
        a = numVariant(0xFFFFFFFFFFFFFFFF)
        a = numVariant(-20)
        a = numVariant(-2000)
        a = numVariant(-200000)
        a = numVariant(-20000000000) 
        a = numVariant(-0xFFFFFFFFFFFFFFFF )
        a = numVariant( True )
        
    def testEq( self ):
        self.assertTrue( 0xFF == numVariant(0xFF) and numVariant(0xFF) == 0xFF )
        self.assertTrue( 0xFFFF == numVariant(0xFFFF) and 0xFFFF == numVariant(0xFFFF) )
        self.assertTrue( 0xFFFFFFFF ==  numVariant(0xFFFFFFFF) and  numVariant(0xFFFFFFFF) == 0xFFFFFFFF )
        self.assertTrue( 0x8000000000000000 == numVariant(0x8000000000000000) )
        self.assertTrue( 0xFFFFFFFFFFFFFFFF ==  numVariant(0xFFFFFFFFFFFFFFFF) )
        self.assertTrue( -20 == numVariant(-20) )
        self.assertTrue( -2000 == numVariant(-2000) )
        self.assertTrue( -0x7FFFFFFF == numVariant(-0x7FFFFFFF) )
        self.assertTrue( -20000000000 == numVariant(-20000000000) )         
        self.assertTrue( -0x8000000000000000 == numVariant(-0x8000000000000000) )
        self.assertTrue( numVariant(0x20L) == numVariant(0x20) )
        self.assertTrue( True == numVariant(True) )
        self.assertTrue( False == numVariant(0) )
        self.assertTrue( True == numVariant(1) )
        self.assertTrue( numVariant(1) == numVariant(1) )
        
    def testNe( self ):
        self.assertTrue( 0xFE != numVariant(0xFF) )
        self.assertTrue( 0xFF00 !=  numVariant(0xFFFF) )
        self.assertTrue( 0xFFFFFF88 != numVariant(0xFFFFFFFF) )
        self.assertTrue( 0x8000000000000000 - 1 != numVariant(0x8000000000000000) )
        self.assertTrue( 0xFFFFFFFFFFFFFFFF - 1 !=  numVariant(0xFFFFFFFFFFFFFFFF) )
        self.assertTrue( -20 + 1 != numVariant(-20) )        
        self.assertTrue( -2000 + 1 != numVariant(-2000) )        
        self.assertTrue( -20000000000 + 1 != numVariant(-20000000000) )
        self.assertTrue( -0x8000000000000000 - 1 != numVariant(-0x8000000000000000) )
        self.assertTrue( numVariant(1) != numVariant(2) )
        
    def testLtGt( self ):
        self.assertTrue( 0xFE < numVariant(0xFF) and numVariant(0xFE) < 0xFF )
        self.assertFalse( -99 < numVariant(-100) and numVariant(-99) < - 100 )
        self.assertTrue( 0xFFFFFFFFFFFFFFFE < numVariant(0xFFFFFFFFFFFFFFFF) )
        self.assertFalse(0xFFFFFFFFFFFFFFFF < numVariant(0xFFFFFFFFFFFFFFFE) )
        self.assertTrue( numVariant(0xFFFFFFFFFFFFFFFE) < 0xFFFFFFFFFFFFFFFF )
        self.assertTrue( numVariant(1) < numVariant(2) )
        
    def testLeGe( self ):
        self.assertTrue( 0xFE <= numVariant(0xFF) and numVariant(0xFE) <= 0xFF )
        self.assertTrue( 0xFF <= numVariant(0xFF) )
        self.assertFalse( -99 <= numVariant(-100) and numVariant(-99) <= - 100 )
        self.assertTrue( 0xFFFFFFFFFFFFFFFE <= numVariant(0xFFFFFFFFFFFFFFFF) )
        self.assertFalse(0xFFFFFFFFFFFFFFFF <= numVariant(0xFFFFFFFFFFFFFFFE) )
        self.assertTrue( numVariant(0xFFFFFFFFFFFFFFFF) <= 0xFFFFFFFFFFFFFFFF )
        self.assertFalse( numVariant(1) >= numVariant(2) )
                
    def testAdd( self ):
        self.assertEqual( 10, numVariant(5) + 5 )
        self.assertEqual( 10, 5 + numVariant(5) )
        a = 10
        a += numVariant(10)
        self.assertEqual( 20, a )
        self.assertEqual( -20, numVariant(-10) + (-10) )
        self.assertEqual( 10, numVariant(-10) + 20 )
        self.assertEqual( 0x7fffffffffffffff + 1, numVariant(0x7fffffffffffffff) + 1)
        self.assertEqual( -0x8000000000000000 + 10, numVariant(-0x8000000000000000) + 10 )
        self.assertEqual( 0, numVariant(-0x8000000000000000) + 0x8000000000000000 )
        self.assertEqual( 5, numVariant(3) + numVariant(2) )

    def testSub( self ):
        self.assertEqual( 0, numVariant(5) - 5 )
        self.assertEqual( 10, 15 - numVariant(5) )
        a = 10
        a -= numVariant(5)
        self.assertEqual( 5, a )
        self.assertEqual( -20, numVariant(-10) -10 )
        self.assertEqual( -10, 10 - numVariant(20) )
        self.assertEqual( -0xFFFFFFFF - 1, numVariant(-0xFFFFFFFF) - 1 )
        self.assertEqual( 5, numVariant(7) - numVariant(2) )
        
    def testMul( self ):
        self.assertEqual( 4, numVariant(2) * 2 )
        self.assertEqual( 4, 2 * numVariant(2) )
        self.assertEqual( -4, 2 * numVariant(-2) )
        self.assertEqual( 4, -2 * numVariant(-2) )
        self.assertEqual( 0x7fffffffffffffff * 2, numVariant(0x7fffffffffffffff) * 2)
        self.assertEqual( 0x80000000*2, numVariant(0x80000000)*2 )
        self.assertEqual( -0x80000000*2, 2 * numVariant(-0x80000000))
        self.assertEqual( 14, numVariant(7)*numVariant(2) )
        
    def testDiv( self ):
        self.assertEqual( 1, numVariant(2) / 2 )
        self.assertEqual( 2, 5 / numVariant(2) )
        self.assertEqual( -1, 2 / numVariant(-2) )
        self.assertEqual( 1, -2 / numVariant(-2) )
        self.assertEqual( 3, numVariant(7)/numVariant(2) )
        
        try: 
            -2 / numVariant(0)
            self.assertTrue( False )
        except ZeroDivisionError:
            self.assertTrue( True )

        try:
            numVariant(2)/0
            self.assertTrue( False )
        except ZeroDivisionError:
            self.assertTrue( True )
            
        try:
            numVariant(0)/numVariant(0)
            self.assertTrue( False )
        except ZeroDivisionError:
            self.assertTrue( True )
            
    def testMod( self ):
        self.assertEqual( 1, numVariant(3) % 2 )
        self.assertEqual( 0, numVariant(3) % 3 )
        self.assertEqual( 1, 3 % numVariant(2) )
        self.assertEqual( 0, 3 % numVariant(3) )
        self.assertEqual( 2, numVariant(5) % numVariant(3) )
        
    def testShift( self ):
        self.assertEqual( 0xFFFFFFFF >> 8, numVariant(0xFFFFFFFF) >> 8 )
        self.assertEqual( 0x00FFFFFF << 8, numVariant(0x00FFFFFF) << 8 )
        self.assertEqual( 0xFFFFFFFF >> 8, 0xFFFFFFFF >> numVariant(8) )
        self.assertEqual( 0x00FFFFFF << 8, 0x00FFFFFF << numVariant(8) )
    
    def testAnd( self ):
        self.assertEqual( 0xFFFFFFFF & 0xFFFF, numVariant(0xFFFFFFFF) & 0xFFFF )
        self.assertEqual( 0xFFFFFFFF & 0xFFFF, 0xFFFFFFFF & numVariant(0xFFFF) )
        self.assertEqual( -0xFFFFFFFF & 0xFFFF, numVariant(-0xFFFFFFFF) & 0xFFFF )
        self.assertEqual( -0xFFFFFFFF & 0xFFFF, -0xFFFFFFFF & numVariant(0xFFFF) )
        
    def testOr( self ):
        self.assertEqual( 0xFFFF0000 | 0xFFFF, numVariant(0xFFFF0000) | 0xFFFF )
        self.assertEqual( 0xFFFF0000 | 0xFFFF, 0xFFFF0000 | numVariant(0xFFFF) )
        self.assertEqual( -0xFFFF0000 | 0xFFFF, numVariant(-0xFFFF0000) | 0xFFFF )
        self.assertEqual( -0xFFFF0000 | 0xFFFF, -0xFFFF0000 | numVariant(0xFFFF) )        
        
    def testXor( self ):
        self.assertEqual( 0xFFFFFFFF ^ 0xFFFF, numVariant(0xFFFFFFFF) ^ 0xFFFF )
        self.assertEqual( 0xFFFFFFFF ^ 0xFFFF, 0xFFFFFFFF ^ numVariant(0xFFFF) )
        self.assertEqual( -0xFFFFFFFF ^ 0xFFFF, numVariant(-0xFFFFFFFF) ^ 0xFFFF )
        self.assertEqual( -0xFFFFFFFF ^ 0xFFFF, -0xFFFFFFFF ^ numVariant(0xFFFF) )        
        
    def testUnary( self ):
        self.assertEqual( -0xFFFFFFFF, -numVariant(0xFFFFFFFF) )
        self.assertEqual( 0xFFFFFFFF, +numVariant(0xFFFFFFFF) )
        self.assertEqual( 0, ~numVariant(0xFFFFFFFF) )
        
    def testLongConvert( self ):
        self.assertEqual( "100", "%d" % numVariant(100) )
        self.assertEqual( "64", "%x" % numVariant(100) )
        self.assertEqual( "FFFF", "%X" % numVariant(0xFFFF) )
        self.assertEqual( "-70000000000", "%d" % numVariant(-70000000000) )
        self.assertEqual( "FFFFFFFFFFFFFF", "%X" % numVariant(0xFFFFFFFFFFFFFF) )
        self.assertEqual( "0", "%d" % numVariant(False) )

    def testFloatConvert(self):
        self.assertEqual( "1.0", "%1.1f" % float(numVariant(1.0)))
        self.assertEqual( "1", "%d" % int(numVariant(1.0)))
        self.assertEqual( "1.0", "%1.1f" % numVariant(1.0))
        self.assertEqual( "1", "%d" % numVariant(1.0))
        
    def testStr(self):
        self.assertEqual( "100", str(numVariant(100)) )

    def testHex(self):
        self.assertEqual( "0x64", hex(numVariant(100)) )


