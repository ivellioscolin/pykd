#
#
#

import sys
from pykd import *

def cr0( value = 0 ):

    if value == 0:
        value = reg( "cr0" )

    dprintln( "CR0: %x (" % value + "".join( [  (  value & ( 1 << ( 31 - i ) ) ) and "1" or "0" for i in range(0,32) ] ) + ")" )
    
    for i in range (0, 32):
        bits = { 0 : "PE", 1 : "MP", 2 : "EM", 3 : "TS", 4 : "ET", 5 : "NE", 16 : "WP", 18 : "AM", 29 : "NW", 30 : "CD", 31 : "PG"  }
        if ( ( 1 << ( 31 -i ) ) & value ) and 31-i in bits:
            dprint( " " + bits[31-i] )   
 
    dprintln("")                                          

   
if __name__ == "__main__":

   if not isWindbgExt():
       print "script is launch out of windbg"
       quit( 0 )

   if ( len( sys.argv ) > 1 ):
       cr0( int( sys.argv[1], 16 ) )
   else:
       cr0()
