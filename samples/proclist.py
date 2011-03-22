
import sys
from pykd import *


def loadSymbols():

   global nt
   nt = loadModule( "nt" )


def processInfo():

    processList = typedVarList( nt.PsActiveProcessHead, "nt", "_EPROCESS", "ActiveProcessLinks"  )

    for process in processList:
      print "".join( [chr(i) for i in process.ImageFileName if i != 0] )



if __name__ == "__main__":

    if not isSessionStart():
        createSession()
        loadDump( sys.argv[1] )
        dprintln( sys.argv[1] + " - loaded OK" )

    loadSymbols()

    processInfo()
