#
#
#

import sys
import fnmatch
from pykd import *


def export( moduleName, mask = "*" ):

    modObj = module( moduleName )
    dprintln( "Module: " + moduleName + " base: %x" % modObj.begin() + " end: %x" % modObj.end() )

    if isKernelDebugging():
        systemModule = module( "nt" )
    else:
        systemModule = module( "ntdll" )
   

    if is64bitSystem():
        ntHeader = systemModule.typedVar( "_IMAGE_NT_HEADERS64", modObj.begin() + ptrDWord( modObj.begin() + 0x3c ) )
        if ntHeader.OptionalHeader.Magic == 0x10b:
            systemModule = loadModule( "ntdll32" ) 
            ntHeader = systemModule.typedVar( "_IMAGE_NT_HEADERS", modObj.begin() + ptrDWord( modObj.begin() + 0x3c ) )
    else:
        ntHeader = systemModule.typedVar("_IMAGE_NT_HEADERS", modObj.begin() + ptrDWord( modObj.begin() + 0x3c ) )


    dprintln( "Export RVA: %x  Size: %x" % ( ntHeader.OptionalHeader.DataDirectory[0].VirtualAddress, ntHeader.OptionalHeader.DataDirectory[0].Size  ) )
    dprintln( "========================" )

    if ntHeader.OptionalHeader.DataDirectory[0].Size == 0:
        return
    
    exportDirAddr = modObj.begin() + ntHeader.OptionalHeader.DataDirectory[0].VirtualAddress;

    namesCount = ptrDWord( exportDirAddr + 0x18 )
   
    namesRva = modObj.begin() + ptrDWord( exportDirAddr + 0x20 ) 

    for i in range( 0, namesCount ):
        exportName = loadCStr( modObj.begin() + ptrDWord( namesRva + 4 * i ) )
        if fnmatch.fnmatch( exportName, mask ): 
            dprintln( exportName )


if __name__ == "__main__":

    if not isWindbgExt():
        print "script is launch out of windbg"
        quit( 0 )

    if len (sys.argv)<=1:
        dprintln( "usage: !py export module_name ( export mask )" )
    elif len( sys.argv ) == 2:
        export( sys.argv[1] )
    else:
        export( sys.argv[1], sys.argv[2] )

       
