from pykd import *
import sys


def checkSSDT():

   nt = loadModule( "nt" )
   nt.KeServiceDescriptorTable = getOffset( "nt", "KeServiceDescriptorTable" )

   serviceTableHeader = loadDWords( nt.KeServiceDescriptorTable, 4 )
   serviceTableStart = serviceTableHeader[0]
   serviceCount = serviceTableHeader[2]


   dprintln( "ServiceTable  start: %(1)x  count: %(2)x" % { "1" : serviceTableStart, "2" : serviceCount } )


   serviceTable = loadDWords( serviceTableStart, serviceCount ) 

 
   for i in range( 0, serviceCount ):
      dprintln( findSymbol( serviceTable[i] ) )       
         

if __name__ == "__main__":

   if not isSessionStart():
      createSession()
      loadDump( sys.argv[1] )

   checkSSDT()
	