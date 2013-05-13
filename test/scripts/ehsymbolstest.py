"""Execution symbols state event test"""

import unittest
import target
import pykd
import testutils

class SymbolsStateHandler(pykd.eventHandler):
    def __init__(self, modBase):
        pykd.eventHandler.__init__(self)
        self._modBase = modBase
        self.modNames = set()
        self.unloadModuleTrigged = False
        self.unloadAllModulesTrigged = False

    def onSymbolsLoaded(self, modBase):
        if modBase:
            self.modNames.add( pykd.module(modBase).name() )

    def onSymbolsUnloaded(self, modBase):
        if not modBase:
            self.unloadAllModulesTrigged = True
        elif self._modBase == modBase:
            self.unloadModuleTrigged = True

class EhSymbolsTest(unittest.TestCase):
    """Execution symbols state event test"""

    def testChangeSymbolsState(self):
        """Start new process and track change symbols exception"""
        _locProcessId = pykd.startProcess( target.appPath + " -testLoadUnload" )
        with testutils.ContextCallIt( testutils.KillProcess(_locProcessId) ) as killStartedProcess:

            mod = pykd.module("targetapp")
            symbolsStateHandler = SymbolsStateHandler( mod.begin() )

            pykd.dbgCommand(".reload /u targetapp.exe")
            self.assertTrue( symbolsStateHandler.unloadModuleTrigged )

            pykd.dbgCommand(".reload /u")
            self.assertTrue( symbolsStateHandler.unloadAllModulesTrigged )

            self.assertRaises(pykd.WaitEventException, testutils.infGo)

            self.assertTrue( "iphlpapi" in symbolsStateHandler.modNames )
