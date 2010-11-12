#
#
#

import sys
import fnmatch
from pykd import *


def export( moduleName, mask = "*" ):

    module = loadModule( moduleName )
    dprintln( "Module: " + moduleName + " base: %x" % module.begin() + " end: %x" % module.end() )


    systemModule = loadModule( "nt" )

    if systemModule==None:
        systemModule = loadModule( "ntdll" ) 	


#    dosHeader = typedVar( systemModule.name(), "_IMAGE_DOS_HEADER", module.begin() )


    if is64bitSystem():
        ntHeader = typedVar( systemModule.name(), "_IMAGE_NT_HEADERS64", module.begin() + ptrDWord( module.begin() + 0x3c ) )
    else:
        ntHeader = typedVar( systemModule.name(), "_IMAGE_NT_HEADERS", module.begin() + ptrDWord( module.begin() + 0x3c ) )


    dprintln( "Export RVA: %x  Size: %x" % ( ntHeader.OptionalHeader.DataDirectory[0].VirtualAddress, ntHeader.OptionalHeader.DataDirectory[0].Size  ) )
    dprintln( "========================" )

    if ntHeader.OptionalHeader.DataDirectory[0].Size == 0:
        return
    
    exportDirAddr = module.begin() + ntHeader.OptionalHeader.DataDirectory[0].VirtualAddress;

    namesCount = ptrDWord( exportDirAddr + 0x18 )
   
    namesRva = module.begin() + ptrDWord( exportDirAddr + 0x20 ) 

    for i in range( 0, namesCount ):
        exportName = loadCStr( module.begin() + ptrDWord( namesRva + 4 * i ) )
        if fnmatch.fnmatch( exportName, mask ): 
            dprintln( exportName )    


if __name__ == "__main__":

    if not isSessionStart():
        print "script is launch out of windbg"
        quit( 0 )

    if len( sys.argv ) == 1:

        if sys.argv[0]=="":
            dprintln( "module name not found" )
        else:      
            export( sys.argv[0] )

    else:
     
        export( sys.argv[0], sys.argv[1] )

       
