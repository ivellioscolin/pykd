#
#
#

import sys
import re

from pykd import isWindbgExt
from pykd import dprintln
from pykd import expr
from pykd import typedVar
from pykd import typeInfo
from pykd import addr64
from pykd import sizeof


def addTableChilds( table, links ):
   
    table.append( links.getAddress() + sizeof( "nt!_RTL_BALANCED_LINKS" ) )

    if links.LeftChild != 0:
        addTableChilds( table, typedVar("nt!_RTL_BALANCED_LINKS", links.LeftChild) )
    if links.RightChild != 0:
        addTableChilds( table, typedVar("nt!_RTL_BALANCED_LINKS", links.RightChild) )       


def getAVLTable( addr ):

    table = []
    avl = typedVar( "nt!_RTL_AVL_TABLE", addr )
    addTableChilds( table, avl.BalancedRoot )
    return table


def printUsage():
    dprintln( "!py avl [addr] (type)")


if __name__ == "__main__":

    if len( sys.argv ) < 2:
        printUsage()
        quit(0)
   
    showAll = False
    args = sys.argv
    if '-a' in args:
        args.remove('-a')
        showAll = True
        
    items = getAVLTable( addr64( expr( sys.argv[1] ) ) )       
        
    if showAll:
        if len( sys.argv ) == 2:
            dprintln( "\n".join( [ "<link cmd=\"db 0x%x\">db 0x%x</link>" % (  entry, entry ) for entry in items ] ), True )
        else:
            ti = typeInfo(sys.argv[2])
            dprintln( "\n".join( [ "<link cmd=\"dt %s 0x%x\">dt %s</link>\n%s" % ( sys.argv[2], entry, sys.argv[2], typedVar(ti, entry) ) for entry in items ] ), True )  
    
    else:
        if len( sys.argv ) == 2:
            dprintln( "\n".join( [ "<link cmd=\"db 0x%x\">db 0x%x</link>" % (  entry, entry ) for entry in items ] ), True )
        else:
            dprintln( "\n".join( [ "<link cmd=\"dt %s 0x%x\">dt %s</link>" % ( sys.argv[2], entry, sys.argv[2] ) for entry in items ] ), True )
 


