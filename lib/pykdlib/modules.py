
#
#  Modules Info
#

import pykd

moduleList = []            

def kernelReloadModules():

    global nt 
    global moduleList

    nt = pykd.loadModule("nt")

    modules = pykd.typedVarList( nt.PsLoadedModuleList, "nt", "_LDR_DATA_TABLE_ENTRY", "InLoadOrderLinks" )

    moduleList = []

    moduleList.append( nt )

    for m in modules:	

        baseName = str( pykd.loadUnicodeString( m.BaseDllName.getAddress() ) )

        if baseName=="ntoskrnl.exe":
            continue

        module = pykd.findModule( m.DllBase )

        globals()[ module.name().lower() ] = module

        moduleList.append( module )


def userReloadModules():

    pass  


def printModuleList():
    pykd.dprintln( "\n".join( [ str(m) for m in moduleList ] ) )                      



if pykd.isKernelDebugging():

    kernelReloadModules()

else:

    userReloadModules()         












