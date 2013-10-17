
from IPython.core.magic import Magics, magics_class, line_magic, cell_magic, line_cell_magic
import pykd

@magics_class
class PykdMagic (Magics):

    @line_magic
    def kd(self,line):
        "any windbg command"
        try:
            pykd.dprintln( pykd.dbgCommand(line) )
        except pykd.BaseException:
            pykd.dprintln("invalid windbg syntax")
        return None

def load_ipython_extension(ipython):
    ipython.register_magics(PykdMagic)   

def unload_ipython_extension(ipython):
    pass 
        
        
