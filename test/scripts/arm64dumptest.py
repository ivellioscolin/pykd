import unittest
import pykd
import sys
import os

class ARM64DumpTest(unittest.TestCase):
    def setUp(self):
        dump_file = os.path.join( os.path.dirname(sys.argv[0]),
                                  r"..\..\kdlibcpp\kdlib\tests\dumps\win10_arm64_mem.cab" )
        self.dump_id = pykd.loadDump( dump_file )

    def tearDown(self):
        pykd.closeDump( self.dump_id )

    def testArm64(self):
        # dbgmem
        self.assertEqual( 0, pykd.addr64(0) )
        self.assertEqual( 0x80000000, pykd.addr64(0x80000000) )

        # dbgeng
        self.assertEqual( pykd.CPUType.ARM64, pykd.getCPUType() )
        self.assertEqual( pykd.CPUType.ARM64, pykd.getCPUMode() )

        self.assertTrue( pykd.is64bitSystem() )
        self.assertEqual( 8, pykd.ptrSize() )

        self.assertEqual( pykd.expr("@$retreg"), pykd.reg("x0") )

        # typeinfo
        self.assertEqual( 8, pykd.typeInfo("nt!_IRP*").size() )

        # symexport
        self.assertEqual( pykd.expr("clipsp!ClipSpInitialize"), pykd.module("clipsp").ClipSpInitialize )

        # cpu context
        cpu = pykd.cpu()
        self.assertEqual( pykd.expr("@$ip"), cpu.ip )
        self.assertEqual( pykd.expr("@$csp"), cpu.sp )
        self.assertEqual( pykd.expr("@fp"), cpu.fp )

        # stack
        stack = pykd.getStack()

        self.assertTrue( len(stack) > 11 )

        self.assertEqual( "nt!KeBugCheck2", pykd.findSymbol(stack[0].ip, False) )

        self.assertEqual( "Wdf01000!FxRequest::CompleteInternal", pykd.findSymbol(stack[11].ip, False) )
        self.assertEqual( 0, stack[11].getParam("Status") )
        self.assertEqual( 0xffffc18eaa798940, stack[11].getLocal("irp").m_Irp )




