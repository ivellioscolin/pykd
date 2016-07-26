#
#
#

import unittest
import target
import pykd
import math

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
        self.assertEqual( 0, len( [ ucharArray[i] for i in range(5) if ucharArray[i] != testArray[i] ] ) )

    def testLoadWords( self ):
        loadArray = pykd.loadWords( target.module.ushortArray, 5 )
        testArray = [ 0, 10, 0xFF, 0x8000, 0xFFFF  ]
        self.assertEqual( len(testArray), len(loadArray) )
        self.assertEqual( 0, len( [ loadArray[i] for i in range(len(testArray)) if loadArray[i] != testArray[i] ] ) )
        
    def testLoadDWords( self ):
        loadArray = pykd.loadDWords( target.module.ulongArray, 5 )
        testArray = [ 0, 0xFF, 0x8000, 0x80000000, 0xFFFFFFFF ]
        self.assertEqual( len(testArray), len(loadArray) )
        self.assertEqual( 0, len( [ loadArray[i] for i in range(len(testArray)) if loadArray[i] != testArray[i] ] ) )
        
    def testLoadQWords( self ):
        loadArray = pykd.loadQWords( target.module.ulonglongArray, 5 )
        testArray = [ 0, 0xFF, 0xFFFFFFFF, 0x8000000000000000, 0xFFFFFFFFFFFFFFFF ]
        self.assertEqual( len(testArray), len(loadArray) )
        self.assertEqual( 0, len( [ loadArray[i] for i in range(len(testArray)) if loadArray[i] != testArray[i] ] ) )
        
    def testLoadSignBytes( self ):
        charArray = pykd.loadSignBytes( target.module.ucharArray, 5 )
        testArray = [ 0, 10, 0x78, -128, -1 ]
        self.assertEqual( 5, len(charArray) )
        self.assertEqual( 0, len( [ charArray[i] for i in range(len(testArray)) if charArray[i] != testArray[i] ] ) )
    
    def testLoadSignWords( self ):
        loadArray = pykd.loadSignWords( target.module.ushortArray, 5 )
        testArray = [ 0, 10, 255, -32768, -1 ]
        self.assertEqual( len(testArray), len(loadArray) )
        self.assertEqual( 0, len( [ loadArray[i] for i in range(len(testArray)) if loadArray[i] != testArray[i] ] ) )
        
    def testLoadSignDWords( self ):
        loadArray = pykd.loadSignDWords( target.module.ulongArray, 5 )
        testArray = [0, 255, 32768, -2147483648, -1]
        self.assertEqual( len(testArray), len(loadArray) )
        self.assertEqual( 0, len( [ loadArray[i] for i in range(len(testArray)) if loadArray[i] != testArray[i] ] ) )
        
    def testLoadSignQWords( self ):
        loadArray = pykd.loadSignQWords( target.module.ulonglongArray, 5 )
        testArray = [0, 255, 4294967295, -9223372036854775808, -1]
        self.assertEqual( len(testArray), len(loadArray) )
        self.assertEqual( 0, len( [ loadArray[i] for i in range(len(testArray)) if loadArray[i] != testArray[i] ] ) )

    def testWriteBytes( self ):
        testArray = pykd.loadBytes( target.module.ucharArray, 5 )
        pykd.writeBytes( target.module.ucharArrayPlace, testArray )
        ucharArray = pykd.loadBytes( target.module.ucharArrayPlace, 5 )
        self.assertEqual( 0, len( [ ucharArray[i] for i in range(5) if ucharArray[i] != testArray[i] ] ) )

    def testWriteWords( self ):
        testArray = pykd.loadWords( target.module.ushortArray, 5 )
        pykd.writeWords( target.module.ushortArrayPlace, testArray )
        ushortArray = pykd.loadWords( target.module.ushortArrayPlace, 5 )
        self.assertEqual( 0, len( [ ushortArray[i] for i in range(5) if ushortArray[i] != testArray[i] ] ) )

    def testWriteDWords( self ):
        testArray = pykd.loadDWords( target.module.ulongArray, 5 )
        pykd.writeDWords( target.module.ulongArrayPlace, testArray )
        ulongArray = pykd.loadDWords( target.module.ulongArrayPlace, 5 )
        self.assertEqual( 0, len( [ ulongArray[i] for i in range(5) if ulongArray[i] != testArray[i] ] ) )

    def testWriteQWords( self ):
        testArray = pykd.loadQWords( target.module.ulonglongArray, 5 )
        pykd.writeQWords( target.module.ulonglongArrayPlace, testArray )
        ulonglongArray = pykd.loadQWords( target.module.ulonglongArrayPlace, 5 )
        self.assertEqual( 0, len( [ ulonglongArray[i] for i in range(5) if ulonglongArray[i] != testArray[i] ] ) )

    def testWriteSignBytes( self ):
        testArray = pykd.loadSignBytes( target.module.charArray, 5 )
        pykd.writeSignBytes( target.module.charArrayPlace, testArray )
        charArray = pykd.loadSignBytes( target.module.charArrayPlace, 5 )
        self.assertEqual( 0, len( [ charArray[i] for i in range(5) if charArray[i] != testArray[i] ] ) )

    def testWriteSignWords( self ):
        testArray = pykd.loadSignWords( target.module.shortArray, 5 )
        pykd.writeSignWords( target.module.shortArrayPlace, testArray )
        shortArray = pykd.loadSignWords( target.module.shortArrayPlace, 5 )
        self.assertEqual( 0, len( [ shortArray[i] for i in range(5) if shortArray[i] != testArray[i] ] ) )

    def testWriteSignDWords( self ):
        testArray = pykd.loadSignDWords( target.module.longArray, 5 )
        pykd.writeSignDWords( target.module.longArrayPlace, testArray )
        longArray = pykd.loadSignDWords( target.module.longArrayPlace, 5 )
        self.assertEqual( 0, len( [ longArray[i] for i in range(5) if longArray[i] != testArray[i] ] ) )

    def testWriteSignQWords( self ):
        testArray = pykd.loadSignQWords( target.module.longlongArray, 5 )
        pykd.writeSignQWords( target.module.longlongArrayPlace, testArray )
        longlongArray = pykd.loadSignQWords( target.module.longlongArrayPlace, 5 )
        self.assertEqual( 0, len( [ longlongArray[i] for i in range(5) if longlongArray[i] != testArray[i] ] ) )

    def testWriteFloats( self ):
        testArray = pykd.loadFloats( target.module.floatArray, 5 )
        pykd.writeFloats( target.module.floatArrayPlace, testArray )
        floatArray = pykd.loadFloats( target.module.floatArrayPlace, 5 )
        self.assertEqual( 0, len( [ floatArray[i] for i in range(5) if floatArray[i] != testArray[i] ] ) )

    def testWriteDoubles( self ):
        testArray = pykd.loadDoubles( target.module.doubleArray, 5 )
        pykd.writeDoubles( target.module.doubleArrayPlace, testArray )
        doubleArray = pykd.loadDoubles( target.module.doubleArrayPlace, 5 )
        self.assertEqual( 0, len( [ doubleArray[i] for i in range(5) if doubleArray[i] != testArray[i] ] ) )

    def testPtrRead( self ):
        self.assertEqual( 0x80, pykd.ptrByte( target.module.bigValue ) )
        self.assertEqual( 0x8080, pykd.ptrWord( target.module.bigValue ) )
        self.assertEqual( 0x80808080, pykd.ptrDWord( target.module.bigValue ) )
        self.assertEqual( 0x8080808080808080, pykd.ptrQWord( target.module.bigValue ) )
        self.assertEqual( -128, pykd.ptrSignByte( target.module.bigValue ) )
        self.assertEqual( -32640, pykd.ptrSignWord( target.module.bigValue ) )
        self.assertEqual( -2139062144, pykd.ptrSignDWord( target.module.bigValue ) )
        self.assertEqual( -9187201950435737472, pykd.ptrSignQWord( target.module.bigValue ) )

    def testSetValue( self ):
        pykd.setByte( target.module.ullValuePlace, pykd.ptrByte( target.module.bigValue ) )
        self.assertEqual( pykd.ptrByte( target.module.bigValue ), pykd.ptrByte( target.module.ullValuePlace ) )

        pykd.setWord( target.module.ullValuePlace, pykd.ptrWord( target.module.bigValue ) )
        self.assertEqual( pykd.ptrWord( target.module.bigValue ), pykd.ptrWord( target.module.ullValuePlace ) )

        pykd.setDWord( target.module.ullValuePlace, pykd.ptrDWord( target.module.bigValue ) )
        self.assertEqual( pykd.ptrDWord( target.module.bigValue ), pykd.ptrDWord( target.module.ullValuePlace ) )

        pykd.setQWord( target.module.ullValuePlace, pykd.ptrQWord( target.module.bigValue ) )
        self.assertEqual( pykd.ptrQWord( target.module.bigValue ), pykd.ptrQWord( target.module.ullValuePlace ) )

        pykd.setSignByte( target.module.ullValuePlace, -128 )
        self.assertEqual( -128, pykd.ptrSignByte( target.module.ullValuePlace ) )

        pykd.setSignWord( target.module.ullValuePlace, pykd.ptrSignWord( target.module.bigValue ) )
        self.assertEqual( pykd.ptrSignWord( target.module.bigValue ), pykd.ptrSignWord( target.module.ullValuePlace ) )

        pykd.setSignDWord( target.module.ullValuePlace, pykd.ptrSignDWord( target.module.bigValue ) )
        self.assertEqual( pykd.ptrSignDWord( target.module.bigValue ), pykd.ptrSignDWord( target.module.ullValuePlace ) )

        pykd.setSignQWord( target.module.ullValuePlace, pykd.ptrSignQWord( target.module.bigValue ) )
        self.assertEqual( pykd.ptrSignQWord( target.module.bigValue ), pykd.ptrSignQWord( target.module.ullValuePlace ) )

        pykd.setFloat( target.module.floatValuePlace, pykd.ptrFloat( target.module.floatValue ) )
        self.assertEqual( pykd.ptrFloat( target.module.floatValue ), pykd.ptrFloat( target.module.floatValuePlace ) )

        pykd.setDouble( target.module.doubleValuePlace, pykd.ptrDouble( target.module.doubleValue ) )
        self.assertEqual( pykd.ptrDouble( target.module.doubleValue ), pykd.ptrDouble( target.module.doubleValuePlace ) )

    def testCompare( self ):
        self.assertTrue( pykd.compareMemory( target.module.helloStr, pykd.ptrPtr(target.module.strArray), 5 ) )
        self.assertFalse( pykd.compareMemory( target.module.helloStr, target.module.helloWStr, 5 ) )
        
    def testCStr( self ):        
        self.assertEqual( 'Hello', pykd.loadCStr( target.module.helloStr ) )
        self.assertEqual( u'Hello', pykd.loadWStr( target.module.helloWStr ) )
        
    def testBigCStr( self ):        
        self.assertEqual( 0x2000, len( pykd.loadCStr( target.module.bigCStr ) ) )
        self.assertEqual( 0x2000, len( pykd.loadWStr( target.module.bigWStr ) ) )
        
    def testVaValid( self ):
        self.assertTrue( pykd.isValid( target.module.begin() ) )
        self.assertFalse( pykd.isValid( 0 ) )
        self.assertFalse( pykd.isValid( 0xDEADBEAF ) )
        
    def testPtrList( self ):
        lst = pykd.loadPtrList( target.module.g_listHead )
        self.assertEqual( 5, len( lst ) )
        
    def testPtrArray( self ):
        lst = pykd.loadPtrs( target.module.arrIntMatrixPtrs, 3 )
        self.assertEqual( 3, len( lst ) )
        
    def testInvalidAddr( self ):
        try:
            pykd.loadSignBytes( 0xDEADBEEF, 5 )
        except pykd.MemoryException:
            self.assertTrue( True )
            
    def testPtrFloat(self):
        self.assertTrue( math.fabs( pykd.ptrFloat( target.module.floatVar) + 5.99 ) < 0.001 )
        self.assertTrue( math.fabs( pykd.ptrDouble( target.module.doubleVar) - 6.00000001 ) < 0.0000001 )
        
    def testLoadFloats(self):
       testArray = [  1.0, 0.001, -199.999, 20000.01, 0.111111 ];
       readArray = pykd.loadFloats( target.module.floatArray, 5 );
       for i in range(5):
           self.assertTrue( math.fabs( testArray[i] - readArray[i]  ) < 0.001 )
           
    def testLoadDoubles(self):
       testArray = [ 1.0000000, 0.0000000001, -199.99999999998, 200000.00000001, 0.3333333333 ];
       readArray = pykd.loadDoubles( target.module.doubleArray, 5 );
       for i in range(5):
           self.assertTrue( math.fabs( testArray[i] - readArray[i]  ) < 0.0000001 )
