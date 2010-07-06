from pykd import *


dprintln( "regs tests begin" )

al = reg("al")
ax = reg("ax")
eax = reg("eax")


dprintln( "al: " + str(al) )
dprintln( "ax: " + str(ax) )
dprintln( "eax: " + str(eax) )


dprintln( "regs tests end" )