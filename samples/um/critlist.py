from pykd import *

def main():
    pass

def listCritSections():

    ntdll = module("ntdll")

    dbglst = ntdll.typedVarList( ntdll.RtlCriticalSectionList, "_RTL_CRITICAL_SECTION_DEBUG", "ProcessLocksList" )
    
    crtlst = [ ntdll.typedVar( "_RTL_CRITICAL_SECTION", x.CriticalSection ) for x in dbglst ]
    
    for crtsec in crtlst:
        dprintln("")
        dprintln( "CRITICAL SECTION  address = %#x ( %s ) " % ( crtsec, findSymbol( crtsec ) ) )
        dprintln( "  Owning thread = %x" % crtsec.OwningThread )
        dprintln( "  Lock count = %d" % crtsec.LockCount )
        

def run():

    while True:

        if isKernelDebugging():
            dprintln( "not a user debugging" )
            break 
        
        listCritSections()        

        break

if __name__ == "__main__":
    run()