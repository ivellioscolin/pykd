#
#
#

from optparse import OptionParser
from pykd import *

IPv4 = 0x0008
ARP = 0x0608
IPv6 = 0xdd86

ICMP_PROTO = 0x01
UDP_PROTO = 0x11
TCP_PROTO = 0x06


NET_BUFFER_LIST = None
MDL = None
NET_BUFFER = None

def getNdisTypesInfo():
    ndis = module("ndis")
    
    global NET_BUFFER_LIST
    global MDL
    global NET_BUFFER
    
    try:
        NET_BUFFER_LIST = ndis.type("_NET_BUFFER_LIST")
        MDL = ndis.type("_MDL")
        NET_BUFFER = ndis.type("_NET_BUFFER")
    except SymbolException:
        NET_BUFFER_LIST =typeInfo("_NET_BUFFER_LIST")
        MDL = typeInfo("_MDL")
        NET_BUFFER = typeInfo("_NET_BUFFER")  


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
        return "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x" % tuple( self.addr )


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

            version = dataPos.next()
            self.ihl = version & 0xF
            self.version = version >> 4
            self.tos = dataPos.next()
            self.TotalLength = getHostWord( dataPos )
            self.ident = getHostWord( dataPos )
            frag = getHostWord( dataPos )
            self.offset = frag & 0x1FFF
            self.flags = frag >> 13
            self.ttl = dataPos.next()
            self.protocol = IpProtocol( dataPos )
            self.checlsum = getNetWord( dataPos )
            self.srcAddr = IpAddress( dataPos )
            self.destAddr = IpAddress( dataPos )
            
            if  self.offset == 0:
                self.nextLayerPckt = self.protocol.getNextLayerPacket( dataPos )
            else:
                self.nextLayerPckt = ""
                
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
            s += "\tID: %x\n" % self.ident
            s += "\tflags: %x\n" % self.flags    
            s += "\toffset: %x" % ( self.offset * 8) 
            if ( self.offset == 0 ) and ( self.flags & 0x4 == 0 ):
                s += " (not fargmented)\n"
            elif  self.offset == 0 :
                s += " (first fragment)\n"
            elif not ( self.flags & 0x4 == 0 ):
                s += " (fragmented)\n"
            else:
                s += " (last fragment)\n"
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
           
           t = getHostDWord( dataPos )
           self.version = ( t >> 28 ) & 0xF
           self.trafficClass = ( t >> 20 ) & 0xFF
           self.flowLabel = t & 0xFFF
           self.payloadLength = getNetWord( dataPos )
           self.nextHeader = IpProtocol( dataPos )
           self.hopLimit = dataPos.next()
           self.srcAddr = Ip6Address( dataPos )
           self.destAddr = Ip6Address( dataPos ) 

           self.nextLayerPckt = self.nextHeader.getNextLayerPacket( dataPos )
           
           self.parsed = True     
        
       except StopIteration:
           pass
 
   def __str__( self ):

       s = "IPv6 header: "

       if self.parsed:
           s += "OK\n"
           s += "\tversion: %x\n" % self.version       
           s += "\ttraffic class %x\n" % self.trafficClass
           s += "\tflowLabel: %x\n" % self.flowLabel
           s += "\tpayloadLength: %x\n" % self.payloadLength
           s += "\tnextHeader: " + str( self.nextHeader ) + "\n"
           s += "\thopLimit: %d\n" % self.hopLimit
           s += "\tsrcAddr: " + str(self.srcAddr) + "\n"
           s += "\tdestAddr: " + str(self.destAddr) + "\n"
           s += str( self.nextLayerPckt )
       else:
           s += "MALFORMED\n"

       return s


class ARPPacket():

   def  __init__( self, dataPos ):
    
       self.parsed = False

       try:

           self.HWType = getNetWord( dataPos )
           self.PType = getNetWord( dataPos )
           self.HLen = dataPos.next()
           self.PLen = dataPos.next()
           self.oper = getNetWord( dataPos )
           self.senderHWAddr = EthernetAddress( dataPos )
           self.senderPAddr = IpAddress( dataPos )
           self.targetHWAddr = EthernetAddress( dataPos )
           self.targetPAddr = IpAddress( dataPos )

           self.parsed = True

       except StopIteration:
           pass

   def __str__( self ):
       s = "ARP Packet: "

       if self.parsed:
           s += "OK\n"
           s += { 0x100: "REQUEST", 0x200: "REPLAY" }.get(self.oper, hex(self.oper) ) + "\n"
           s += "HTYPE: " + { 0x100: "Ethernet", }.get( self.HWType, hex( self.HWType) ) + "  "
           s += "PTYPE: " + { IPv4: "IPv4", }.get( self.PType, hex( self.PType) ) + "  "
           s += "HLEN: %x  " % self.HLen
           s += "PLEN: %x  " % self.PLen
           s += "\nSender: " + str(self.senderHWAddr) + "  " + str( self.senderPAddr )
           s += "\nTarget: " + str(self.targetHWAddr) + "  " + str( self.targetPAddr ) + "\n"

       else:
           s += "MALFORMED\n"

       return s


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
        return { IPv4 : "IPv4", ARP : "ARP", IPv6 : "IPv6" }.get( self.typeVal, str(self.typeVal) )    

    def getNextLayerPacket( self, dataPos ):
        return {  
            IPv4 : lambda x : IpPacket(x),
            ARP : lambda x : ARPPacket(x), 
            IPv6 : lambda x : Ip6Packet(x), 
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

    def __init__( self, rawData, startProtocol="eth", beginOffset=0 ):
        self.rawData = rawData
        dataPos = iter( self.rawData[ beginOffset : ] )        
        
        self.mediaParsed = {
            "eth" : lambda : EthernetPacket( dataPos ),
            "ip4" : lambda : IpPacket( dataPos ),
            "ip6" : lambda : Ip6Packet( dataPos ),
            "tcp" : lambda : TcpPacket( dataPos ),
            "udp" : lambda : UdpPacket( dataPos )            
        }[startProtocol]()

    def __str__( self ):
        s = "Length: %d bytes\n" % len(self.rawData) 
        s += str( self.mediaParsed )
        return s            


def getPacketFromNb( nb ):

    pcktBytes = list()

    mdl = typedVar( MDL, nb.CurrentMdl )
    dataLength = nb.DataLength
    dataOffset = nb.CurrentMdlOffset

    while dataLength > 0:
 
        copyData = mdl.ByteCount - dataOffset
        if copyData > dataLength: copyData = dataLength
             
        pcktBytes.extend( loadBytes( mdl.MappedSystemVa + dataOffset, copyData ) )

        dataLength -= copyData

        mdl = typedVar( MDL, mdl.Next )

    return pcktBytes
    


def getPacketsFromNbl( nblAddr ):

    try:
        
        getNdisTypesInfo()

        pcktList = list()

        nbl = typedVar( NET_BUFFER_LIST, nblAddr )

        while True:
    
            nb = typedVar( NET_BUFFER, nbl.FirstNetBuffer )
      
            while True:

                pcktList.append( getPacketFromNb( nb ) )

                if nb.Next == 0:
                    break

                nb = typedVar( NET_BUFFER, nb.Next )

            if nbl.Next == 0:
                break

            nbl = typedVar( NET_BUFFER_LIST, nbl.Next )

        return pcktList 
        
    except TypeException:      

        dprintln( "the symbols ar wrong" )  

def printNblStruct( nblAddr, showNdisStruct = False, beginProtocol="eth", beginOffset=0 ):

    try:
    
        getNdisTypesInfo()    

        while nblAddr:
                     
            if showNdisStruct: 
                dprintln( "NET_BUFFER_LIST %#x" % nblAddr )

            nbl = typedVar( NET_BUFFER_LIST, nblAddr )

            nbAddr = nbl.FirstNetBuffer

            while nbAddr:
               
                nb = typedVar( NET_BUFFER, nbAddr )
               
                if showNdisStruct: 
                    dprint( "\tNET_BUFFER %#x" % nbAddr )
                    dprintln( "  data length = %d, data offset = %#x " % ( nb.DataLength, nb.DataOffset ) )

                mdlAddr = nb.CurrentMdl

                while mdlAddr:
               
                    mdl = typedVar( MDL, mdlAddr )

                    if showNdisStruct:
                        dprint( "\t\tMDL %#x" % mdlAddr )
                        dprintln( "  byte count = %d, byte offset = %#x, mapped addr = %#x" % ( mdl.ByteCount, mdl.ByteOffset, mdl.MappedSystemVa ) )

                    mdlAddr = mdl.Next

                dprintln( str( NetPacket( getPacketFromNb( nb ), beginProtocol, beginOffset ) ) )

                nbAddr = nb.Next  

            nblAddr = nbl.Next

    except TypeException:      

        dprintln( "the symbols ar wrong" )

def main():

    if not isKernelDebugging():
        dprintln( "This script is for kernel debugging only" )
        return
        
    parser = OptionParser(usage="usage: !py nbl [options] address")
    parser.add_option("-s", "--struct", action="store_true",  dest="showNdisStruct", default=False, help="Show NDIS structures" )  
    parser.add_option("-p", "--proto", action="store", type="string", dest="startProtocol", default="eth", help="Packet protocol. Can be eth, ip4, ip6, tcp, udp. By default - eth." )
    parser.add_option("-o", "--offset", action="store", type="long", dest="beginOffset", default=0, help="Bytes offset from packet begining" )
    parser.add_option("-r", "--raw", action="store_true", dest="rawBuffer", default=False, help="Show raw buffer")
   
    (options, args) = parser.parse_args()

    if len(args) < 1:
        parser.error("you should note address of network packet")

    if options.rawBuffer:

        if len(args) < 2:
            parser.error("you should note buffer length")
            return

        dprintln( str( NetPacket( rawData = loadBytes(expr(args[0]),expr(args[1])), startProtocol=options.startProtocol, beginOffset=options.beginOffset ) ) )

    else:
        printNblStruct( expr(args[0]), showNdisStruct = options.showNdisStruct, beginProtocol=options.startProtocol, beginOffset=options.beginOffset )
   

if __name__ == "__main__":
    main()