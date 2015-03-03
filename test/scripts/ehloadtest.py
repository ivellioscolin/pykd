"""Debug events handler: test [un-]load modules notification"""

import unittest
import target
import pykd
import fnmatch
import testutils

class ModuleLoadHandler(pykd.eventHandler):
    """Track load/unload module implementation"""
    def __init__(self, moduleMask):
        pykd.eventHandler.__init__(self)

        self.moduleMask = moduleMask.lower()

        self.wasLoad = 0
        self.wasUnload = False

    def onLoadModule(self, modBase, name):
        """Load module handler"""

        if ( fnmatch.fnmatch(name.lower(), self.moduleMask) ):
            self.wasLoad = modBase

        return pykd.executionStatus.NoChange

    def onUnloadModule(self, modBase, name):
        """Unload module handler"""

        if ( self.wasLoad and (self.wasLoad == modBase) ):
            self.wasUnload = True

        return pykd.executionStatus.NoChange

class EhLoadTest(unittest.TestCase):
    """Unit tests of [un-]load modules notification"""

    def testLoadUnload(self):
        """Start new process and track loading and unloading modules"""
        pykd.startProcess(target.appPath + " loadunloadmodule")
        with testutils.ContextCallIt( pykd.killProcess ) as contextCallIt:

            pykd.go() # skip initail break

            modLoadHandler = ModuleLoadHandler( "ws2_32*" )

            pykd.go()

            #with testutils.ContextCallIt( getattr(modLoadHandler, "reset") ) as resetEventHandler:
            #    try:
            #        while True:
            #            pykd.go()
            #    except pykd.WaitEventException:
            #        pass

            self.assertTrue(modLoadHandler.wasLoad)
            self.assertTrue(modLoadHandler.wasUnload)

