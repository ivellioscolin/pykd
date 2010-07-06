from pykd import *

dprintln( "type test begin" )

idtr=reg("idtr")

var = typedVar( "nt", "_KIDTENTRY", idtr )

for t, v in var.iteritems():
  dprintln( t + " : " + str(v) )


dprintln( "type test end" )