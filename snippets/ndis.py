#
#
#

import sys
from pykd import *


def printBreakLine():

    dprintln( "\n=====================================================================================\n" )


def printNdisObj():
                   
    ndis=loadModule("ndis")

    ndisMajorVersion = ptrByte( ndis.NdisGetVersion + 1 )
    ndisMinorVersion = ptrByte( ndis.NdisGetVersion + 3 )

    mpList = typedVarList( ndis.ndisMiniportList, "ndis", "_NDIS_MINIPORT_BLOCK", "NextGlobalMiniport" )

    printBreakLine()

    for m in mpList: 

        dprintln( "<u>Adapter:</u>", True )
        
        dprintln( "%s\t<link cmd=\"dt ndis!_NDIS_MINIPORT_BLOCK %x\">NDIS_MINIPORT_BLOCK( %x )</link>" % ( loadUnicodeString(m.pAdapterInstanceName), m.getAddress(), m.getAddress() ), True )
        
        if ndisMajorVersion  >= 6: 

            lwf = m.LowestFilter

            if lwf != 0:
                dprintln( "\n<u>Light-Weight Filters:</u>", True )

            while lwf != 0:

                filt = typedVar( "ndis", "_NDIS_FILTER_BLOCK", lwf )

                dprintln( "%s\t<link cmd=\"dt ndis!_NDIS_FILTER_BLOCK %x\">NDIS_FILTER_BLOCK( %x )</link>" % ( loadUnicodeString(filt.FilterFriendlyName), filt.getAddress(), filt.getAddress() ), True )

                lwf = filt.HigherFilter     


            opn = m.OpenQueue

            if  opn != 0:
                dprintln( "\n<u>Bound protocols:</u>", True )

            while opn != 0:

                openBlock = typedVar( "ndis", "_NDIS_OPEN_BLOCK", opn )

                proto = typedVar( "ndis", "_NDIS_PROTOCOL_BLOCK", openBlock.ProtocolHandle )
                                                                                            
                dprint( "%s \t<link cmd=\"dt ndis!_NDIS_OPEN_BLOCK %x\">NDIS_OPEN_BLOCK( %x )</link>" % ( loadUnicodeString( proto.Name.getAddress() ), openBlock.getAddress(), openBlock.getAddress() ), True ) 
                dprintln( "\t<link cmd=\"dt ndis!_NDIS_PROTOCOL_BLOCK %x\">NDIS_PROTOCOL_BLOCK( %x )</link>" % ( proto.getAddress(), proto.getAddress() ), True ) 
           
                opn = openBlock.MiniportNextOpen
        else:

            opn = m.OpenQueue

            if  opn != 0:
                dprintln( "\n<u>Bound protocols:</u>", True )

            while opn != 0:

                openBlock = typedVar( "ndis", "_NDIS_OPEN_BLOCK", opn )

                proto = typedVar( "ndis", "_NDIS_PROTOCOL_BLOCK", openBlock.ProtocolHandle )
                                                                                            
                dprint( "%s \t<link cmd=\"dt ndis!_NDIS_OPEN_BLOCK %x\">NDIS_OPEN_BLOCK( %x )</link>" % ( loadUnicodeString( proto.ProtocolCharacteristics.Name.getAddress() ), openBlock.getAddress(), openBlock.getAddress() ), True ) 
                dprintln( "\t<link cmd=\"dt ndis!_NDIS_PROTOCOL_BLOCK %x\">NDIS_PROTOCOL_BLOCK( %x )</link>" % ( proto.getAddress(), proto.getAddress() ), True ) 
           
                opn = openBlock.MiniportNextOpen     


        printBreakLine()

if __name__ == "__main__":

    if not isWindbgExt():
        dprintln( "script is launch out of windbg" )
        quit(0)

    if not isKernelDebugging:
        dprintln( "script for kernel mode only" )
        quit(0)


    printNdisObj()    


                                       
    
    
  
         

