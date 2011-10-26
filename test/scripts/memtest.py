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
        testArray = [ 0, 10, 0xFF, 0xFFF, 0xFFFF  ]
        self.assertEqual( len(testArray), len(loadArray) )
        self.assertEqual( 0, len( [ loadArray[i] for i in xrange(len(testArray)) if loadArray[i] != testArray[i] ] ) )
        
    def testLoadDWords( self ):
        loadArray = pykd.loadDWords( target.module.ulongArray, 5 )
        testArray = [ 0, 0xFF, 0xFFF, 0xFFFF, 0xFFFFFFFF ]
        self.assertEqual( len(testArray), len(loadArray) )
        self.assertEqual( 0, len( [ loadArray[i] for i in xrange(len(testArray)) if loadArray[i] != testArray[i] ] ) )
        
    def testLoadQWords( self ):
        loadArray = pykd.loadQWords( target.module.ulonglongArray, 5 )
        testArray = [ 0, 0xFF, 0xFFF, 0x8000000000000000, 0xFFFFFFFFFFFFFFFF ]
        self.assertEqual( len(testArray), len(loadArray) )
        self.assertEqual( 0, len( [ loadArray[i] for i in xrange(len(testArray)) if loadArray[i] != testArray[i] ] ) )        