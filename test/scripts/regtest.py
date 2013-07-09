
import unittest
import target
import pykd

class CpuRegTest( unittest.TestCase ):
    
    def testCtor(self):
        currentcpu = pykd.cpu()
        cpu0 = pykd.cpu(0)

    def testIp(self):
        currentcpu = pykd.cpu()
        self.assertNotEqual( 0, currentcpu.ip )
        self.assertNotEqual( 0, currentcpu.sp )
        self.assertNotEqual( 0, currentcpu.fp )

    def testRegEnum(self):
        for r in pykd.cpu():
            pass
