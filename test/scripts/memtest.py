#
#
#

import unittest
import target
import pykd

class MemoryTest( unittest.TestCase ):

    def testLoadChars( self ):
        s = pykd.loadChars( target.module.helloStr, 5 )
        self.assertEqual( "Hello", s )
        
    def testLoadWChars( self ):
        s = pykd.loadWChars( target.module.helloWStr, 5 )
        self.assertEqual( "Hello", s )
        
    def testLoadBytes( self ):
        ucharArray = pykd.loadBytes( target.module.ucharArray, 5 )
        testArray = [ 0, 10, 0x78, 128, 0xFF ]
        self.assertEqual( 5, len(ucharArray) )
        self.assertEqual( 0, len( [ ucharArray[i] for i in xrange(5) if ucharArray[i] != testArray[i] ] ) )

    def testLoadWords( self ):
        loadArray = pykd.loadWords( target.module.ushortArray, 5 )
        testArray = [ 0, 10, 0xFF, 0x8000, 0xFFFF  ]
        self.assertEqual( len(testArray), len(loadArray) )
        self.assertEqual( 0, len( [ loadArray[i] for i in xrange(len(testArray)) if loadArray[i] != testArray[i] ] ) )
        
    def testLoadDWords( self ):
        loadArray = pykd.loadDWords( target.module.ulongArray, 5 )
        testArray = [ 0, 0xFF, 0x8000, 0x80000000, 0xFFFFFFFF ]
        self.assertEqual( len(testArray), len(loadArray) )
        self.assertEqual( 0, len( [ loadArray[i] for i in xrange(len(testArray)) if loadArray[i] != testArray[i] ] ) )
        
    def testLoadQWords( self ):
        loadArray = pykd.loadQWords( target.module.ulonglongArray, 5 )
        testArray = [ 0, 0xFF, 0xFFFFFFFF, 0x8000000000000000, 0xFFFFFFFFFFFFFFFF ]
        self.assertEqual( len(testArray), len(loadArray) )
        self.assertEqual( 0, len( [ loadArray[i] for i in xrange(len(testArray)) if loadArray[i] != testArray[i] ] ) )
        
    def testLoadSignBytes( self ):
        charArray = pykd.loadSignBytes( target.module.ucharArray, 5 )
        testArray = [ 0, 10, 0x78, -128, -1 ]
        self.assertEqual( 5, len(charArray) )
        self.assertEqual( 0, len( [ charArray[i] for i in xrange(len(testArray)) if charArray[i] != testArray[i] ] ) )    
    
    def testLoadSignWords( self ):
        loadArray = pykd.loadSignWords( target.module.ushortArray, 5 )
        testArray = [ 0, 10, 255, -32768, -1 ]
        self.assertEqual( len(testArray), len(loadArray) )
        self.assertEqual( 0, len( [ loadArray[i] for i in xrange(len(testArray)) if loadArray[i] != testArray[i] ] ) )
        
    def testLoadSignDWords( self ):
        loadArray = pykd.loadSignDWords( target.module.ulongArray, 5 )
        testArray = [0, 255, 32768, -2147483648, -1]
        self.assertEqual( len(testArray), len(loadArray) )
        self.assertEqual( 0, len( [ loadArray[i] for i in xrange(len(testArray)) if loadArray[i] != testArray[i] ] ) )
        
    def testLoadSignQWords( self ):
        loadArray = pykd.loadSignQWords( target.module.ulonglongArray, 5 )
        testArray = [0, 255, 4294967295L, -9223372036854775808L, -1]
        self.assertEqual( len(testArray), len(loadArray) )
        self.assertEqual( 0, len( [ loadArray[i] for i in xrange(len(testArray)) if loadArray[i] != testArray[i] ] ) )

    def testPtrRead( self ):
        self.assertEqual( 0x80, pykd.ptrByte( target.module.g_bigValue ) )
        self.assertEqual( 0x8080, pykd.ptrWord( target.module.g_bigValue ) )
        self.assertEqual( 0x80808080, pykd.ptrDWord( target.module.g_bigValue ) )
        self.assertEqual( 0x8080808080808080, pykd.ptrQWord( target.module.g_bigValue ) )
        self.assertEqual( -128, pykd.ptrSignByte( target.module.g_bigValue ) )
        self.assertEqual( -32640, pykd.ptrSignWord( target.module.g_bigValue ) )
        self.assertEqual( -2139062144, pykd.ptrSignDWord( target.module.g_bigValue ) )
        self.assertEqual( -9187201950435737472, pykd.ptrSignQWord( target.module.g_bigValue ) )

    def testCompare( self ):
        self.assertTrue( pykd.compareMemory( target.module.helloStr, pykd.ptrPtr(target.module.strArray), 5 ) )
        self.assertFalse( pykd.compareMemory( target.module.helloStr, target.module.helloWStr, 5 ) )
