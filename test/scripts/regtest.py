
import unittest
import pykd
import target

class CpuRegTest( unittest.TestCase ):
  
    def testBasic(self):
        try:
            reg = pykd.cpuReg(0)
            self.assertTrue(True)
        except pykd.BaseException:
            pass     
            
                
    def testGPR(self):
    
        if pykd.is64bitSystem():
        
            rax = pykd.cpuReg("rax")
            self.assertEqual( rax, pykd.reg("rax") )
            
            rip = pykd.cpuReg("rip")
            self.assertEqual( rip, pykd.reg("rip") )            
        
        else:
        
            eax = pykd.cpuReg("eax")
            self.assertEqual( eax, pykd.reg("eax") )
        
            eip = pykd.cpuReg("eip")
            self.assertEqual( eip, pykd.reg("eip") )          
      
                             