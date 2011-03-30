
#
#  Modules Info
#

import pykd

moduleList = []            

def reloadModules():

    global moduleList

    
    for m in moduleList: globals()[ m.name().lower() ] = None


    if pykd.isKernelDebugging():

        global nt 

        nt = pykd.loadModule("nt")

        modules = pykd.typedVarList( nt.PsLoadedModuleList, "nt", "_LDR_DATA_TABLE_ENTRY", "InLoadOrderLinks" )

        moduleList.append( nt )

    else:
        
        ntdll = pykd.loadModule("ntdll")

        peb = pykd.typedVar( "ntdll", "_PEB", pykd.getCurrentProcess() )

        ldr = pykd.typedVar( "ntdll", "_PEB_LDR_DATA", peb.Ldr )

        modules = pykd.typedVarList( ldr.InLoadOrderModuleList.getAddress(), "ntdll", "_LDR_DATA_TABLE_ENTRY", "InLoadOrderLinks" ) 
 

    moduleList = []

    for m in modules:	

        baseName = str( pykd.loadUnicodeString( m.BaseDllName.getAddress() ) )

        if baseName=="ntoskrnl.exe":
            continue

        module = pykd.findModule( m.DllBase )
  
        globals()[ module.name().lower() ] = module

        moduleList.append( module )


def printModuleList():
    pykd.dprintln( "\n".join( [ str(m) for m in moduleList ] ) )                      


reloadModules()


    












