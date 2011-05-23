
import sys
from pykd import *


def processInfo():

    nt = loadModule( "nt" )

    processList = typedVarList( nt.PsActiveProcessHead, "nt", "_EPROCESS", "ActiveProcessLinks"  )

    for process in processList:
        print "".join( [chr(i) for i in process.ImageFileName if i != 0] )


def main():

    if not isWindbgExt():
        if not loadDump( sys.argv[1] ):
             dprintln( sys.argv[1] + " - load failed" )
             return

    if not isKernelDebugging():
        dprintln( "not a kernel debugging" )
        return   
                 
    processInfo()

	
if __name__ == "__main__":
    main()


