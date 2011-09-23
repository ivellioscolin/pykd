"""
Tests for pyDia
"""

import unittest
import target
import pykd

class DiaTest( unittest.TestCase ):
  def testFind(self):
    gScope = pykd.diaLoadPdb( str(target.module.pdb()) )
    self.assertNotEqual(0, len(gScope))
    symFunction = gScope.find("FuncWithName0")
    self.assertTrue(1 == len( symFunction ))
    symFunction = gScope.findEx(pykd.SymTagNull,
                                "FuNc*Name?",
                                pykd.nsCaseInRegularExpression)
    self.assertTrue(len(symFunction) > 1)

  def testSize(self):
    gScope = pykd.diaLoadPdb( str(target.module.pdb()) )
    self.assertEqual(1, gScope["g_ucharValue"].type().size())
    self.assertEqual(2, gScope["g_ushortValue"].type().size())
    self.assertEqual(4, gScope["g_ulongValue"].type().size())
    self.assertEqual(8, gScope["g_ulonglongValue"].type().size())

  def testValue(self):
    gScope = pykd.diaLoadPdb( str(target.module.pdb()) )
    self.assertEqual(0x5555, gScope["g_constNumValue"].value())
    self.assertEqual(True, gScope["g_constBoolValue"].value())

  def testName(self):
    gScope = pykd.diaLoadPdb( str(target.module.pdb()) )
    self.assertEqual("g_constNumValue", gScope["g_constNumValue"].name())
    self.assertEqual("FuncWithName0", gScope["FuncWithName0"].name())

  def testRva(self):
    gScope = pykd.diaLoadPdb( str(target.module.pdb()) )
    _rva = gScope["FuncWithName0"].rva()
    self.assertNotEqual(0, _rva)
    self.assertTrue( _rva < (target.module.end() - target.module.begin()) )
    _rva = gScope["g_string"].rva()
    self.assertNotEqual(0, _rva)
    self.assertTrue( _rva < (target.module.end() - target.module.begin()) )

  def testSymTag(self):
    gScope = pykd.diaLoadPdb( str(target.module.pdb()) )
    self.assertEqual(pykd.SymTagFunction, gScope["FuncWithName0"].symTag())
    self.assertEqual(pykd.SymTagData, gScope["g_string"].symTag())

  def testLocType(self):
    gScope = pykd.diaLoadPdb( str(target.module.pdb()) )
    self.assertEqual(pykd.LocIsConstant, gScope["g_constNumValue"].locType())
    self.assertEqual(pykd.LocIsStatic, gScope["FuncWithName1"].locType())

  def testBasicType(self):
    gScope = pykd.diaLoadPdb( str(target.module.pdb()) )
    self.assertFalse(gScope["g_string"].type().isBasic())
    self.assertEqual(pykd.btBool, gScope["g_constBoolValue"].type().baseType())
    self.assertEqual(pykd.btULong, gScope["g_ulongValue"].type().baseType())

  def testBits(self):
    gScope = pykd.diaLoadPdb( str(target.module.pdb()) )
    structWithBits = gScope["structWithBits"]
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

  def testIndexId(self):
    gScope = pykd.diaLoadPdb( str(target.module.pdb()) )
    self.assertNotEqual( gScope["classChild"].indexId(), 
                         gScope["classBase"].indexId() )
    self.assertNotEqual( gScope["FuncWithName0"].indexId(), 
                         gScope["FuncWithName1"].indexId() )

  def testUdtKind(self):
    gScope = pykd.diaLoadPdb( str(target.module.pdb()) )
    self.assertEqual(pykd.UdtStruct, gScope["structWithBits"].udtKind())
    self.assertEqual(pykd.UdtUnion, gScope["unionTest"].udtKind())
    self.assertEqual(pykd.UdtClass, gScope["classBase"].udtKind())

  def testOffset(self):
    gScope = pykd.diaLoadPdb( str(target.module.pdb()) )
    structTest = gScope["structTest"]
    self.assertEqual( 0, structTest["m_field0"].offset() )
    self.assertTrue( structTest["m_field0"].offset() < 
                     structTest["m_field1"].offset() )
    self.assertTrue( structTest["m_field1"].offset() < 
                     structTest["m_field2"].offset() )
    self.assertTrue( structTest["m_field2"].offset() < 
                     structTest["m_field3"].offset() )
    self.assertTrue(structTest["m_field3"].offset() < structTest.size())

  def testMachine(self):
    gScope = pykd.diaLoadPdb( str(target.module.pdb()) )
    self.assertTrue( (gScope.machineType() == pykd.IMAGE_FILE_MACHINE_I386) or
                     (gScope.machineType() == pykd.IMAGE_FILE_MACHINE_AMD64) )

  def testFindByRva(self):
    gScope = pykd.diaLoadPdb( str(target.module.pdb()) )
    func = gScope["FuncWithName0"]

    tplSymOffset = gScope.findByRva(func.rva(), pykd.SymTagFunction)
    self.assertEqual(tplSymOffset[0].indexId(), func.indexId())
    self.assertEqual(tplSymOffset[1], 0)

    tplSymOffset = gScope.findByRva(func.rva() + 2, pykd.SymTagFunction)
    self.assertEqual(tplSymOffset[0].indexId(), func.indexId())
    self.assertEqual(tplSymOffset[1], 2)

  def testSymbolById(self):
    gScope = pykd.diaLoadPdb( str(target.module.pdb()) )
    func = gScope["FuncWithName0"]
    self.assertEqual( gScope.symbolById(func.indexId()).indexId(),
                      func.indexId())
