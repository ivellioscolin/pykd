
import unittest
import target
import pykd

class CpuRegTest( unittest.TestCase ):
            
    def testCtor(self):
        if pykd.is64bitSystem():
            pykd.reg("rax")
        else:
            pykd.reg("eax")
             
        pykd.reg( 0 )
         
    def testFormat(self):
        self.assertEqual( "%d" % int(pykd.reg(0)), "%d" % pykd.reg(0) )
        self.assertEqual( "%x" % int(pykd.reg(0)), "%x" % pykd.reg(0) )
        
    def testGpr(self):
        if pykd.is64bitSystem():
            pykd.reg("rax")
            pykd.reg("rbx")
            pykd.reg("rcx")
            pykd.reg("rdx")
            pykd.reg("rdi")
            pykd.reg("rsi")
            pykd.reg("rbp")
            pykd.reg("rsp")
            pykd.reg("rip")
        else:
            pykd.reg("eax")
            pykd.reg("ebx")
            pykd.reg("ecx")
            pykd.reg("edx")
            pykd.reg("edi")
            pykd.reg("esi")
            pykd.reg("ebp")
            pykd.reg("esp")
            pykd.reg("eip")
    

    def testFloatRegister(self):
        "TODO: support float point regsiters"
        self.assertRaises( pykd.BaseException, pykd.reg, "st0" )

    def testMmxRegister(self):
        "TODO: support MMX regsiters"
        self.assertRaises( pykd.BaseException, pykd.reg, "mmx0" )

