"""Tests of thread context"""

import unittest
import target
import pykd

class ThreadContextTest( unittest.TestCase ):
    def testCurrentThreadContext( self ):
        """Some checks of current thread context content"""
        ctx = pykd.getContext()
#        for reg in ctx:
#            regName = ""
#            if ctx.processorType() == "X86":
#                regName = pykd.diaI386Regs[ reg[0] ]
#            else:
#                regName = pykd.diaAmd64Regs[ reg[0] ]
#            pykd.dprint( "\n" + regName + ": 0x%x " % reg[1])
        self.assertNotEqual( 0, len(ctx) )
        self.assertNotEqual( 0, ctx.ip() )
        self.assertNotEqual( 0, ctx.csp() )

    def testComplexRegisters( self ):
        """Test of "sub-"registers"""
        ctx = pykd.getContext()
        self.assertEqual( (ctx.get(pykd.CV_REG_AH) << 8) | ctx.get(pykd.CV_REG_AL), ctx.get(pykd.CV_REG_AX) )
        self.assertEqual( ctx.get(pykd.CV_REG_AX), ctx.get(pykd.CV_REG_EAX) & 0xffff )
        if ctx.processorType() == "X64":
            self.assertEqual( ctx.get(pykd.CV_REG_EAX), ctx.get(pykd.CV_AMD64_RAX) & 0xffffffff )

        self.assertEqual( (ctx.get(pykd.CV_REG_DH) << 8) | ctx.get(pykd.CV_REG_DL), ctx.get(pykd.CV_REG_DX) )
        self.assertEqual( ctx.get(pykd.CV_REG_DX), ctx.get(pykd.CV_REG_EDX) & 0xffff )
        if ctx.processorType() == "X64":
            self.assertEqual( ctx.get(pykd.CV_REG_EDX), ctx.get(pykd.CV_AMD64_RDX) & 0xffffffff )
