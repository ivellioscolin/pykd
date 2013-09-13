#
#
#

import sys
import pykd

from pykd import dprintln


def getFuncs():

    funcs = sorted( [ item for item in pykd.__dict__.values() if type(item).__name__ == "function" ], 
        key=lambda x: x.__name__ ) 

    return funcs

def getClasses():

    classes = sorted( [ item for item in pykd.__dict__.values() if type(item).__name__ == "class" ], 
        key=lambda x: x.__name__ ) 

    return classes
  


def printContent():

    dprintln( "\nPYKD API Help\n")


    funcs = getFuncs()

    dprintln( "\n<u>FUNCTIONS</u>\n", True )

    for f in funcs:
        dprintln( "<link cmd=\"!py help %s\">%s</link>" %( f.__name__, f.__name__ ), True )

    classes = getClasses()

    dprintln( "\n<u>CLASSES</u>\n", True )

    for c in classes:
        dprintln( "<link cmd=\"!py help %s\">%s</link>" %( c.__name__, c.__name__ ), True )


def printDetail( name ):
     
    if name in pykd.__dict__:
        help( "pykd.%s" % name )
   
    dprintln( "\n\n<link cmd=\"!py help\">View content</link>", True )
    


if __name__ == "__main__":
    if len(sys.argv)<=1:
        printContent()
    if len(sys.argv)==2:
        printDetail(sys.argv[1])
        
