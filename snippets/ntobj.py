"""
Work with NT Object tree manager

  getType(p)
    Return object type address by object address

  getObjectNameInfo(p)
    Return typedVar of nt!_OBJECT_HEADER_NAME_INFO or None

  getObjectName(p)
    Return string of object name. If can not get name - empty string

  buildObjectName(p)
    Return string of full object name. If can not get name - empty string

  getListByHandleTable(tableHandles=None, objTypeAddr=0, containHeaders=True)
    Return list of objects from table of handles

  getListByDirectoryObject(p, objTypeAddr=0)
    Return list of objects from object directory

  getObjectByName(name, caseSensitive=False)
    Return address of object by full name. If error (f.e. not exist) - None

"""

from pykd import *

nt = module("nt")

# optimization
OBJECT_HEADER = nt.type("_OBJECT_HEADER")
HANDLE_TABLE_ENTRY = nt.type("_HANDLE_TABLE_ENTRY")
OBJECT_DIRECTORY_ENTRY = nt.type("_OBJECT_DIRECTORY_ENTRY")
OBJECT_HEADER_NAME_INFO = nt.type("_OBJECT_HEADER_NAME_INFO")



def getTypeWin7(p):
  """
  Get object header by object pointer
  Implementation for Win7+
  """
  objHeader = containingRecord(p, OBJECT_HEADER, "Body")
  tableTypeIndex = nt.ObTypeIndexTable
  return ptrPtr(tableTypeIndex + (ptrSize() * objHeader.TypeIndex))

def getTypeLegacy(p):
  """
  Get object header by object pointer
  Implementation for before Win7
  """
  objHeader = containingRecord(p, OBJECT_HEADER, "Body")
  return objHeader.Type

# Select platform-specific function for getting object header
# Select key body type: nt!CmpKeyObjectType or nt!CmKeyObjectType
if (ptrWord( nt.NtBuildNumber ) >= 7600):
  getType = getTypeWin7
  # _kcbObjectType = expr("poi(nt!CmKeyObjectType)")
else:
  getType = getTypeLegacy
  # _kcbObjectType = expr("poi(nt!CmpKeyObjectType)")


def getObjectNameInfoFromHeader(p):
  """
  Get object name information from field NameInfoOffset of object header
  """
  objHeader = containingRecord(p, OBJECT_HEADER, "Body")
  if (0 == objHeader.NameInfoOffset):
    return None
  return typedVar(OBJECT_HEADER_NAME_INFO, objHeader.getAddress() - objHeader.NameInfoOffset)

def getObjectNameInfoFromInfoMask(p):
  """
  Get object name information from field NameInfoOffset of object header
  """
  objHeader = containingRecord(p, OBJECT_HEADER, "Body")
  if (0 == (objHeader.InfoMask & 2)):
    return None
  offsetNameInfo = ptrByte( nt.ObpInfoMaskToOffset + (objHeader.InfoMask & 3) )
  if (0 == offsetNameInfo):
    return None
  return typedVar(OBJECT_HEADER_NAME_INFO, objHeader.getAddress() - offsetNameInfo)


# Select platform-specific function for getting name of object
getObjectNameInfo = None
try:
  OBJECT_HEADER.NameInfoOffset
  getObjectNameInfo = getObjectNameInfoFromHeader
except TypeException:
  getObjectNameInfo = getObjectNameInfoFromInfoMask


def getObjectName(p):
  """
  Get object name by name information
  """
  nameInfo = getObjectNameInfo(p)
  if (None == nameInfo):
    return ""
  return loadUnicodeString(nameInfo.Name.getAddress())

def buildObjectName(p):
  """
  Get object name by name information (parent directory handled)
  """
  objectFullName = ""

  nameInfo = getObjectNameInfo(p)
  if None == nameInfo:
    return objectFullName
  namePart = loadUnicodeString(nameInfo.Name.getAddress())
  if namePart != "\\":
    objectFullName = namePart + "\\" + objectFullName
  else:
    objectFullName = "\\" + objectFullName

  while (0 != nameInfo.Directory):
    p = nameInfo.Directory
    nameInfo = getObjectNameInfo(p)
    if (None == nameInfo):
      return "...\\" + objectFullName
    namePart = loadUnicodeString(nameInfo.Name.getAddress())
    if namePart != "\\":
      objectFullName = namePart + "\\" + objectFullName
    else:
      objectFullName = "\\" + objectFullName
  
  return objectFullName


HANDLE_VALUE_INC = 4
HT_PAGE_SIZE = 4096
HT_ENTRY_SIZE = (2 * ptrSize())
HT_LOWLEVEL_COUNT = HT_PAGE_SIZE // HT_ENTRY_SIZE
HT_MIDLEVEL_COUNT = HT_PAGE_SIZE // ptrSize()

def getListByHandleTable(tableHandles=None, objTypeAddr=0, containHeaders=True):
  """ 
  Build list of objects from target handle table

  Parameter objTypeAddr if not 0 used for getting object of specific type,
  otherwise get object of all types

  Parameter containHeaders used to interpret table contents: 
  if containHeaders=True then table contains pointers to nt!_OBJECT_HEADER, 
  otherwise table contains pointers to objects
  """

  def getByHandleEntry(entryHandle, containHeader):
    """
    Query object pointer by handle entry from handle table
    """
    if (0 == entryHandle):
      return 0
  
    HandleEntry = typedVar( HANDLE_TABLE_ENTRY, entryHandle)
    if (0xFFFFFFFE == HandleEntry.NextFreeTableEntry):
      return 0

    p = ptrPtr(HandleEntry.getAddress()) & 0xFFFFFFFFFFFFFFF8
    if (0 == p):
      return 0

    if (containHeader):
      objHeader = typedVar( OBJECT_HEADER, p)
      p = objHeader.Body.getAddress()
    return p

  def getListByHandleTableL0(pTableContent, nMaxHandleIndex, objTypeAddr, containHeaders):
    """
    Build list of objects from target handle table level 0
    """
    lstObjects = list()
    nTableLevel0Count = nMaxHandleIndex // HANDLE_VALUE_INC
    for HandleEntryIndex in range(nTableLevel0Count):

      pHandleEntry = pTableContent + (HT_ENTRY_SIZE * HandleEntryIndex)
      p = getByHandleEntry(pHandleEntry, containHeaders)
      if (0 == p):
        continue

      if (0 == objTypeAddr):
        lstObjects.append( ( p, HandleEntryIndex*HANDLE_VALUE_INC) )
      else:
        pCurrentType = getType(p)
        if (addr64(objTypeAddr) == addr64(pCurrentType)):
          lstObjects.append( ( p, HandleEntryIndex*HANDLE_VALUE_INC) )

    return lstObjects

  def getListByHandleTableL1(pTableContent, nMaxHandleIndex, objTypeAddr, containHeaders):
    """
    Build list of objects from target handle table level 1
    """
    lstObjects = list()
    nTableLevel1Count = (nMaxHandleIndex // HANDLE_VALUE_INC) // HT_LOWLEVEL_COUNT
    for Index in range(nTableLevel1Count):
      pTableLevel0 = ptrPtr(pTableContent + (Index * ptrSize()))
      lstObjects += getListByHandleTableL0(pTableLevel0, HT_LOWLEVEL_COUNT * HANDLE_VALUE_INC, objTypeAddr, containHeaders)

    return lstObjects

  def getListByHandleTableL2(pTableContent, nMaxHandleIndex, objTypeAddr, containHeaders):
    """
    Build list of objects from target handle table level 2
    """
    lstObjects = list()
    nTableLevel2Count = ((nMaxHandleIndex // HANDLE_VALUE_INC) // HT_LOWLEVEL_COUNT) // HT_MIDLEVEL_COUNT
    for Index in range(nTableLevel2Count):
      pTableLevel1 = ptrPtr(pTableContent + (Index * ptrSize()))
      lstObjects += getListByHandleTableL1(pTableLevel1, HT_MIDLEVEL_COUNT * HT_LOWLEVEL_COUNT * HANDLE_VALUE_INC, objTypeAddr, containHeaders)

    return lstObjects

  if (None == tableHandles):
    currProcess = nt.typedVar("_EPROCESS", getCurrentProcess())
    if (None == currProcess):
      dprintln("Get current process failed")
      return
    tableHandles = currProcess.ObjectTable

  tableHandles = nt.typedVar("_HANDLE_TABLE", tableHandles)
  nMaxHandleIndex = tableHandles.NextHandleNeedingPool & 0xFFFFFFFF
  nTableLevel = (tableHandles.TableCode & 3)
  pTableContent = tableHandles.TableCode - nTableLevel

  if (0 == nTableLevel):
    return getListByHandleTableL0(pTableContent, nMaxHandleIndex, objTypeAddr, containHeaders)
  elif (1 == nTableLevel):
    return getListByHandleTableL1(pTableContent, nMaxHandleIndex, objTypeAddr, containHeaders)
  elif (2 == nTableLevel):
    return getListByHandleTableL2(pTableContent, nMaxHandleIndex, objTypeAddr, containHeaders)

  dprintln("ERROR: Unknown handle table level: %u" % nTableLevel)
  return list()
  
 

NUMBER_HASH_BUCKETS = 37

def getListByDirectoryObject(p, objTypeAddr=0):
  """ 
  Build list of objects from object directory

  Parameter objTypeAddr if not 0 used for getting object of specific type,
  otherwise get object of all types
  """

  if getType(p) != ptrPtr( nt.ObpDirectoryObjectType ):
    return None

  result = list()

  for i in range(0, NUMBER_HASH_BUCKETS):
    bucket = ptrPtr( p + (i * ptrSize()) )
    while bucket:
      bucketVar = typedVar( OBJECT_DIRECTORY_ENTRY, bucket)
      if objTypeAddr and (getType(bucketVar.Object) ==  objTypeAddr):
        result.append(bucketVar.Object)
      elif (not objTypeAddr):
        result.append(bucketVar.Object)
      bucket = bucketVar.ChainLink

  return result

def getObjectByName(name, caseSensitive=False):
  """
  Query address of object by full name
  """

  def cmpCase(s1, s2): return s1 == s2
  def cmpNoCase(s1, s2): return s1.lower() == s2.lower()

  if not len(name):
    return None

  if name[0] != '\\':
    return None

  object = ptrPtr( nt.ObpRootDirectoryObject )

  cmpFunc = cmpNoCase
  if caseSensitive:
    cmpFunc = cmpCase

  while True:
    name = name[1:]
    if not len(name):
      break

    tok = name.find("\\")
    if -1 != tok:
      namePart = name[:tok]
      name = name[tok:]
    else:
      namePart = name

    if 0 == len(namePart):
      return None

    # FIXME: use name/index hash
    lstObjects = getListByDirectoryObject(object)
    if None == lstObjects:
      return None

    found = False
    for p in lstObjects:
      objName = getObjectName(p)
      if len(objName) and cmpFunc( namePart, objName ):
        object = p
        found = True
        break

    if not found:
      return None

    if -1 == tok:
      break

  return object


import sys

# Display object kd-command
ViewObjectCommand = {
  addr64( expr("poi(nt!IoFileObjectType)") )    : "!fileobj",
  addr64( expr("poi(nt!PsProcessType)") )       : "!process",
  addr64( expr("poi(nt!PsThreadType)") )        : "!thread",
  addr64( expr("poi(nt!IoDeviceObjectType)") )  : "!devobj",
  addr64( expr("poi(nt!SeTokenObjectType)") )   : "!token",
  # addr64( _kcbObjectType )                      : "!reg kcb", 
  addr64( expr("poi(nt!IoDriverObjectType)") )  : "!drvobj 7"
}

def main():
  """
  Print content of object table (handle table)
  
  Usage:  
    !py ntobj [table= <ADDR>] [type= <ADDR>] [headers= <BOOL>]

  When (options can be specified in any order):
    table= <ADDR>   : table of handle address. F.e. address object table 
                      contained in field ObjectTable structure nt!_EPROCESS
                      default: nt!_EPROCESS.ObjectTable of current processes

    type= <ADDR>    : address of object type. If specified (!= 0) then print
                      object of target type only.
                      default: 0

    headers= <BOOL> : table of handles format: contain pointer to object or to 
                      object header (nt!_OBJECT_HEADER): True or Flase 
                      F.e. poi(nt!PspCidTable) contains objects (processes and 
                      threads), and nt!_EPROCESS.ObjectTable contains headers
                      default: True
  Examples:  
  <link cmd=\"!py ntobj\">!py ntobj</link>
  Print object table of current process

  <link cmd=\"!py ntobj type= poi(nt!IoFileObjectType)\">!py ntobj type= poi(nt!IoFileObjectType)</link>
  Print object table (only _FILE_OBJECT *) of current process
  
  <link cmd=\"!py ntobj table= poi(poi(nt!PsInitialSystemProcess)+@@C++(#FIELD_OFFSET(nt!_EPROCESS,ObjectTable)))\">!py ntobj table= poi(poi(nt!PsInitialSystemProcess)+@@C++(#FIELD_OFFSET(nt!_EPROCESS,ObjectTable)))</link>
  Print object table for SYSTEM process.

  <link cmd=\"!py ntobj table= poi(nt!PspCidTable) headers= False type= poi(nt!PsProcessType)\">!py ntobj table= poi(nt!PspCidTable) headers= False type= poi(nt!PsProcessType)</link>
  Print all process objects from nt!PspCidTable
  """

  if not isWindbgExt():
    print "Script is launch out of WinDBG"
    return

  if not isKernelDebugging():
    dprintln("This script for kernel debugging only")
    return

  argc = len(sys.argv)

  if (2 == argc) and ("help" == sys.argv[1] or "?" == sys.argv[1]):
    dprintln(main.__doc__, True)
    return

  if (0 == (argc % 2)):
    dprintln("Invalid number of comand line arguments")
    dprintln(main.__doc__, True)
    return

  tableHandles = None
  objTypeAddr = 0
  containHeaders = True

  for i in range(1, argc, 2):
    if ("table=" == sys.argv[i]):
      tableHandles = expr(sys.argv[i + 1])
    elif ("type=" == sys.argv[i]):
      objTypeAddr = expr(sys.argv[i + 1])
    elif ("headers=" == sys.argv[i]):
      containHeaders = ("True" == sys.argv[i + 1])
    else:
      dprintln("Unknown option `" + sys.argv[i] + "'")
      dprintln(main.__doc__, True)
      return

  ObjectHandlePairs = [ (p[0], p[1]) for p in getListByHandleTable(tableHandles, objTypeAddr, containHeaders) ]

  dprintln("%u objects:" % len(ObjectHandlePairs))
  for objectHandle in ObjectHandlePairs:
    objectType = getType(objectHandle[0])
    if objectType in ViewObjectCommand:
      viewCommand = ViewObjectCommand[objectType]
    else:
      viewCommand = "!object"

    dprint("\t<link cmd=\"" + viewCommand + " 0x%x\">" % objectHandle[0] + viewCommand + " 0x%x</link>" % objectHandle[0], True)
    objectName = buildObjectName(objectHandle[0])
    if len(objectName):
      dprint( ", name=`" + objectName + "'" )
    elif nt.typedVar("_OBJECT_TYPE", getType(objectHandle[0])).TypeInfo.QueryNameProcedure:
      dprint(", <i>custom</i> name", True)
    else:
      dprint(" , <_unnamed_>")

    dprint(", <link cmd=\"!handle 0x%x\">!handle 0x%x</link>\n" % (objectHandle[1], objectHandle[1]), True)

    dprint("\ttype is `" + getObjectName(objectType) + "' (<link cmd=\"!object 0x%x\">0x%x</link>)" % (objectType, objectType), True)

    dprintln( "\n" )

  if (1 == argc):
    dprintln("\n<link cmd=\"!py " + sys.argv[0] + " ?\" \">View help for ntobj</link>", True)

if __name__ == "__main__":
  main()
