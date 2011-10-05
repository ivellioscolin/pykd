
import regs
import disasm
import stack
import cmd


def register( mainFrame ):
    cmd.CmdWidget( mainFrame )
    regs.RegistersWidget( mainFrame )
    disasm.DisasmWidget( mainFrame )
    stack.StackWidget( mainFrame )
  