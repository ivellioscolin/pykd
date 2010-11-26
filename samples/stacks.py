import sys
from pykd import *


def printStack():

    threadList = getThreadList()

    for threadPtr in threadList:
        oldThread = 0
        setImplicitThread( threadPtr )
        stackFrames = getCurrentStack()
        for frame in stackFrames: dprintln( findSymbol( frame.instructionOffset ) + "  (%x)" % frame.instructionOffset )
        dprintln("")


if __name__ == "__main__":

    if not isSessionStart():
        createSession()
        loadDump( sys.argv[1] )
        dprintln( sys.argv[1] + " - loaded OK" )

    printStack()

