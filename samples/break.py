#
#
#

from pykd import *


def bpCallback():

    if is64bitSystem():
        objAttr = typedVar( "ntdll", "_OBJECT_ATTRIBUTES", reg("r8") ) 
    else:
        objAttr = typedVar( "ntdll", "_OBJECT_ATTRIBUTES", ptrPtr(reg("esp") + 0xC) )  

    name = loadUnicodeString( objAttr.ObjectName )

    dprintln( "NtCreateFile: " + name )

    return DEBUG_STATUS_GO_HANDLED



if not isWindbgExt():
    startProcess("notepad.exe")



if not isDumpAnalyzing() and not isKernelDebugging():
    	
    nt = loadModule("ntdll")

    b1 = bp( nt.NtCreateFile, bpCallback )
   
    # wait for user break, exceptions or process exit
    go()

    dprintln( "stopped" )    

else:

    dprintln( "The debugger must be connected to live usermode process" )    
   
        
