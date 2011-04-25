#
#
#

import sys
from pykd import *

IPv4 = 0x0008
ARP = 0x0608
IPv6 = 0xdd86

ICMP_PROTO = 0x01
UDP_PROTO = 0x11
TCP_PROTO = 0x06


def getHostWord( dataPos ):
    return ( dataPos.next() << 8 ) + dataPos.next()   


def getNetWord( dataPos ):
    return dataPos.next() + ( dataPos.next() << 8 )


def getHostDWord( dataPos ):
    return ( dataPos.next() << 24 ) + ( dataPos.next() << 16 ) + ( dataPos.next() << 8 ) + dataPos.next()


def getNetDWord( dataPos ):
    return dataPos.next() + ( dataPos.next() << 8 ) + ( dataPos.next() << 16 ) + ( dataPos.next() << 24 )



class UdpPacket:

    def __init__( self, dataPos ):

        self.parsed = False

        try:
            self.sourcePort = getHostWord( dataPos )
            self.destPort = getHostWord( dataPos )
            self.length = getHostWord( dataPos )
            self.checksum = getHostWord( dataPos )             
            self.parsed = True

        except StopIteration:
            pass                   

    def __str__( self ):
        s = "UDP header: "
        if self.parsed:
            s += "OK\n"
            s += "\tSrc port: %d\n" % self.sourcePort
            s += "\tDest port: %d\n" % self.destPort
            s += "\tLength: %d\n" % self.length
            s += "\tChecksum: %#x\n" % self.checksum
            s += "\n"
        else:
            s += "MALFORMED\n"

        return s


class TcpPacket:
    
    def __init__( self, dataPos ):

        self.parsed = False

        try:
            self.parsed = True
            self.sourcePort = getHostWord( dataPos )
            self.destPort = getHostWord( dataPos )
            self.SeqNumber = getHostDWord( dataPos )
            self.AckNumber = getHostDWord( dataPos )
            self.dataOffset = ( dataPos.next() >> 4 )
            self.flags = dataPos.next() & 0x3F
            self.window = getHostWord( dataPos )
            self.checksum = getHostWord( dataPos )              
            self.urgentPointer = getHostWord( dataPos )     


        except StopIteration:
            pass

    def __str__( self ):

        s = "TCP header: "
        fl = [ "FIN", "SYN","RST", "PSH", "ACK", "URG" ]

        if self.parsed:
            s += "OK\n"
            s += "\tSrc port: %d\n" % self.sourcePort
            s += "\tDest port: %d\n" % self.destPort
            s += "\tSEQ: %x\n" % self.SeqNumber
            s += "\tACK: %x\n" % self.AckNumber
            s += "\tFlags: %x ( %s )\n" % ( self.flags, " ".join( [ fl[i] for i in xrange( len(fl) ) if ( self.flags & ( 1 << i ) ) != 0 ] ) ) 
            s += "\tWindows: %x\n" % self.window 
            s += "\tChecksum: %x\n" % self.checksum

        else:
            s += "MALFORMED\n"

        return s            


class ArpPacket:

    def __init__( self, dataPos ):
        pass
    
    def __str__( self ):
        return ""


class IpAddress:

    def __init__( self, dataPos ):

        self.addr = [ dataPos.next() for i in xrange(4) ]

    def __str__( self ):
        
        return "%d.%d.%d.%d" % tuple( self.addr[0:4] ) 


class Ip6Address:
   
    def __init__( self, dataPos ):
        self.addr = [ getHostWord( dataPos ) for i in xrange(8) ] 

    def __str__( self ):
        return "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x" % tuple( self.addr[0:8] )


class IpProtocol:

    def __init__( self, dataPos ):
        self.typeVal = dataPos.next()

    def isICMP( self ):
        return self.typeVal==ICMP_PROTO

    def isUDP( self ):
        return self.typeVal==UDP_PROTO

    def isTCP( self ):
        return self.typeVal==TCP_PROTO

    def __str__( self ):
        return { ICMP_PROTO: "ICMP", UDP_PROTO: "UDP", TCP_PROTO: "TCP" }.get( self.typeVal, hex(self.typeVal) )


    def getNextLayerPacket( self, dataPos ):
        return { 
		ICMP_PROTO : lambda x : "",
		UDP_PROTO : lambda x : UdpPacket(x),
                TCP_PROTO : lambda x : TcpPacket(x)
            }.get( self.typeVal, lambda x : "Unknown protocol" )(dataPos)


class IpPacket:

    def __init__( self, dataPos ):

        self.parsed = False

        try:                                

            self.version = dataPos.next()
            self.ihl = self.version & 0xF
            self.version = self.version >> 4
            self.tos = dataPos.next()
            self.TotalLength = getHostWord( dataPos ) 
            self.ident = getNetWord( dataPos )
            self.fargment = getNetWord( dataPos )
            self.ttl = dataPos.next()
            self.protocol = IpProtocol( dataPos )
            self.checlsum = getNetWord( dataPos )
            self.srcAddr = IpAddress( dataPos )
            self.destAddr = IpAddress( dataPos )
            self.nextLayerPckt = self.protocol.getNextLayerPacket( dataPos )
            self.parsed = True

        except StopIteration:
             pass



    def __str__( self ):

        s = "IPv4 header: "

        if self.parsed:
            s += "OK\n" 
            s += "\tversion: %x\n" % self.version
            s += "\theader length: %d bytes\n" % ( self.ihl * 4 )
            s += "\ttotal length: %d bytes\n" % self.TotalLength
            s += "\tprotocol: " + str( self.protocol ) + "\n"
            s += "\tTTL: %d\n" % self.ttl
            s += "\tSrc addr: " + str(self.srcAddr) + "\n"
            s += "\tDest addr: " + str(self.destAddr) + "\n"
            s += str( self.nextLayerPckt )

        else:
            s += "MALFORMED\n"

        return s


class Ip6Packet():

   def  __init__( self, dataPos ):
  
       self.parsed = False
        
       try:
           
           t = getNetDWord( dataPos )
           self.version = ( t >> 28 ) & 0xF
           self.trafficClass = ( t >> 20 ) & 0xFF
           self.flowLabel = t & 0xFFF
           self.payloadLength = getNetWord( dataPos )
           self.nextHeader = dataPos.next()
           self.hopLimit = dataPos.next()
           self.srcAddr = Ip6Address( dataPos )
           self.destAddr = Ip6Address( dataPos ) 

           self.parsed = True     
        
       except StopIteration:
           pass
 
   def __str__( self ):

       s = "IPv6 header"

       if self.parsed:
           s += "OK\n"
           s += "\tversion: %x\n" % self.version       
           s += "\ttraffic class %x\n" % self.trafficClass
           s += "\tflowLabel: %x\n" % self.flowLabel
           s += "\tpayloadLength: %x\n" % self.payloadLength
           s += "\tnextHeader: %x\n" % self.nextHeader
           s += "\thopLimit: %d\n" % self.hopLimit
           s += "\tsrcAddr: " + str(self.srcAddr) + "\n"
           s += "\tdestAddr: " + str(self.destAddr) + "\n"
       else:
           s += "MALFORMED\n"

       return ""


class ARPPacket():

   def  __init__( self, dataPos ):
       pass

   def __str__( self ):
       return ""


class EthernetType:

    def __init__( self, dataPos ):
        self.typeVal = getNetWord( dataPos )

    def isIPv4( self ):
        return self.typeVal == IPv4

    def isARP( self ):
        return self.typeVal == ARP

    def isIPv6( self ):
        return self.typeVal == IPv6

    def __str__( self ):
        return { IPv4 : "IPv4", ARP : "ARP", IPv6 : "IPv6" }.get( self.typeVal, self.typeVal )    

    def getNextLayerPacket( self, dataPos ):
        return {  
            IPv4 : lambda x : IpPacket(x),
            ARP : lambda x : Ip6Packet(x), 
            IPv6 : lambda x : ARPPacket(x), 
        }.get( self.typeVal, lambda x : "" )( dataPos )


class EthernetAddress:

    def __init__( self, dataPos ):
        self.addr = [ dataPos.next() for i in range(0,6) ]

    def __str__( self ):      
        return "%02x-%02x-%02x-%02x-%02x-%02x" % tuple( self.addr[0:6] )       
    


class EthernetPacket:

    def __init__( self, dataPos ):

        self.parsed = False

        try:

            self.destAddress = EthernetAddress( dataPos)
            self.srcAddress = EthernetAddress( dataPos)
            self.frametype = EthernetType( dataPos )
            self.nextLayerPckt = self.frametype.getNextLayerPacket( dataPos )
            self.parsed = True

        except StopIteration:
            pass


    def __str__( self):

        s = "Ethernet header: "
         
        if self.parsed:
         
            s += "OK\n"
            s += "\tDest MAC: " + str(self.destAddress) + "\n"
            s += "\tSrc MAC: " + str(self.srcAddress) + "\n"
            s += "\tType: " + str( self.frametype) + "\n"
            s += str( self.nextLayerPckt )

        else:
            s += "FAILED\n"

        return s


class NetPacket:

    def __init__( self, rawData ):
        self.rawData = rawData
        dataPos = iter( self.rawData )
        self.mediaParsed = EthernetPacket( dataPos )

    def __str__( self ):
        s = "Length: %d bytes\n" % len(self.rawData) 
        s += str( self.mediaParsed )
        return s            


def getPacketsFromNbl( nblAddr ):

    pcktList = list()

    nbl = typedVar( "ndis", "_NET_BUFFER_LIST", nblAddr )

    while nbl:
    
        nb = typedVar( "ndis", "_NET_BUFFER", nbl.FirstNetBuffer )
      
        while nb:

            pcktBytes = list()

            mdl = typedVar( "ndis", "_MDL", nb.CurrentMdl )
            dataLength = nb.DataLength
            dataOffset = nb.CurrentMdlOffset

            while dataLength > 0:
 
                copyData = mdl.ByteCount - dataOffset
                if copyData > dataLength: copyData = dataLength
             
                pcktBytes.extend( loadBytes( mdl.MappedSystemVa + dataOffset, copyData ) )

                dataLength -= copyData

                mdl = typedVar( "ndis", "_MDL", mdl.Next )

            pcktList.append( pcktBytes )

            nb = typedVar( "ndis", "_NET_BUFFER", nb.Next )

        nbl = typedVar( "ndis", "_NET_BUFFER_LIST", nbl.Next )

    return pcktList  


def main():

    if len(sys.argv) < 2:
        return
   
    if not isKernelDebugging():
        dprintln( "This script is for kernel debugging only" )
        return


    pcktList = getPacketsFromNbl( expr(sys.argv[1]) )

    parsedPcktList = [ NetPacket(p) for p in pcktList ]

    
    print "Packet's count: ", len(parsedPcktList)

    for p in parsedPcktList: print "\n", p



if __name__ == "__main__":
    main()