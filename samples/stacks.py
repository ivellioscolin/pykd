import sys
from pykd import *


#def loadSymbols():
#
#    global nt
#    nt = loadModule( "nt" )
#    nt.PsActiveProcessHead = getOffset( "nt", "PsActiveProcessHead" )


def printStack():

    threadList = getThreadList()

    for threadPtr in threadList:
        oldThread = 0
        setImplicitThread( threadPtr )
        stackFrames = getCurrentStack()
        for frame in stackFrames: dprintln( findSymbol( frame.instructionOffset ) + "  (%x)" % frame.instructionOffset )
        dprintln("")


#    stackFrames = getCurrentStack()
	
#    [ dprintln( findSymbol( f.instructionOffset ) + "  (%x)" % f.instructionOffset )  for f in stackFrames ]


if __name__ == "__main__":

    if not isSessionStart():
        createSession()
        loadDump( sys.argv[1] )
        dprintln( sys.argv[1] + " - loaded OK" )

    printStack()

