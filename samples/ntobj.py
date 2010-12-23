# 
# Work with NT Object tree manager
# 
# To use:
# 
#   ntobj.getType(p)
#     query object header by object pointer
# 
#   ntobj.getListByHandleTable(pHandleTable, pType=0, bContainHeaders=True)
#     build object list from handle table
# 

from pykd import *

def getTypeWin7(p):
  """
  Get object header by object pointer
  Implementation for Win7+
  """
  pHeader = containingRecord(p, "nt", "_OBJECT_HEADER", "Body")
  pTypeIndexTable = getOffset("nt", "ObTypeIndexTable")
  return ptrPtr(pTypeIndexTable + (ptrSize() * pHeader.TypeIndex))

def getTypeLegacy(p):
  """
  Get object header by object pointer
  Implementation for before Win7
  """
  pHeader = containingRecord(p, "nt", "_OBJECT_HEADER", "Body")
  return addr64(pHeader.Type)

# Select platform-specific function for getting object header
if (ptrWord(getOffset("nt", "NtBuildNumber")) >= 7600):
  getType = getTypeWin7
else:
  getType = getTypeLegacy

HANDLE_VALUE_INC = 4
HT_PAGE_SIZE = 4096
HT_ENTRY_SIZE = (2 * ptrSize())
HT_LOWLEVEL_COUNT = HT_PAGE_SIZE // HT_ENTRY_SIZE
HT_MIDLEVEL_COUNT = HT_PAGE_SIZE // ptrSize()

def getListByHandleTable(pHandleTable, pType=0, bContainHeaders=True):
  """ 
  Build list of objects from target handle table

  Parameter pType if not 0 used for getting object of specific type,
  otherwise get object of all types

  Parameter bContainHeaders used to interpret table contents: 
  if bContainHeaders=True then table contains pointers to nt!_OBJECT_HEADER, 
  otherwise table contains pointers to objects
  """

  def getByHandleEntry(hEntry, bContainHeader):
    """
    Query object pointer by handle entry from handle table
    """
    if (0 == hEntry):
      return 0
  
    HandleEntry = typedVar("nt", "_HANDLE_TABLE_ENTRY", addr64(hEntry))
    if (0xFFFFFFFE == HandleEntry.NextFreeTableEntry):
      return 0

    p = ptrPtr(HandleEntry.getAddress()) & 0xFFFFFFFFFFFFFFF8
    if (0 == p):
      return 0

    if (bContainHeader):
      pHeader = typedVar("nt", "_OBJECT_HEADER", addr64(p))
      p = pHeader.Body.getAddress()
    return p

  def getListByHandleTableL0(pTableContent, nMaxHandleIndex, pType, bContainHeaders):
    """
    Build list of objects from target handle table level 0
    """
    lstObjects = list()
    nTableLevel0Count = nMaxHandleIndex // HANDLE_VALUE_INC
    for HandleEntryIndex in range(nTableLevel0Count):

      pHandleEntry = pTableContent + (HT_ENTRY_SIZE * HandleEntryIndex)
      p = getByHandleEntry(pHandleEntry, bContainHeaders)
      if (0 == p):
        continue

      if (0 == pType):
        lstObjects.append(p)
      else:
        pCurrentType = getType(p)
        if (addr64(pType) == pCurrentType):
          lstObjects.append(p)

    return lstObjects

  def getListByHandleTableL1(pTableContent, nMaxHandleIndex, pType, bContainHeaders):
    """
    Build list of objects from target handle table level 1
    """
    lstObjects = list()
    nTableLevel1Count = (nMaxHandleIndex // HANDLE_VALUE_INC) // HT_LOWLEVEL_COUNT
    for Index in range(nTableLevel1Count):
      pTableLevel0 = ptrPtr(pTableContent + (Index * ptrSize()))
      lstObjects += getListByHandleTableL0(pTableLevel0, HT_LOWLEVEL_COUNT * HANDLE_VALUE_INC, pType, bContainHeaders)

    return lstObjects

  def getListByHandleTableL2(pTableContent, nMaxHandleIndex, pType, bContainHeaders):
    """
    Build list of objects from target handle table level 2
    """
    lstObjects = list()
    nTableLevel2Count = ((nMaxHandleIndex // HANDLE_VALUE_INC) // HT_LOWLEVEL_COUNT) // HT_MIDLEVEL_COUNT
    for Index in range(nTableLevel2Count):
      pTableLevel1 = ptrPtr(pTableContent + (Index * ptrSize()))
      lstObjects += getListByHandleTableL1(pTableLevel1, HT_MIDLEVEL_COUNT * HT_LOWLEVEL_COUNT * HANDLE_VALUE_INC, pType, bContainHeaders)

    return lstObjects

  pHandleTable = typedVar("nt", "_HANDLE_TABLE", pHandleTable)
  nMaxHandleIndex = pHandleTable.NextHandleNeedingPool & 0xFFFFFFFF
  nTableLevel = (pHandleTable.TableCode & 3)
  pTableContent = pHandleTable.TableCode - nTableLevel

  if (0 == nTableLevel):
    return getListByHandleTableL0(pTableContent, nMaxHandleIndex, pType, bContainHeaders)
  elif (1 == nTableLevel):
    return getListByHandleTableL1(pTableContent, nMaxHandleIndex, pType, bContainHeaders)
  elif (2 == nTableLevel):
    return getListByHandleTableL2(pTableContent, nMaxHandleIndex, pType, bContainHeaders)

  dprintln("ERROR: Unknown handle table level: %u" % nTableLevel)
  return list()
