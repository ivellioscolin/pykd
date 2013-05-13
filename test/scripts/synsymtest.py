"""Synthetic symbols tests"""

import unittest
import target
import pykd

class SynSymTest(unittest.TestCase):
    """Unit tests of synthetic symbols"""

    def testAdd(self):
        """Add new synthetic symbol"""
        pykd.addSynSymbol(
            target.module.offset("FuncWithName0")-1, 1, "synSym1")
        self.assertEqual(
            target.module.offset("FuncWithName0")-1,
            target.module.offset("synSym1"))

    def testDel(self):
        """Remove synthetic symbol"""
        pykd.addSynSymbol(
            target.module.offset("FuncWithName0")-2, 1, "synSym2")
        self.assertEqual(
            pykd.delSynSymbol( target.module.offset("synSym2") ), 1 )

        exceptionOccurred = True
        try:
            target.module.rva("synSym2")
            exceptionOccurred = False
        except pykd.BaseException:
            pass
        self.assertTrue(exceptionOccurred)

    def testDelAll(self):
        """Remove all synthetic symbols"""
        pykd.addSynSymbol(
            target.module.offset("FuncWithName0")-3, 1, "synSym3")
        pykd.delAllSynSymbols()

        exceptionOccurred = True
        try:
            target.module.rva("synSym3")
            exceptionOccurred = False
        except pykd.BaseException:
            pass
        self.assertTrue(exceptionOccurred)

    def testDelByMask(self):
        """Remove synthetic symbol by mask"""
        pykd.addSynSymbol(
            target.module.offset("FuncWithName0")-4, 1, "synSym4")
        self.assertTrue( pykd.delSynSymbolsMask( "*", "synSym4" ) >= 1 )

        exceptionOccurred = True
        try:
            target.module.rva("synSym4")
            exceptionOccurred = False
        except pykd.BaseException:
            pass
        self.assertTrue(exceptionOccurred)

    def testReload(self):
        """Restore synthetic symbols after reload module symbols"""
        pykd.addSynSymbol(
            target.module.offset("FuncWithName0")-5, 1, "synSym5")
        target.module.reload()
        self.assertEqual(
            target.module.offset("FuncWithName0")-5,
            target.module.offset("synSym5"))

    def testAddSynSymbolException(self):
        """Test of AddSynSymbolException"""
        pykd.addSynSymbol(
            target.module.offset("FuncWithName0")-6, 1, "synSym6")

        exceptionOccurred = False
        try:
            pykd.addSynSymbol(
                target.module.offset("FuncWithName0")-6, 1, "synSym7")
        except pykd.AddSynSymbolException:
            exceptionOccurred = True
        self.assertTrue(exceptionOccurred)
