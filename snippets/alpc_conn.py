"""
Print connection port for ALPC-port
"""

import sys
from pykd import *
import ntobj

def printConnPort(portAddr):
  """
  Print connection port by port address
  """
  port = typedVar("nt!_ALPC_PORT", portAddr)
  if (port != None):
    portCommInfo = typedVar("nt!_ALPC_COMMUNICATION_INFO", port.CommunicationInfo)
    dprintln( dbgCommand("!object %x" % portCommInfo.ConnectionPort) )
  else:
    dprintln("Error: query port object by address failed")

def main():
  """
  Print connection port for ALPC-port
  Usage: alpc_server <PORT_OBJ_ADDR>
    PORT_OBJ_ADDR - address of ALPC-port. If not specified: 
                    print all ALPC-ports for current process
  """
  argc_ = len(sys.argv)
  if (1 == argc_):
    portTypeAddr = getOffset("nt", "AlpcPortObjectType")
    if (0 != portTypeAddr):
      objTable = typedVar("nt!_EPROCESS", getCurrentProcess()).ObjectTable
      lstAlpcPorts = ntobj.getListByHandleTable(objTable, ptrPtr(portTypeAddr))
      for port in lstAlpcPorts:
        dprintln("Port object %x" % port + ", conection port:")
        printConnPort(port)
    else:
      dprintln("Error: symbol nt!AlpcPortObjectType not found")
  elif (2 == argc_):
    printConnPort(expr(sys.argv[1]))
  else:
    dprintln(main.__doc__)

if __name__ == "__main__":
  if not isWindbgExt():
    dprintln("Script is launch out of windbg")
    quit(0)

  if (False == isKernelDebugging()):
    dprintln("This script only for kernel debugging")
    quit(0)

  main()
  
