"""Debug events handler: test [un-]load modules notification"""

import unittest
import target
import pykd
import fnmatch

class ModuleLoadHandler(pykd.eventHandler):
    """Track load/unload module implementation"""
    def __init__(self, client, moduleMask):
        pykd.eventHandler.__init__(self, client)

        self.moduleMask = moduleMask.lower()

        self.wasLoad = 0
        self.wasUnload = False

    def onLoadModule(self, module):
        """Load module handler"""

        if ( fnmatch.fnmatch(module.name().lower(), self.moduleMask) ):
            self.wasLoad = module.begin()

        return pykd.DEBUG_STATUS_NO_CHANGE

    def onUnloadModule(self, modBase):
        """Unload module handler"""

        if ( self.wasLoad and (self.wasLoad == modBase) ):
            self.wasUnload = True

        return pykd.DEBUG_STATUS_NO_CHANGE

class EhLoadTest(unittest.TestCase):
    """Unit tests of [un-]load modules notification"""

    def testLoadUnload(self):
        """Start new process and track loading and unloading modules"""
        testClient = pykd.createDbgClient()
        testClient.startProcess( target.appPath + " -testLoadUnload" )

        modLoadHandler = ModuleLoadHandler( testClient, "*Iphlpapi*" )
        try:
            while True:
                testClient.go()
        except pykd.WaitEventException:
            pass

        self.assertTrue(modLoadHandler.wasLoad)
        self.assertTrue(modLoadHandler.wasUnload)

