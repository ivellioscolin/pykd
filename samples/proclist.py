
import sys
from pykd import *


def loadSymbols():

   global nt
   nt = loadModule( "nt" )
   nt.PsActiveProcessHead = getOffset( "nt", "PsActiveProcessHead" )


def printStacks():

    processList = typedVarList( nt.PsActiveProcessHead, "nt", "_EPROCESS", "ActiveProcessLinks"  )

    for process in processList:
	dprintln( "".join( [ chr(i) for k, i in process.ImageFileName.items() ] ) )

    return


if __name__ == "__main__":

    if not isSessionStart():
        createSession()
        loadDump( sys.argv[1] )
        dprintln( sys.argv[1] + " - loaded OK" )

    loadSymbols()

    printStacks()
