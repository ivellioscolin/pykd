
#
#
#

from pykd import *
import sys


def vmcsGetQword( vmcs, offset ):

    a = 0

    for i in range( 0, 8 ):	
        a = a * 0x100 + vmcs[ offset + 7 - i ]

    return a				


def vmcsGetDword( vmcs, offset ):

    a = 0

    for i in range( 0, 4 ):	
        a = a * 0x100 + vmcs[ offset + 3 - i ]

    return a				


def vmcsGetWord( vmcs, offset ):

    return vmcs[ offset + 1] * 0x100 + vmcs[ offset ]



def vmcsGetExitReason( reason ):

    vmexits = { \
        0 : "Exception or non-maskable interrupt", 
        1 : "External interrupt",
        2 : "Triple fault",
        3 : "INIT signal",
        4 : "Start-up IPI (SIPI)",
        5 : "I/O system-management interrupt (SMI)",
        6 : "Other SMI",
        7 : "Interrupt window",
        8 : "NMI window",
        9 : "Task switch",
       10 : "CPUID",
       11 : "GETSEC", 
       12 : "HLT",
       13 : "INVD", 
       14 : "INVLPG",
       15 : "RDPMC",
       16 : "RDTSC",
       17 : "RSM",
       18 : "VMCALL",
       19 : "VMCLEAR",
       20 : "VMLAUNCH",
       21 : "VMPTRLD",
       22 : "VMPTRST",
       23 : "VMREAD",
       24 : "VMRESUME",
       25 : "VMWRITE",
       26 : "VMXOFF",
       27 : "VMXON",
       28 : "Control-register accesses",
       29 : "MOV DR",
       30 : "I/O instruction",
       31 : "RDMSR",
       32 : "WRMSR",
       33 : "VM-entry failure due to invalid guest state",
       34 : "VM-entry failure due to MSR loading",
       36 : "MWAIT",
       37 : "Monitor trap flag",
       39 : "MONITOR",
       40 : "PAUSE",
       41 : "VM-entry failure due to machine check",
       43 : "TPR below threshold"
    }

    if reason in vmexits:
        return vmexits[ reason & 0x7FFFFFFF ]
    else:
        return "unknown vmexit reason"
    
                                                                                 

def vmcsPrint( addr ):

    dprintln("======================")	

    dprintln( "VMCS  Start: %#x" % addr )

    vmcs = loadBytes( addr, 0x1000, True )

    revid = vmcsGetQword( vmcs, 0 )

    dprintln( "RevId: %x" % revid )

    dprintln("======================")
    dprintln( "<u>GUEST STATE</u>" )

    dprintln( "VMCS link pointer: %x" % vmcsGetQword( vmcs, 0x20 ) )

    dprintln( "RIP: %x" % vmcsGetQword( vmcs, 0x338 ) )
    dprintln( "RSP: %x" % vmcsGetQword( vmcs, 0x330 ) )
    dprintln( "RFLAGS: %x" % vmcsGetQword( vmcs, 0x340 ) )


    dprintln( "CR0: %x" % vmcsGetQword( vmcs, 0x300 ) )
    dprintln( "CR3: %x" % vmcsGetQword( vmcs, 0x308 ) )
    dprintln( "CR4: %x" % vmcsGetQword( vmcs, 0x310 ) )
    dprintln( "DR7: %x" % vmcsGetQword( vmcs, 0x328 ) )

    dprintln( "ES selector: %x  base: %x  limit: %x  access: %x" % ( vmcsGetWord( vmcs, 0x100 ), vmcsGetQword( vmcs, 0x108 ), vmcsGetDword( vmcs, 0x110 ), vmcsGetDword( vmcs, 0x114 ) ) )
    dprintln( "CS selector: %x  base: %x  limit: %x  access: %x" % ( vmcsGetWord( vmcs, 0x118 ), vmcsGetQword( vmcs, 0x120 ), vmcsGetDword( vmcs, 0x128 ), vmcsGetDword( vmcs, 0x12c ) ) )
    dprintln( "SS selector: %x  base: %x  limit: %x  access: %x" % ( vmcsGetWord( vmcs, 0x130 ), vmcsGetQword( vmcs, 0x138 ), vmcsGetDword( vmcs, 0x140 ), vmcsGetDword( vmcs, 0x144 ) ) )
    dprintln( "DS selector: %x  base: %x  limit: %x  access: %x" % ( vmcsGetWord( vmcs, 0x148 ), vmcsGetQword( vmcs, 0x150 ), vmcsGetDword( vmcs, 0x158 ), vmcsGetDword( vmcs, 0x15c ) ) )
    dprintln( "FS selector: %x  base: %x  limit: %x  access: %x" % ( vmcsGetWord( vmcs, 0x160 ), vmcsGetQword( vmcs, 0x168 ), vmcsGetDword( vmcs, 0x170 ), vmcsGetDword( vmcs, 0x174 ) ) )
    dprintln( "GS selector: %x  base: %x  limit: %x  access: %x" % ( vmcsGetWord( vmcs, 0x178 ), vmcsGetQword( vmcs, 0x180 ), vmcsGetDword( vmcs, 0x188 ), vmcsGetDword( vmcs, 0x18c ) ) )
    dprintln( "LDTR selector: %x  base: %x  limit: %x  access: %x" % ( vmcsGetWord( vmcs, 0x190 ), vmcsGetQword( vmcs, 0x198 ), vmcsGetDword( vmcs, 0x1a0 ), vmcsGetDword( vmcs, 0x1a4 ) ) )    
    dprintln( "TR selector: %x  base: %x  limit: %x  access: %x" % ( vmcsGetWord( vmcs, 0x1a8 ), vmcsGetQword( vmcs, 0x1b0 ), vmcsGetDword( vmcs, 0x1b8 ), vmcsGetDword( vmcs, 0x1bc ) ) )

    dprintln( "GDTR base: %x  limit: %x" % ( vmcsGetQword( vmcs, 0x318 ), vmcsGetWord( vmcs, 0x240 ) ) )
    dprintln( "IDTR base: %x  limit: %x" % ( vmcsGetQword( vmcs, 0x320 ), vmcsGetWord( vmcs, 0x248 ) ) )

    dprintln("======================")
    dprintln( "<u>READ ONLY</u>" )

    dprintln( "VM-instruction error: %x" % vmcsGetQword( vmcs, 0x200 ) )
    dprintln( "VM-exit reason: %d ( " % vmcsGetQword( vmcs, 0x208 )  + vmcsGetExitReason( vmcsGetQword( vmcs, 0x208 ) ) + " )" )
    dprintln( "VM-exit interruption information: %x" % vmcsGetQword( vmcs, 0x210 ) )
    dprintln( "VM-exit interruption error code: %x" % vmcsGetQword( vmcs, 0x218 ) )
    dprintln( "IDT-vectoring information field: %x" % vmcsGetQword( vmcs, 0x220 ) )
    dprintln( "IDT-vectoring error code: %x" % vmcsGetQword( vmcs, 0x228 ) )
    dprintln( "VM-exit instruction length: %x" % vmcsGetQword( vmcs, 0x230 ) )
    dprintln( "VM-exit instruction information: %x" % vmcsGetQword( vmcs, 0x238 ) )


    dprintln("======================")
    dprintln( "<u>HOST STATE</u>" )
    dprintln( "RIP: %x" % vmcsGetQword( vmcs, 0x3D8 ) )
    dprintln( "RIP: %x" % vmcsGetQword( vmcs, 0x3D0 ) )

    dprintln( "CR0: %x" % vmcsGetQword( vmcs, 0x380 ) )
    dprintln( "CR3: %x" % vmcsGetQword( vmcs, 0x388 ) )
    dprintln( "CR4: %x" % vmcsGetQword( vmcs, 0x390 ) )

    dprintln( "ES selector: %x" % vmcsGetWord( vmcs, 0x1c0 ) )
    dprintln( "CS selector: %x" % vmcsGetWord( vmcs, 0x1c8 ) )
    dprintln( "DS selector: %x" % vmcsGetWord( vmcs, 0x1d0 ) )
    dprintln( "DS selector: %x" % vmcsGetWord( vmcs, 0x1d8 ) )
    dprintln( "FS selector: %x  base: %x" % ( vmcsGetWord( vmcs, 0x1e0 ), vmcsGetQword( vmcs, 0x398 ) ) )
    dprintln( "GS selector: %x  base: %x" % ( vmcsGetWord( vmcs, 0x1e8 ), vmcsGetQword( vmcs, 0x3a0 ) ) )
    dprintln( "TR selector: %x  base: %x" % ( vmcsGetWord( vmcs, 0x1f0 ), vmcsGetQword( vmcs, 0x3a8 ) ) )

    dprintln( "GDTR base: %x" %  vmcsGetQword( vmcs, 0x3b0 ) )
    dprintln( "IDTR base: %x" %  vmcsGetQword( vmcs, 0x3b8 ) )



    dprintln("======================")
    dprintln( "<u>CONTROL STATE</u>" )
    dprintln( "Pin-based VM-execution controls: %x" % vmcsGetDword( vmcs, 0x40 ) )
    dprintln( "Primary processor-based VM-execution controls: %#x" % vmcsGetDword( vmcs, 0x48 ) )
    dprintln( "Exception bitmap: %x" % vmcsGetDword( vmcs, 0x50 ) )
    dprintln( "Page-fault error-code mask: %x" % vmcsGetDword( vmcs, 0x58 ) )
    dprintln( "Page-fault error-code match: %x" % vmcsGetDword( vmcs, 0x60 ) )
    dprintln( "VM-exit controls: %x" % vmcsGetDword( vmcs, 0x70 ) )
    dprintln( "VM-entry controls: %x" % vmcsGetDword( vmcs, 0x88 ) )

    dprintln( "CR0 guest/host mask: %x" % vmcsGetQword( vmcs, 0x278 ) )
    dprintln( "CR4 guest/host mask: %x" % vmcsGetQword( vmcs, 0x280 ) )
    dprintln( "CR0 read shadow: %x" % vmcsGetQword( vmcs, 0x288 ) )
    dprintln( "CR4 read shadow: %x" % vmcsGetQword( vmcs, 0x290 ) )



    dprintln("======================")

    	


if __name__ == "__main__":

   if not isSessionStart():
       print "script is launch out of windbg"
       quit( 0 )

   vmcsPrint( int( sys.argv[1], 16 ) )
          
          


