#
#
#

import sys
from pykd import *

def symreload():

    reloadModule( "/f" )

    PsLoadedModuleList = getOffset( "nt", "PsLoadedModuleList" )

    loadedModulesInfo = typedVarList( PsLoadedModuleList, "nt", "_LDR_DATA_TABLE_ENTRY", "InLoadOrderLinks" )

    for module in loadedModulesInfo:
 
        if "" == getPdbFile( module.DllBase ):
            baseName = loadUnicodeString( module.BaseDllName.getAddress() )
	    if baseName=="ntoskrnl.exe": baseName = "nt"
            reloadModule(  " /u " + str(baseName)  )
   
if __name__ == "__main__":

    if not isSessionStart():
        dprintln( "script is launch out of windbg" )
        quit( 0 )

    symreload()