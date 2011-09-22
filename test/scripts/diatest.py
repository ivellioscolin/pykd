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

  def testSize(self):
    globalScope = pykd.diaOpenPdb( str(target.module.pdb()) )
    self.assertEqual(1, globalScope["g_ucharValue"].type().size())
    self.assertEqual(2, globalScope["g_ushortValue"].type().size())
    self.assertEqual(4, globalScope["g_ulongValue"].type().size())
    self.assertEqual(8, globalScope["g_ulonglongValue"].type().size())

  def testValue(self):
    globalScope = pykd.diaOpenPdb( str(target.module.pdb()) )
    self.assertEqual(0x5555, globalScope["g_constNumValue"].value())
    self.assertEqual(True, globalScope["g_constBoolValue"].value())

  def testName(self):
    globalScope = pykd.diaOpenPdb( str(target.module.pdb()) )
    self.assertEqual("g_constNumValue", globalScope["g_constNumValue"].name())
    self.assertEqual("FuncWithName0", globalScope["FuncWithName0"].name())

  def testRva(self):
    globalScope = pykd.diaOpenPdb( str(target.module.pdb()) )
    _rva = globalScope["FuncWithName0"].rva()
    self.assertNotEqual(0, _rva)
    self.assertTrue( _rva < (target.module.end() - target.module.begin()) )
    _rva = globalScope["g_string"].rva()
    self.assertNotEqual(0, _rva)
    self.assertTrue( _rva < (target.module.end() - target.module.begin()) )

  def testSymTag(self):
    globalScope = pykd.diaOpenPdb( str(target.module.pdb()) )
    self.assertEqual(pykd.SymTagFunction, globalScope["FuncWithName0"].symTag())
    self.assertEqual(pykd.SymTagData, globalScope["g_string"].symTag())

  def testLocType(self):
    globalScope = pykd.diaOpenPdb( str(target.module.pdb()) )
    self.assertEqual(pykd.LocIsConstant, globalScope["g_constNumValue"].locType())
    self.assertEqual(pykd.LocIsStatic, globalScope["FuncWithName1"].locType())

  def testBasicType(self):
    globalScope = pykd.diaOpenPdb( str(target.module.pdb()) )
    self.assertFalse(globalScope["g_string"].type().isBasic())
    self.assertEqual(pykd.btBool, globalScope["g_constBoolValue"].type().baseType())
    self.assertEqual(pykd.btULong, globalScope["g_ulongValue"].type().baseType())

  def testBits(self):
    globalScope = pykd.diaOpenPdb( str(target.module.pdb()) )
    structWithBits = globalScope["structWithBits"]
    bitField = structWithBits["m_bit0_4"]
    self.assertEqual(pykd.LocIsBitField, bitField.locType())
    self.assertEqual(0, bitField.bitPos())
    self.assertEqual(5, bitField.size())
    bitField = structWithBits["m_bit5"]
    self.assertEqual(pykd.LocIsBitField, bitField.locType())
    self.assertEqual(5, bitField.bitPos())
    self.assertEqual(1, bitField.size())
    bitField = structWithBits["m_bit6_7"]
    self.assertEqual(pykd.LocIsBitField, bitField.locType())
    self.assertEqual(6, bitField.bitPos())
    self.assertEqual(2, bitField.size())
