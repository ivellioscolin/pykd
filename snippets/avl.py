#
#
#

import sys
import re

from pykd import isWindbgExt
from pykd import dprintln
from pykd import expr
from pykd import typedVar
from pykd import addr64
from pykd import sizeof


def addTableChilds( table, links ):
   
    table.append( links.getAddress() + sizeof( "nt", "_RTL_BALANCED_LINKS" ) )

    if links.LeftChild != 0:
        addTableChilds( table, typedVar("nt", "_RTL_BALANCED_LINKS", links.LeftChild) )
    if links.RightChild != 0:
        addTableChilds( table, typedVar("nt", "_RTL_BALANCED_LINKS", links.RightChild) )       


def getAVLTable( addr ):

    table = []
    avl = typedVar( "nt", "_RTL_AVL_TABLE", addr )
    addTableChilds( table, avl.BalancedRoot )
    return table


def printUsage():
    dprintln( "!py avl [addr] (type)")


if __name__ == "__main__":

    if not isWindbgExt():
        print "this script should be run within windbg"
        quit(0)

    if len( sys.argv ) < 2:
        printUsage()
        quit(0)

    if len( sys.argv ) == 2:
        items = getAVLTable( addr64( expr( sys.argv[1] ) ) )
        dprintln( "\n".join( [ "<link cmd=\"db 0x%x\">db 0x%x</link>" % (  entry, entry ) for entry in items ] ), True )
    else:
        items = getAVLTable( addr64( expr( sys.argv[1] ) ) )
        dprintln( "\n".join( [ "<link cmd=\"dt %s 0x%x\">dt %s</link>" % ( sys.argv[2], entry, sys.argv[2] ) for entry in items ] ), True )
 


