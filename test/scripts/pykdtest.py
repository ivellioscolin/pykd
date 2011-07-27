
import sys
import os
import unittest
import pykd
import target
import basetest
import typeinfo     


if __name__ == "__main__":

    targetAppPath = sys.argv[1]
    
    target.moduleName = os.path.splitext(os.path.basename(targetAppPath))[0]

    pykd.startProcess( targetAppPath )
    pykd.go()

    target.module = pykd.loadModule( target.moduleName )
   
    suite = unittest.TestSuite( [
          #  unittest.TestLoader().loadTestsFromTestCase( basetest.BaseTest ),
            unittest.TestLoader().loadTestsFromTestCase( typeinfo.TypeInfoTest ),
        ] )

    unittest.TextTestRunner().run( suite )


   





# Append current pykd.pyd path to PYTHONPATH
#sys.path.append(os.path.dirname(sys.argv[1]))
#import pykd

#class TestPykd(unittest.TestCase):

#    def setUp(self):
#        self.targetAppPath = sys.argv[1]
#        self.targetModuleName = os.path.splitext(os.path.basename(self.targetAppPath))[0]
#        self.targetModule = None
#        self.gVerInfoOffset = 0L
#        self.verInfo = None

#    def testPykd(self):
#        print self.targetAppPath
        
#        self.assertTrue(pykd.startProcess(self.targetAppPath))

#        self.targetModule = pykd.loadModule(self.targetModuleName)
#        self.assertNotEqual(self.targetModule, None)
#        pykd.go()
        
#        self.gVerInfoOffset = pykd.getOffset(self.targetModuleName, "gVerInfo")
#        self.assertNotEqual(self.gVerInfoOffset, 0L)

#        self.verInfo = pykd.typedVar(self.targetModuleName, "_OSVERSIONINFOA", self.gVerInfoOffset)
#        self.assertNotEqual(self.verInfo, None)
#        pykd.go()
        

#if __name__ == '__main__':
    #unittest.main(argv=[])
#   suite = unittest.TestLoader().loadTestsFromTestCase(TestPykd)
#    unittest.TextTestRunner().run(suite)
