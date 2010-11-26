import sys
from pykd import *


def printStack():

    threadList = getThreadList()

    oldMode = getProcessorMode()

    if oldMode == "X64" and loadModule( "wow64" ) != None:
        setProcessorMode("X86")

    for threadPtr in threadList:
        setImplicitThread( threadPtr )
        stackFrames = getCurrentStack()
        for frame in stackFrames: dprintln( findSymbol( frame.instructionOffset ) + "  (%x)" % frame.instructionOffset )
        dprintln("")

    setProcessorMode(oldMode)


if __name__ == "__main__":

    if not isSessionStart():
        createSession()
        loadDump( sys.argv[1] )
        dprintln( sys.argv[1] + " - loaded OK" )

    printStack()

