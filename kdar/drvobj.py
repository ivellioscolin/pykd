from pykd import *


drvObj = typedVar( "nt", "_DRIVER_OBJECT", addr64( 0x82000c08 ) )

dprintln( "DriverName.Length = %(1)d" % { "1" : drvObj.DriverName.Length } )

for i,f in drvObj.MajorFunction.iteritems():

	dprintln( "MajorFunction[%(1)d] = " % { "1" : i } + findSymbol( addr64( f ) ) )

