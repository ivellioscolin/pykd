#
#
#

import sys
import fnmatch
from pykd import *


def iat( moduleName, mask = "*" ):

    mod = module( moduleName )
    dprintln( "Module: " + moduleName + " base: %x" % mod.begin() + " end: %x" % mod.end() )

    if isKernelDebugging():
        systemModule = module( "nt" )
    else:
        systemModule = module( "ntdll" )
    

    if is64bitSystem():
        ntHeader = systemModule.typedVar( "_IMAGE_NT_HEADERS64", mod.begin() + ptrDWord( mod.begin() + 0x3c ) )
        if ntHeader.OptionalHeader.Magic == 0x10b:
            systemModule = loadModule( "ntdll32" ) 
            ntHeader = systemModule.typedVar( "_IMAGE_NT_HEADERS", mod.begin() + ptrDWord( mod.begin() + 0x3c ) )
            pSize = 4
        else:
            pSize = 8     
    else:
        ntHeader = systemModule.typedVar(  "_IMAGE_NT_HEADERS", mod.begin() + ptrDWord( mod.begin() + 0x3c ) )
        pSize = 4


    dprintln( "IAT RVA: %x  Size: %x" % ( ntHeader.OptionalHeader.DataDirectory[12].VirtualAddress, ntHeader.OptionalHeader.DataDirectory[12].Size  ) )
    dprintln( "========================" )

    if ntHeader.OptionalHeader.DataDirectory[12].Size == 0:
        return
    
    iatAddr = mod.begin() + ntHeader.OptionalHeader.DataDirectory[12].VirtualAddress;

    for i in range( 0, ntHeader.OptionalHeader.DataDirectory[12].Size / pSize ):

        if ( pSize == 4 ):
            iatEntry = addr64(ptrDWord( iatAddr + i*pSize ))
        else:
            iatEntry = addr64(ptrQWord( iatAddr + i*pSize ))

        if  iatEntry != None and iatEntry != 0:
            symbolName = findSymbol( iatEntry ) 
            if fnmatch.fnmatch( symbolName, mask ): 
                dprintln( symbolName ) 


if __name__ == "__main__":

    if not isWindbgExt():
        print "script is launch out of windbg"
        quit( 0 )

    if len (sys.argv)<=1:
        dprintln( "usage: !py import module_name ( symbol name mask )" )
    elif len( sys.argv ) == 2:
        iat( sys.argv[1] )
    else:
        iat( sys.argv[1], sys.argv[2] )

