from pykd import *
import sys

def checkSSDT():

   nt = loadModule( "nt" )

   if is64bitSystem():   

        def getServiceAddrWlh(Start, Offset):
            return Start + (Offset / 16)

        def getServiceAddr2k3(Start, Offset):
            return Start + (Offset & ~0xf)

        if (ptrWord(nt.NtBuildNumber) == 3790):
            getServiceAddr = getServiceAddr2k3
        else:
            getServiceAddr = getServiceAddrWlh

        serviceTableHeader = loadQWords( nt.KeServiceDescriptorTable, 4 )       
        serviceTableStart = serviceTableHeader[0]
        serviceCount = serviceTableHeader[2]

        dprintln( "ServiceTable  start: %(1)x  count: %(2)x" % { "1" : serviceTableStart, "2" : serviceCount } )

        serviceTable = loadSignDWords( serviceTableStart, serviceCount ) 

        for i in range( 0, serviceCount ):
            routineAddress = getServiceAddr(serviceTableStart, serviceTable[i]);
            dprintln( "[%u] " % i + findSymbol( routineAddress ) )

   else:

       serviceTableHeader = loadDWords( nt.KeServiceDescriptorTable, 4 )
       serviceTableStart = serviceTableHeader[0]
       serviceCount = serviceTableHeader[2]

       dprintln( "ServiceTable  start: %(1)x  count: %(2)x" % { "1" : serviceTableStart, "2" : serviceCount } )

       serviceTable = loadPtrs( serviceTableStart, serviceCount ) 

       for i in range( 0, serviceCount ):
          dprintln( "[%u] " % i + findSymbol( serviceTable[i] ) )       

def run():
    checkSSDT()
        

if __name__ == "__main__":

    while True:

        if not isWindbgExt():
            if not loadDump( sys.argv[1] ):
                dprintln( sys.argv[1] + " - load failed" )
                break

        if not isKernelDebugging():
            dprintln( "not a kernel debugging" )
            break 
        
        run()         

        break
