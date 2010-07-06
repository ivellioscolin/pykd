from pykd import *

s = dbgCommand( "dt nt!_DRIVER_OBJECT" )

dprintln( s )