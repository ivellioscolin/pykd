"""Synthetic symbols tests"""

import unittest
import target
import pykd

class SynSymTest(unittest.TestCase):
    """Unit tests of synthetic symbols"""

    def testAdd(self):
        """Add new synthetic symbol"""
        _addr = target.module.offset("FuncTestClass::staticMethod") - 1

        pykd.addSyntheticSymbol(_addr, 1, "synSym1")
        self.assertEqual(target.module.synSym1, _addr)

        self.assertRaises( pykd.DbgException, pykd.addSyntheticSymbol, _addr, 1, "synSym1" )

    def testDel(self):
        """Remove synthetic symbol"""
        _addr = target.module.offset("FuncTestClass::staticMethod") - 2

        _synsym = pykd.addSyntheticSymbol(_addr, 1, "synSym2")
        self.assertEqual(target.module.synSym2, _addr)

        pykd.removeSyntheticSymbol(_synsym)

        self.assertRaises( pykd.DbgException, target.module.offset, "synSym2" )

        self.assertRaises( pykd.DbgException, pykd.removeSyntheticSymbol, _synsym )

    def testModule(self):
        """Add and remove synthetic module"""
        base = 64 * 1024

        pykd.addSyntheticModule(base, 1024, "artificial_module1")
        pykd.addSyntheticModule(base + 1024, 1024, "artificial_module2", "artificial_module2_path")

        pykd.removeSyntheticSymbol( pykd.addSyntheticSymbol(base, 1, "artificial_symbol") )

        pykd.removeSyntheticModule(base + 1024)
        pykd.removeSyntheticModule(base)
