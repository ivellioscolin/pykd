#
#
#

import sys
from pykd import *

def cr4( value = 0 ):

    if value == 0:
        value = reg( "cr4" )

    dprintln( "CR4: %x (" % value + "".join( [  (  value & ( 1 << ( 31 - i ) ) ) and "1" or "0" for i in range(0,32) ] ) + ")" )
    
    for i in range (0, 32):
        bits = { 0 : "VME", 1 : "PVI", 2 : "TSD",  3 : "DE", 4 : "PSE", 5 : "PAE",  6 : "MCE",  7 : "PGE",  8 : "PCE",  9 : "OSFXSR", 10 : "OSXMMEXCPT",   13 : "VMXE",  14 : "SMXE", 17 : "PCIDE", 18 : "OSXSAVE"  }
        if ( ( 1 << ( 31 -i ) ) & value ) and 31-i in bits:
            dprint( " " + bits[31-i] )   
 
    dprintln("")                                          

   
if __name__ == "__main__":

   if not isWindbgExt():
       print "script is launch out of windbg"
       quit( 0 )

   if len(sys.argv) > 1:
       cr4( int( sys.argv[1], 16 ) )
   else:
       cr4()
    


