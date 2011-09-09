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


def PrintPortMesage(messageAddr, printFormat="b", use32=False):
  """
  Print _PORT_MESSAGE header and dump of message dump

  Usage: portmsg messageAddr, printFormat[=b], use32[=False]
  When:
    messageAddr - address of port message
    printFormat - string of display format ("d+printFormat"). 
                  May be: a, b, yb, w, W, u, d, dc, yd, f, q, D, p, ps
                  Display formats overview:
    <link cmd=\".shell -x rundll32 url.dll,FileProtocolHandler http://msdn.microsoft.com/en-us/library/ff542790(VS.85).aspx\">http://msdn.microsoft.com/en-us/library/ff542790(VS.85).aspx</link>

    use32       - use _PORT_MESSAGE32 (instead of _PORT_MESSAGE) structure (True or False)
  """

  # WOW64 workaround: !!! workitem/9499 !!!!
  dynPtr = typeInfo("", "portmsg *")


  def buildPortMessageType():
    clientIdType = typeInfo("portmsg~_CLIENT_ID")
    clientIdType.append(dynPtr, "UniqueProcess")
    clientIdType.append(dynPtr, "UniqueThread")

    print clientIdType

    portMsgType = typeInfo("portmsg~_PORT_MESSAGE")
    portMsgType.append(ushort_t,      "DataLength")
    portMsgType.append(ushort_t,      "TotalLength")
    portMsgType.append(ushort_t,      "Type")
    portMsgType.append(ushort_t,      "DataInfoOffset")
    portMsgType.append(clientIdType,  "ClientId")
    portMsgType.append(ulong_t,       "MessageId")
    portMsgType.append(ulong_t,       "CallbackId")
    return portMsgType

  def buildPortMessage32Type():

    clientIdType = typeInfo("portmsg~_CLIENT_ID32")
    clientIdType.append(ulong_t,  "UniqueProcess")
    clientIdType.append(ulong_t,  "UniqueThread")

    portMsgType = typeInfo("portmsg~_PORT_MESSAGE32", 4)
    portMsgType.append(ushort_t,      "DataLength")
    portMsgType.append(ushort_t,      "TotalLength")
    portMsgType.append(ushort_t,      "Type")
    portMsgType.append(ushort_t,      "DataInfoOffset")
    portMsgType.append(clientIdType,  "ClientId")
    portMsgType.append(ulong_t,       "MessageId")
    portMsgType.append(ulong_t,       "CallbackId")
    return portMsgType


  if (use32):
    messageTypeName = buildPortMessage32Type()
  else:
    messageTypeName = buildPortMessageType()

  messageHeader = typedVar(messageTypeName, messageAddr)
  if (None == messageHeader):
    dprintln("ERROR: Getting (" + moduleName + "!" + messageTypeName + " *)(0x%x) failed" % messageAddr )
    return

  dprintln( "Data length      : %3d (0x%02x)" % (messageHeader.DataLength, messageHeader.DataLength) )
  dprintln( "Total length     : %3d (0x%02x)" % (messageHeader.TotalLength, messageHeader.TotalLength) )
  calcHeaderLen = messageHeader.TotalLength - messageHeader.DataLength
  headerLen = messageTypeName.size()
  if (calcHeaderLen != headerLen):
    dprintln( "WARRING: calculated size (%2d (0x%02x)) of LPC-header does not match with symbols information (%2d (0x%02x))" % (calcHeaderLen, calcHeaderLen, headerLen, headerLen) )
  if (messageHeader.Type in LpcMessageType):
    dprintln( "Message type     : " + LpcMessageType[messageHeader.Type] )
  else:
    dprintln( "Message type     : %3d (0x%x)" % (messageHeader.Type, messageHeader.Type) )

  procFindStr = ""
  if isKernelDebugging():
    procFindStr = "<link cmd=\"!process 0x%x\">%d(0x%x)</link>" % (messageHeader.ClientId.UniqueProcess, messageHeader.ClientId.UniqueProcess, messageHeader.ClientId.UniqueProcess)
  else:
    procFindStr = "%d(0x%x)" % (messageHeader.ClientId.UniqueProcess, messageHeader.ClientId.UniqueProcess)
  dprintln( "Client ID        : process= " + procFindStr + ", thread= %d(0x%x)" %  (messageHeader.ClientId.UniqueThread, messageHeader.ClientId.UniqueThread), isKernelDebugging())
  dprintln( "View/Callback    : %d (0x%x)" % (messageHeader.CallbackId, messageHeader.CallbackId) )
  if (printFormat not in DispFormatsLength):
    dprintln( "WARRING: Unknown (" + printFormat + ") diplay fromat. Use \"b\"" )
    printFormat = "b"
  dataAddr = messageHeader.getAddress() + headerLen
  printCommand = "d" + printFormat + " 0x%x" % dataAddr
  dataCount = messageHeader.DataLength / DispFormatsLength[printFormat]
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
  else:
    dprintln(PrintPortMesage.__doc__, True)
