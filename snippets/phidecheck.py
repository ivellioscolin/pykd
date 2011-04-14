# 
# Search hidden processes: 
# compare content of PspCidTable table and PsActiveProcessHead list
# 

from pykd import *
import ntobj

if __name__ == "__main__":

  if not isWindbgExt():
    print "Script is launch out of WinDBG"
    quit(0)

  # build list from PsActiveProcessHead
  pActiveProcessList = getOffset("nt", "PsActiveProcessHead")
  lstTypedActiveProcesses = typedVarList(pActiveProcessList, "nt", "_EPROCESS", "ActiveProcessLinks")
  lstActiveProcesses = [process.getAddress() for process in lstTypedActiveProcesses]

  # build list from PspCidTable
  pCidTable = ptrPtr(getOffset("nt", "PspCidTable"))
  pProcessType = ptrPtr(getOffset("nt", "PsProcessType"))
  lstProcessTable = ntobj.getListByHandleTable(pCidTable, pProcessType, False)

  # compare lists and print result
  founded = 0
  for processFromTable in lstProcessTable:
    if (0 == lstActiveProcesses.count(processFromTable)):
      dprintln("!process 0x%X removed from PsActiveProcessHead" % processFromTable)
      founded += 1
  dprintln("checked %u processes" % len(lstProcessTable) + (", %u hidden" % founded if (0 != founded) else ", hidden not found"))
