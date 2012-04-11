
from commands.processcmd import ProcessController
from commands.dbgcmd import DebuggerController

from widgets.registers import RegisterWidget
from widgets.cmd import CmdWidget


class DefaultSettings:

    def __init__( self, dbgcore, parentwnd ):
        self.processCmd = ProcessController( dbgcore, parentwnd )
        self.debugCmd = DebuggerController( dbgcore, parentwnd )

        self.regWidget = RegisterWidget( dbgcore, parentwnd )
        self.cmdWidget = CmdWidget( dbgcore, parentwnd )

settings = { "default" : DefaultSettings }
