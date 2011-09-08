"""
Prase first param RpcServerRegisterIf[Xxx]
"""

import sys
from pykd import *

def rpcSrvIf(ifSpec):
  """
  Prase RPC server interface specification

  Usage example:

  kd> ba e 1 rpcrt4!RPC_SERVER::RegisterInterface
  kd> gc

  And analyze first parameter

    for x86:
  kd> !py rpcSrvIf poi(@esp+@$ptrsize)
    or for x64:
  kd> !py rpcSrvIf @rdx

  P.S.  RPC_SERVER::RegisterInterface called from: RpcServerRegisterIf, 
        RpcServerRegisterIf2 and RpcServerRegisterIfEx

  """

  def formatSintaxId(synId):

    def formatGuid(guid):
      part1 = "%08x-%04x-%04x" % (guid.data1, guid.data2, guid.data3)
      part2 = "".join( ["%02x" % _byte for _byte in guid.data4] )
      part3 = "".join( ["%02x" % _byte for _byte in guid.data5] )
      return part1 + "-" + part2 + "-" + part3

    def formatRpcVer(prcVer):
      return "v.%d.%d" % (prcVer.MajorVersion, prcVer.MinorVersion)

    return formatGuid(synId.SyntaxGUID) + " " + formatRpcVer(synId.SyntaxVersion)

  # prepare structures for parsing
  commGuid = typeInfo("rpcSrvIf~_GUID")
  commGuid.append(ulong_t,  "data1")
  commGuid.append(ushort_t, "data2")
  commGuid.append(ushort_t, "data3")
  commGuid.append(uchar_t,  "data4", 2)
  commGuid.append(uchar_t,  "data5", 6)
  # print commGuid

  rpcVersion = typeInfo("rpcSrvIf~_RPC_VERSION")
  rpcVersion.append(ushort_t, "MajorVersion")
  rpcVersion.append(ushort_t, "MinorVersion")
  # print rpcVersion

  rpcSintaxIdentifier = typeInfo("rpcSrvIf~_RPC_SYNTAX_IDENTIFIER")
  rpcSintaxIdentifier.append(commGuid,    "SyntaxGUID")
  rpcSintaxIdentifier.append(rpcVersion,  "SyntaxVersion")
  # print rpcSintaxIdentifier

  prcDispatchTable = typeInfo("rpcSrvIf~_RPC_DISPATCH_TABLE")
  prcDispatchTable.append(uint_t, "DispatchTableCount")
  prcDispatchTable.append(ptr_t,  "DispatchTable")
  prcDispatchTable.append(ptr_t,  "Reserved")
  # print prcDispatchTable

  midlServerInfoHeader = typeInfo("rpcSrvIf~_MIDL_SERVER_INFO_hdr")
  midlServerInfoHeader.append(ptr_t,  "pStubDesc")
  midlServerInfoHeader.append(ptr_t,  "DispatchTable")
  # print midlServerInfoHeader

  rpcServerInterface = typeInfo("rpcSrvIf~_RPC_SERVER_INTERFACE")
  rpcServerInterface.append(uint_t,               "Length")
  rpcServerInterface.append(rpcSintaxIdentifier,  "InterfaceId")
  rpcServerInterface.append(rpcSintaxIdentifier,  "TransferSyntax")
  rpcServerInterface.append(ptr_t,                "DispatchTable") # -> prcDispatchTable
  rpcServerInterface.append(uint_t,               "RpcProtseqEndpointCount")
  rpcServerInterface.append(ptr_t,                "RpcProtseqEndpoint")
  rpcServerInterface.append(ptr_t,                "DefaultManagerEpv")
  rpcServerInterface.append(ptr_t,                "InterpreterInfo") # -> midlServerInfoHeader
  rpcServerInterface.append(uint_t,               "Flags")
  # print rpcServerInterface

  # get and print interface header
  srvIf = rpcServerInterface.load( ifSpec )
  dprintln("Interface ID    : " + formatSintaxId(srvIf.InterfaceId) )
  dprintln("Transfer Syntax : " + formatSintaxId(srvIf.TransferSyntax) )
  dprintln("Endpoint count  : %d" % srvIf.RpcProtseqEndpointCount )
  if srvIf.RpcProtseqEndpointCount:
    protoseqEndp = srvIf.RpcProtseqEndpoint
    for i in range(0, srvIf.RpcProtseqEndpointCount):
      dprintln("\t[%02d] protocol is `" % i + loadCStr( ptrPtr(protoseqEndp) ) + "'" )
      protoseqEndp += ptrSize()
      dprintln("\t[%02d] endpoint is `" % i + loadCStr( ptrPtr(protoseqEndp) ) + "'" )
      protoseqEndp += ptrSize()

  dprintln("")

  # query dispatch routines table
  dspTableSize = prcDispatchTable.load(srvIf.DispatchTable).DispatchTableCount
  dspTable = midlServerInfoHeader.load(srvIf.InterpreterInfo).DispatchTable
  dprintln('<exec cmd="dps 0x%x L 0x%x">Routine Table 0x%x, count %d (0x%02x)</exec>\n' % 
            (dspTable, dspTableSize, dspTable, dspTableSize, dspTableSize) ,True)

if __name__ == "__main__":
  if len(sys.argv) == 2:
    rpcSrvIf( expr(sys.argv[1]) )
  else:
    dprintln(rpcSrvIf.__doc__)
