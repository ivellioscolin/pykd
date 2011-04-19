"""
Print nt!_PORT_MESSAGE header and message dump
"""

import sys
from pykd import *

# LPC message type
LpcMessageType = {
  1   : "LPC_REQUEST",
  2   : "LPC_REPLY",
  3   : "LPC_DATAGRAM",
  4   : "LPC_LOST_REPLY",
  5   : "LPC_PORT_CLOSED",
  6   : "LPC_CLIENT_DIED",
  7   : "LPC_EXCEPTION",
  8   : "LPC_DEBUG_EVENT",
  9   : "LPC_ERROR_EVENT",
  10  : "LPC_CONNECTION_REQUEST"
}

# diplay format type
DispFormatsLength = {
  "a" : 1, "b" : 1, "yb": 1,
  "w" : 2, "W" : 2, "u" : 2,
  "d" : 4, "dc": 4, "yd": 4, "f" : 4,
  "q" : 8, "D" : 8,
  "p" : ptrSize(), "ps": ptrSize()
}


def PrintPortMesage(messageAddr, printFormat="b", use32=False, moduleName="nt"):
  """
  Print _PORT_MESSAGE header and dump of message dump

  Usage: portmsg messageAddr, printFormat[=b], use32[=False], moduleName[=nt]
  When:
    messageAddr - address of port message
    printFormat - string of display format ("d+printFormat"). 
                  May be: a, b, yb, w, W, u, d, dc, yd, f, q, D, p, ps
                  Display formats overview:
    <link cmd=\".shell -x rundll32 url.dll,FileProtocolHandler http://msdn.microsoft.com/en-us/library/ff542790(VS.85).aspx\">http://msdn.microsoft.com/en-us/library/ff542790(VS.85).aspx</link>

    use32       - use _PORT_MESSAGE32 (instead of _PORT_MESSAGE) structure (True or False)
    moduleName  - module name with _PORT_MESSAGE structure in symbols
  """

  messageTypeName = "_PORT_MESSAGE"
  if (use32):
    messageTypeName = "_PORT_MESSAGE32"

  messageHeader = typedVar(moduleName, messageTypeName, messageAddr)
  if (None == messageHeader):
    dprintln("ERROR: Getting (" + moduleName + "!" + messageTypeName + " *)(0x%x) failed" % messageAddr )
    return

  dprintln( "Data length      : %3d (0x%02x)" % (messageHeader.u1.s1.DataLength, messageHeader.u1.s1.DataLength) )
  dprintln( "Total length     : %3d (0x%02x)" % (messageHeader.u1.s1.TotalLength, messageHeader.u1.s1.TotalLength) )
  calcHeaderLen = messageHeader.u1.s1.TotalLength - messageHeader.u1.s1.DataLength
  headerLen = sizeof(moduleName, messageTypeName)
  if (calcHeaderLen != headerLen):
    dprintln( "WARRING: calculated size (%2d (0x%02x)) of LPC-header does not match with symbols information (%2d (0x%02x))" % (calcHeaderLen, calcHeaderLen, headerLen, headerLen) )
  if (messageHeader.u2.s2.Type in LpcMessageType):
    dprintln( "Message type     : " + LpcMessageType[messageHeader.u2.s2.Type] )
  else:
    dprintln( "Message type     : %3d (0x%x)" % (messageHeader.u2.s2.Type, messageHeader.u2.s2.Type) )
  procFindStr = "<link cmd=\"!process 0x%x\">%d(0x%x)</link>" % (messageHeader.ClientId.UniqueProcess, messageHeader.ClientId.UniqueProcess, messageHeader.ClientId.UniqueProcess)
  dprintln( "Client ID        : process= " + procFindStr + ", thread= %d(0x%x)" %  (messageHeader.ClientId.UniqueThread, messageHeader.ClientId.UniqueThread), True)
  dprintln( "View/Callback    : %d (0x%x)" % (messageHeader.ClientViewSize, messageHeader.ClientViewSize) )
  if (printFormat not in DispFormatsLength):
    dprintln( "WARRING: Unknown (" + printFormat + ") diplay fromat. Use \"b\"" )
    printFormat = "b"
  dataAddr = messageHeader.getAddress() + headerLen
  printCommand = "d" + printFormat + " 0x%x" % dataAddr
  dataCount = messageHeader.u1.s1.DataLength / DispFormatsLength[printFormat]
  printCommand += " L 0x%x " % dataCount
  dprintln( "<link cmd=\"" + printCommand + "\">Dump of message data:</link>", True )
  dprintln( dbgCommand(printCommand) )
    

if __name__ == "__main__":
  argc = len(sys.argv)
  if (2 == argc):
    PrintPortMesage(expr(sys.argv[1]))
  elif (3 == argc):
    PrintPortMesage(expr(sys.argv[1]), sys.argv[2])
  elif (4 == argc):
    PrintPortMesage(expr(sys.argv[1]), sys.argv[2], sys.argv[3] == "True")
  elif (5 == argc):
    PrintPortMesage(expr(sys.argv[1]), sys.argv[2], sys.argv[3] == "True", sys.argv[4])
  else:
    dprintln(PrintPortMesage.__doc__, True)
