"""
Switch x86(WOW64) <-> x64 processor mode
And execute commands (if specified)
"""

import sys
from pykd import *

if __name__ == "__main__":
  if getProcessorType() == "X64":
    setProcessorMode( {"X64": "X86", "X86": "X64"}[ getProcessorMode() ] )
    if len(sys.argv) > 1:
      command = ""
      for arg in sys.argv[1:]:
        command = command + " " + arg
      dprintln( dbgCommand(command) )
  else:
    dprintln("For \"X64\" processor only")
