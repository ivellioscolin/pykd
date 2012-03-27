
import sys
import re

from pykd import *

fwpsLayer = typeInfo( "FWPS_BUILTIN_LAYERS_" ).asMap()
fwpsDataType = typeInfo( "FWP_DATA_TYPE_" ).asMap()

layerRe = re.compile( 'LAYER' )
discardRe = re.compile( 'DISCARD' )

fwpsFields = {}

for layerId, v in fwpsLayer.items():
    if discardRe.search( v ):
        continue
    try:  
        fwpsFields[ layerId ] = typeInfo( layerRe.sub( 'FIELDS', v, 1 ) + '_' ).asMap()
    except:
        pass

def printBlob( blob ):
    bb = loadBytes( blob.data, blob.size )
    str = "\n"

    i = 0
    for b in bb:
        str += " %02x" % b  
        i = ( i + 1 ) % 16
        if i == 0: str += "\n"
    str += "\n"

    return str

def printFwpsValue( value ):
    return { 
        "FWP_UINT8"  : lambda : "%#x" % value.uint8,
        "FWP_UINT16" : lambda : "%#x" % value.uint16,
        "FWP_UINT32" : lambda : "%#x" % value.uint32,
        "FWP_UINT64" : lambda : "%#x" % value.uint64.deref(),
        "FWP_INT8"   : lambda : "%#x" % value.int8,
        "FWP_INT16"  : lambda : "%#x" % value.int16,
        "FWP_INT32"  : lambda : "%#x" % value.int32,
        "FWP_INT64"  : lambda : "%#x" % value.int64.deref(),
        "FWP_BYTE_BLOB_TYPE" : lambda : printBlob( value.byteBlob.deref() ),

    }.get( fwpsDataType[ value.type ], lambda : "---" )()

def wfpFixedValues( addr ):
    inFixedValue = typedVar( "FWPS_INCOMING_VALUES0_", addr )

    dprintln( " Layer: " + fwpsLayer[ inFixedValue.layerId ] )
    dprintln( " Value: %d" % inFixedValue.valueCount )

    values = [ x.value for x in typedVarArray( int(inFixedValue.incomingValue), "FWPS_INCOMING_VALUE0_", inFixedValue.valueCount ) ]

    for i in range( 0, len(values) ): 
        dprintln( "    " + fwpsFields[ inFixedValue.layerId ][ i ] )
        dprintln( "      Type: " + fwpsDataType[ values[i].type ] )
        dprintln( "      Value: " +  printFwpsValue( values[i] )  )

def usage():
    dprintln( "Usage:" )


def main():

    if not isKernelDebugging():
        dprintln( "This script is for kernel debugging only" )

    if len(sys.argv) < 2:
        usage()
        return 

    if sys.argv[1]=="/fixed":
        wfpFixedValues( expr(sys.argv[2]) )
        return

    if sys.argv[1]=="/meta":
        wfpMetaValues( expr(sys.argv[2]) )
        return

    usage()    
              
if __name__ == "__main__":
    main()