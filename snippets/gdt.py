#
#
#

import sys
from pykd import *


def printGdtEntry( addr ):

    dprintln( "GDT Entry: %x" % addr )
 
    attr = ptrByte( addr + 5 ) + ( ( ptrByte( addr + 6 ) & 0xF0 ) << 4 )
  
    limit = ptrWord( addr ) + ( ( ptrByte( addr + 6  ) & 0xF ) << 16 )
   
    base = ptrWord( addr + 2 ) + ( ptrByte( addr + 4) << 16 ) + ( ptrByte( addr + 7 ) << 24 )


    if attr & 0x10:
        pass
    else:
        if is64bitSystem():
            base = ( ptrDWord( addr + 8 ) << 32 ) + base   

    if attr & 0x800:
        limit = ( limit << 12 ) | 0xFFF


    dprint( "attr: %x ( " % attr  + "".join( [ ( attr & ( 1 << ( 11 - i ) ) ) and "1" or "0" for i in range(0,12) ] ) + " )" )
    dprint( "  base: %x" % base )        
    dprintln( "  limit: %x" % limit )


def printGdt( gdtr ):
    for s in ( "cs", "es", "ds", "ss", "gs", "fs", "tr" ):
        dprintln( s + " (%x):" % reg(s) )
        printGdtEntry( gdtr + ( reg( s ) & 0xFFF8 ) )
        dprintln("")              

    

def printGdtHelp():
 
    dprintln( "Usage:" )
    dprintln( "!py gdt help - Print out this message" )
    dprintln( "!py gdt x - Print out gdt entry. The gdt entry's base is got from gdtr" )
    dprintln( "!py gdt x y - Print out gdt entry. The gdt entry's base is x, y - offset" )


if __name__ == "__main__":

    if not isWindbgExt():
        print "script is launch out of windbg"
        quit( 0 )

    gdtr = reg("gdtr")

    if len( sys.argv)==1:
        printGdt( gdtr )

    elif sys.argv[1] == "help":
        printGdtHelp()      

    elif len( sys.argv )==2:
        printGdtEntry( gdtr + ( int( sys.argv[1], 16 ) & 0xFFF8 ) )

    else:
        printGdtEntry( int( sys.argv[1], 16 ) + ( int( sys.argv[2], 16 ) & 0xFFF8 ) )
