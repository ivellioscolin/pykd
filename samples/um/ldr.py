from pykd import *

def main():
    pass

def listModuleFromLdr64():

    dprintln( "<u>64 bit modules:</u>", True )
 
    peb = typedVar( "ntdll!PEB", getProcessOffset(getCurrentProcess()) )

    moduleLst = typedVarList( peb.Ldr.deref().InMemoryOrderModuleList, "ntdll!_LDR_DATA_TABLE_ENTRY", "InMemoryOrderLinks" )

    for mod in moduleLst:
        name = typedVar( "ntdll!_UNICODE_STRING", mod.BaseDllName )  
        dprintln(loadWChars(name.Buffer, name.Length/2))

    try:
    
        peb32 = typedVar( "ntdll32!_PEB", getProcessOffset(getCurrentProcess()) - pageSize() )

        dprintln( "\n<u>32 bit modules:</u>", True)

        moduleLst = typedVarList( peb32.Ldr.deref().InMemoryOrderModuleList, "ntdll32!_LDR_DATA_TABLE_ENTRY", "InMemoryOrderLinks" )

        for mod in moduleLst:
            name = typedVar( "ntdll32!_UNICODE_STRING", mod.BaseDllName )  
            dprintln(loadWChars(name.Buffer, name.Length/2))

    except BaseException:
        pass

def listModuleFromLdr():

    peb = typedVar( "ntdll!PEB", getProcessOffset(getCurrentProcess()) )
    
    moduleLst = typedVarList( peb.Ldr.deref().InMemoryOrderModuleList, "ntdll!_LDR_DATA_TABLE_ENTRY", "InMemoryOrderLinks" )

    for mod in moduleLst:
        dprintln(loadUnicodeString(mod.BaseDllName))


def run():

    while True:

        if isKernelDebugging():
            dprintln( "not a user debugging" )
            break 
        
        if is64bitSystem():
            listModuleFromLdr64()
        else:
            listModuleFromLdr()

        break

if __name__ == "__main__":
    run()