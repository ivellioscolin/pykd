import unittest
import pykd
import sys
import os

class ARMDumpTest(unittest.TestCase):
    def setUp(self):
        dump_file = os.path.join( os.path.dirname(sys.argv[0]),
                                  r"..\..\kdlibcpp\kdlib\tests\dumps\win10_arm_rpi3_mem\win10_arm_rpi3_mem.cab" )
        self.dump_id = pykd.loadDump( dump_file )

    def tearDown(self):
        pykd.closeDump( self.dump_id )

    def testArm(self):
        # dbgmem
        self.assertEqual( 0, pykd.addr64(0) )
        self.assertEqual( 0xffffffff80000000, pykd.addr64(0x80000000) )

        # dbgeng
        self.assertEqual( pykd.CPUType.ARM, pykd.getCPUType() )
        self.assertEqual( pykd.CPUType.ARM, pykd.getCPUMode() )

        self.assertEqual( False, pykd.is64bitSystem() )
        self.assertEqual( 4, pykd.ptrSize() )

        self.assertEqual( pykd.addr64(pykd.expr("@$retreg")), pykd.addr64(pykd.reg("r0")) )

        # typeinfo
        self.assertEqual( 4, pykd.typeInfo("nt!_IRP*").size() )

        # cpu context
        self.assertEqual( pykd.expr("@$csp"), pykd.cpu().sp )

        # stack
        stack = pykd.getStack()

        self.assertTrue( len(stack) > 1 )

        self.assertEqual( "Wdf01000!FxRequest::CompleteInternal", pykd.findSymbol(stack[0].ip, False) )

        self.assertEqual( "Wdf01000!imp_WdfRequestCompleteWithInformation", pykd.findSymbol(stack[1].ip, False) )
        self.assertEqual( 0x102, stack[1].getParam("RequestStatus") )
