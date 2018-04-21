
import unittest
import target
import pykd

class CpuRegTest( unittest.TestCase ):

    def testGetRegName(self):
        self.assertNotEqual(None, pykd.getRegisterName(10))

    def testGetRegValue(self):
        for regIndex in range(pykd.getNumberRegisters()):
            regName = pykd.getRegisterName(regIndex)
            try:
                self.assertEqual( pykd.reg(regIndex), pykd.reg(regName) )
            except pykd.DbgException:
                pass  # pass exception unsupported register type

    def testSetRegValue(self):

         oldVal = pykd.reg(2)
         pykd.setReg(2, 10)
         self.assertEqual(pykd.reg(2), 10)
         pykd.setReg( pykd.getRegisterName(2), oldVal )
         self.assertEqual(pykd.reg(2), oldVal )



    #def testCtor(self):
    #    currentcpu = pykd.cpu()
    #    cpu0 = pykd.cpu(0)

    #def testIp(self):
    #    currentcpu = pykd.cpu()
    #    self.assertNotEqual( 0, currentcpu.ip )
    #    self.assertNotEqual( 0, currentcpu.sp )
    #    self.assertNotEqual( 0, currentcpu.fp )

    #def testRegEnum(self):
    #    for r in pykd.cpu():
    #        pass



