
from widget import *
from PySideKick.Console import QPythonConsole

class CmdWidget( DebugWidget ):

    def __init__(self, dbgCore, mainWindow, visible = False ):
        BaseWidget.__init__( self, dbgCore, mainWindow, "Commands", visible )

        self.addMenuTriggerAction( "Commands" )
        self.console = QPythonConsole()
        self.setWidget( self.console )
        self.console.interpreter.push("from pykd import *")



        






