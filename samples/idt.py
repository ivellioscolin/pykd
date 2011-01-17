from pykd import *
import sys

def checkInterrupt():


    if not is64bitSystem():

        dprintln( "check interrupt handlers...\n" )	

        idtr = reg( "idtr" )

        nt = loadModule( "nt" )
        hal = loadModule( "hal" )

        ErrorCount = 0

        for i in range(0,255):

          idtEntry = typedVar( "nt", "_KIDTENTRY", idtr + i*8 )

          if idtEntry.Selector == 8:

              InterruptHandler = ( idtEntry.ExtendedOffset * 0x10000 ) + idtEntry.Offset

              if InterruptHandler != 0 and not nt.contain( InterruptHandler ) and not hal.contain( InterruptHandler ):

                   kinterrupt = containingRecord( InterruptHandler, "nt", "_KINTERRUPT", "DispatchCode" )
                 
                   dprintln ( "KINTERRUPT: %(1)x" % { "1" : kinterrupt.getAddress() } )

                   if addr64( kinterrupt.DispatchAddress ) != nt.KiInterruptDispatch and addr64(  kinterrupt.DispatchAddress ) != nt.KiChainedDispatch:
                         dprintln ( "Threat!!! KINTERRUPT::DispatchAddress PATCHED" )
                         ErrorCount += 1

                   if findModule( kinterrupt.ServiceRoutine ) == None:
                         dprintln ( "Threat!!! KINTERRUPT::ServiceRoutine (%(1)x) out of  any module" % { "1" : kinterrupt.ServiceRoutine } ) 
                         ErrorCount += 1

                   if not compareMemory( nt.KiInterruptTemplate, InterruptHandler, 98 ):
                         dprintln ( "Threat!!! KINTERRUPT::DispatchCode area PATCHED" )
                         ErrorCount += 1

                   dprintln ( "" )
           
        dprintln( "check end: %(1)d threats" % { "1" : ErrorCount } )

    else:

       dprintln( "x64 is not supported" )


if __name__ == "__main__":

   if not isSessionStart():
      createSession()
      loadDump( sys.argv[1] )

   checkInterrupt()




   

