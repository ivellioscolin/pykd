# 
# Add synthetic symbols for module by imports
# 

from pykd import *
import sys

def addSymSymbolsByImports(dbgModule):
  if isKernelDebugging():
      systemModule = loadModule( "nt" )
  else:
      systemModule = loadModule( "ntdll" )

  if is64bitSystem():
    ntHeader = typedVar( systemModule.name(), "_IMAGE_NT_HEADERS64", dbgModule.begin() + ptrDWord( dbgModule.begin() + 0x3c ) )
    if ntHeader.OptionalHeader.Magic == 0x10b:
      systemModule = loadModule( "ntdll32" ) 
      ntHeader = typedVar( systemModule.name(), "_IMAGE_NT_HEADERS", dbgModule.begin() + ptrDWord( dbgModule.begin() + 0x3c ) )
      pSize = 4
    else:
      pSize = 8     
  else:
      ntHeader = typedVar( systemModule.name(), "_IMAGE_NT_HEADERS", dbgModule.begin() + ptrDWord( dbgModule.begin() + 0x3c ) )
      pSize = 4

  if ntHeader.OptionalHeader.DataDirectory[12].Size == 0:
    return
  
  iatAddr = dbgModule.begin() + ntHeader.OptionalHeader.DataDirectory[12].VirtualAddress;

  for i in range( 0, ntHeader.OptionalHeader.DataDirectory[12].Size / pSize ):
      pIatEtry = iatAddr + i*pSize;

      if ( pSize == 4 ):
        iatEntry = ptrDWord( pIatEtry )
      else:
        iatEntry = ptrQWord( pIatEtry )

      if iatEntry != 0:
        symbolName = findSymbol( iatEntry ) 
        addSynSymbol(pIatEtry, pSize, "_imp_" + symbolName)

if __name__ == "__main__":

  if not isSessionStart():
    print "Script is launch out of WinDBG"
    quit(0)

  argc = len(sys.argv)
  if (2 == argc):
    addSymSymbolsByImports(findModule(expr(sys.argv[1])))
  else:
    dprintln("Invalid command line")
    dprintln("Usage: " + sys.argv[0] + " module_address")
