
import sys
import re

from pykd import *

fwpsLayer = dict( [ (long(val), key) for key, val in typeInfo( "FWPS_BUILTIN_LAYERS_" ).fields() ] )
fwpsDataType = dict( [ (long(val), key)  for key, val in typeInfo( "FWP_DATA_TYPE_" ).fields() ] )
fwpDirection = dict( [ (long(val), key) for key, val in typeInfo( "FWP_DIRECTION_" ).fields() ] )

def printBlob( blob ):
    bb = loadBytes( blob.data, blob.size )
    str = "\n"

    i = 0
    for b in bb:
        str += " %02x" % b  
        i = ( i + 1 ) % 16
        if i == 0: str += "\n"
    str += "\n"

    str += "As string: " + loadWStr(blob.data )
    str += "\n"  

    return str

def printArray16( array16 ):
    return " ".join( [ "%02x"%v for v in array16.byteArray16 ] )

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
        "FWP_BYTE_ARRAY16_TYPE" : lambda : printArray16( value.byteArray16.deref() )

    }.get( fwpsDataType[ value.field("type") ], lambda : "---" )()

def wfpFixedValues( addr ):
  
    dprintln( "FWPS_INCOMING_VALUES0:" )

    inFixedValue = typedVar( "FWPS_INCOMING_VALUES0_", addr )
    
    dprintln( " Layer: " + fwpsLayer[ inFixedValue.layerId ] )
    dprintln( " Value: %d" % inFixedValue.valueCount )

    values = [ x.value for x in typedVarArray( int(inFixedValue.incomingValue), "FWPS_INCOMING_VALUE0_", inFixedValue.valueCount ) ]

    layerName = fwpsLayer[ inFixedValue.layerId ]

    discardRe = re.compile( '_DISCARD' )
    layerName = discardRe.sub( '', layerName, 1 )

    layerRe = re.compile( 'LAYER' )
    fwpsFields = typeInfo( layerRe.sub( 'FIELDS', layerName, 1 ) + '_' ).fields()

    for i in xrange( min(len(fwpsFields),len(values)) ):
        dprintln( "    " + fwpsFields[i][0] )
        dprintln( "      Type: " + fwpsDataType[ values[i].field("type") ] )
        dprintln( "      Value: " +  printFwpsValue( values[i] )  )

def printDiscardReason( discardReason ):
    return ""

def printBlobAsStr( blob ):
    return loadWChars( blob.data, blob.size )

def printFwpsMetaValue( valueIndex, inMetaValues ):

   return {
        0x00000001 : lambda x: printDiscardReason( x.discardMetadata ),
        0x00000002 : lambda x: "%#x" % inMetaValues.flowHandle,
        0x00000004 : lambda x: "%#x" % inMetaValues.ipHeaderSize,
        0x00000008 : lambda x: printBlobAsStr( x.processPath.deref() ),
        0x00000010 : lambda x: "%#lx" % inMetaValues.token,
        0x00000020 : lambda x: "%#lx" % inMetaValues.processId,
        0x00000040 : lambda x: "%#x" % inMetaValues.flags,
        0x00000080 : lambda x: "%#lx" % inMetaValues.reserved,
        0x00000100 : lambda x: "%#x" % inMetaValues.sourceInterfaceIndex,
        0x00000200 : lambda x: "%#x" % inMetaValues.destinationInterfaceIndex,
        0x00000400 : lambda x: "%#x" % inMetaValues.transportHeaderSize,
        0x00000800 : lambda x: "%#x" % inMetaValues.compartmentId,
        0x00001000 : lambda x: "id: %x  offset: %x  length: %x" % ( x.fragmentMetadata.fragmentIdentification, x.fragmentMetadata.fragmentOffset, x.fragmentMetadata.fragmentLength ),
        0x00002000 : lambda x: "%#x" % x.pathMtu,
        0x00004000 : lambda x: "%#lx" % x.completionHandle,
        0x00008000 : lambda x: "%#lx" % x.transportEndpointHandle,
        0x00010000 : lambda x: "Data: %#lx, Length: %#x" % ( x.controlData, x.controlDataLength ),
        0x00020000 : lambda x: "Zone: %d Level: %d" % ( x.remoteScopeId.Zone, x.remoteScopeId.Level ),
        0x00040000 : lambda x: fwpDirection[ x.packetDirection ],
    }.get( valueIndex, lambda x: "" )( inMetaValues )


def wfpMetaValues( addr ):

    dprintln( "FWPS_INCOMING_METADATA_VALUES0:" )

    fwpsMetadataFields = {
        0x00000001 : "FWPS_METADATA_FIELD_DISCARD_REASON",
        0x00000002 : "FWPS_METADATA_FIELD_FLOW_HANDLE",
        0x00000004 : "FWPS_METADATA_FIELD_IP_HEADER_SIZE",
        0x00000008 : "FWPS_METADATA_FIELD_PROCESS_PATH",
        0x00000010 : "FWPS_METADATA_FIELD_TOKEN",
        0x00000020 : "FWPS_METADATA_FIELD_PROCESS_ID",
        0x00000040 : "FWPS_METADATA_FIELD_SYSTEM_FLAGS",
        0x00000080 : "FWPS_METADATA_FIELD_RESERVED",
        0x00000100 : "FWPS_METADATA_FIELD_SOURCE_INTERFACE_INDEX",
        0x00000200 : "FWPS_METADATA_FIELD_DESTINATION_INTERFACE_INDEX",
        0x00000400 : "FWPS_METADATA_FIELD_TRANSPORT_HEADER_SIZE",
        0x00000800 : "FWPS_METADATA_FIELD_COMPARTMENT_ID",
        0x00001000 : "FWPS_METADATA_FIELD_FRAGMENT_DATA",
        0x00002000 : "FWPS_METADATA_FIELD_PATH_MTU",
        0x00004000 : "FWPS_METADATA_FIELD_COMPLETION_HANDLE",
        0x00008000 : "FWPS_METADATA_FIELD_TRANSPORT_ENDPOINT_HANDLE",
        0x00010000 : "FWPS_METADATA_FIELD_TRANSPORT_CONTROL_DATA",
        0x00020000 : "FWPS_METADATA_FIELD_REMOTE_SCOPE_ID",
        0x00040000 : "FWPS_METADATA_FIELD_PACKET_DIRECTION",
        0x00080000 : "FWPS_METADATA_FIELD_PACKET_SYSTEM_CRITICAL",
        0x00100000 : "FWPS_METADATA_FIELD_FORWARD_LAYER_OUTBOUND_PASS_THRU",
        0x00200000 : "FWPS_METADATA_FIELD_FORWARD_LAYER_INBOUND_PASS_THRU",
        0x00400000 : "FWPS_METADATA_FIELD_ALE_CLASSIFY_REQUIRED",
        0x00800000 : "FWPS_METADATA_FIELD_TRANSPORT_HEADER_INCLUDE_HEADER",
        0x01000000 : "FWPS_METADATA_FIELD_DESTINATION_PREFIX",
        0x02000000 : "FWPS_METADATA_FIELD_ETHER_FRAME_LENGTH",
        0x04000000 : "FWPS_METADATA_FIELD_PARENT_ENDPOINT_HANDLE",
        0x08000000 : "FWPS_METADATA_FIELD_ICMP_ID_AND_SEQUENCE",
        0x10000000 : "FWPS_METADATA_FIELD_LOCAL_REDIRECT_TARGET_PID",
        0x20000000 : "FWPS_METADATA_FIELD_ORIGINAL_DESTINATION",
        0x40000000 : "FWPS_METADATA_FIELD_REDIRECT_RECORD_HANDLE",
        0x80000000 : "FWPS_METADATA_FIELD_SUB_PROCESS_TAG"
    }

    inMetaValues = typedVar( "FWPS_INCOMING_METADATA_VALUES0_", addr )

    for i in ( 1 << i for i in range( 0, 32) ):
        if inMetaValues.currentMetadataValues & i:
            dprint( "    " )
            dprint( fwpsMetadataFields.get( i, "Unknown filed %#010x" % i ) + ": " )
            dprint( printFwpsMetaValue( i, inMetaValues ) )
            dprintln("")
  

def usage():
    dprintln( "Usage:" )
    dprintln( "!py wfp /fixed addr")
    dprintln( "!py wfp /meta addr" )

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