
import sys
import os.path

from pykd import dprintln
from pykd import dprint

def printAllSamples():
    dprintln( "<b>Kernel mode</b>", True )
    dprintln( "Get process list <link cmd=\"!py samples run km.proclist\">Run</link> <link cmd=\"!py samples source km.proclist\">Source</link>", True)
    dprintln( "Get kernel service list <link cmd=\"!py samples run km.ssdt\">Run</link> <link cmd=\"!py samples source km.ssdt\">Source</link>", True)
    dprintln( "Get driver object <link cmd=\"!py samples run km.drvobj\">Run</link> <link cmd=\"!py samples source km.drvobj\">Source</link>", True)
    dprintln( "" )

def runSample( sampleName ):

    try:
        packageName, moduleName = sampleName.split(".")

        module = __import__( name = sampleName, fromlist = moduleName  )

        module.__dict__[ "run" ]()

    except ImportError:
        dprintln("import error")
        pass

    dprintln( "" )
    dprintln( "<link cmd=\"!py samples\">Sample list</link>", True )
    dprintln( "" )  

def printSample( sampleName ):

    try:
        packageName, moduleName = sampleName.split(".")

        module = __import__( name = sampleName, fromlist = moduleName  )

        fileName = os.path.dirname( module.__dict__["__file__"] )
        fileName = os.path.join( fileName, moduleName + ".py" )

        with open( fileName ) as f:
            for line in f:
                dprint( line )            

    except ImportError:
        dprintln("import error")
        pass

    dprintln( "" )
    dprintln( "<link cmd=\"!py samples\">Sample list</link>", True )
    dprintln( "" )  
  

def main():
    if len(sys.argv) <= 2:
        return printAllSamples()

    if sys.argv[1] == "run":
        runSample( sys.argv[2] )

    if sys.argv[1] == "source":
        printSample( sys.argv[2] )
        

if __name__ == "__main__":
    main()




