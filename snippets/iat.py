#
#
#

import sys
import fnmatch
from pykd import *


def iat( moduleName, mask = "*" ):

    module = loadModule( moduleName )
    dprintln( "Module: " + moduleName + " base: %x" % module.begin() + " end: %x" % module.end() )

    if isKernelDebugging():
        systemModule = loadModule( "nt" )
    else:
        systemModule = loadModule( "ntdll" )
    

    if is64bitSystem():
        ntHeader = typedVar( systemModule.name(), "_IMAGE_NT_HEADERS64", module.begin() + ptrDWord( module.begin() + 0x3c ) )
        if ntHeader.OptionalHeader.Magic == 0x10b:
            systemModule = loadModule( "ntdll32" ) 
            ntHeader = typedVar( systemModule.name(), "_IMAGE_NT_HEADERS", module.begin() + ptrDWord( module.begin() + 0x3c ) )
            pSize = 4
        else:
            pSize = 8     
    else:
        ntHeader = typedVar( systemModule.name(), "_IMAGE_NT_HEADERS", module.begin() + ptrDWord( module.begin() + 0x3c ) )
        pSize = 4


    dprintln( "IAT RVA: %x  Size: %x" % ( ntHeader.OptionalHeader.DataDirectory[12].VirtualAddress, ntHeader.OptionalHeader.DataDirectory[12].Size  ) )
    dprintln( "========================" )

    if ntHeader.OptionalHeader.DataDirectory[12].Size == 0:
        return
    
    iatAddr = module.begin() + ntHeader.OptionalHeader.DataDirectory[12].VirtualAddress;

    for i in range( 0, ntHeader.OptionalHeader.DataDirectory[12].Size / pSize ):

        if ( pSize == 4 ):
            iatEntry = ptrDWord( iatAddr + i*pSize )
        else:
            iatEntry = ptrQWord( iatAddr + i*pSize )

        if  iatEntry != 0:
            symbolName = findSymbol( iatEntry ) 
            if fnmatch.fnmatch( symbolName, mask ): 
                dprintln( symbolName )

 


if __name__ == "__main__":

    if not isSessionStart():
        print "script is launch out of windbg"
        quit( 0 )

    if len (sys.argv)<=0:
        dprintln( "usage: !py import module_name ( symbol name mask )" )
    elif len( sys.argv ) == 2:
        iat( sys.argv[1] )
    else:
        iat( sys.argv[1], sys.argv[2] )

