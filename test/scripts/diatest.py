"""
Tests for pyDia
"""

import unittest
import target
import pykd

class DiaTest( unittest.TestCase ):
  def testFind(self):
    globalScope = pykd.diaOpenPdb( str(target.module.pdb()) )

    self.assertNotEqual(0, len(globalScope))

    symFunction = globalScope.find("FuncWithName0")
    self.assertTrue(1 == len( symFunction ))

    symFunction = globalScope.findEx(pykd.SymTagNull,
                                     "FuNc*Name?",
                                     pykd.nsCaseInRegularExpression)
    self.assertTrue(len(symFunction) > 1)


