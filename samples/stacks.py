import sys
from pykd import *


def printStack():        


    def printThreadStack( threadPtr ):

        setImplicitThread( threadPtr )
        stackFrames = getCurrentStack()
        for frame in stackFrames: dprintln( findSymbol( frame.instructionOffset ) + "  (%x)" % frame.instructionOffset )
        dprintln("")  



    def printUserStack():

        threadList = getThreadList()

        oldMode = getProcessorMode()

        if oldMode == "X64" and loadModule( "wow64" ) != None:
            setProcessorMode("X86")

        for threadPtr in threadList:
            printThreadStack( threadPtr )

        setProcessorMode(oldMode)



    def printKernelStack():

        process = typedVar( "nt", "_EPROCESS", getCurrentProcess() ) 

        threadList = typedVarList( process.ThreadListHead.getAddress(), "nt", "_ETHREAD", "ThreadListEntry" )

        oldMode = getProcessorMode()

        if is64bitSystem() and process.Wow64Process != 0:
            setProcessorMode("X86")

        for thread in threadList:
            printThreadStack( thread.getAddress() )

        setProcessorMode(oldMode)


    if isKernelDebugging():
        printKernelStack()
    else:
        printUserStack()



if __name__ == "__main__":

    while True:

        if not isWindbgExt():
            if not loadDump( sys.argv[1] ):
                dprintln( sys.argv[1] + " - load failed" )
                break
                
        printStack()
        break      


