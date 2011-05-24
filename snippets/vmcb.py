#
#
#


from pykd import *
import sys


VMCB_CONTROL_CR0 = 0x000
VMCB_CONTROL_DR0 = 0x004
VMCB_CONTROL_00C = 0x00C
VMCB_CONTROL_010 = 0x010
VMCB_CONTROL_PAUSE_COUNT = 0x03E
VMCB_CONTROL_IOPM_BASE_PA = 0x040
VMCB_CONTROL_MSRPM_BASE_PA = 0x048
VMCB_CONTROL_GUEST_ASID = 0x058
VMCB_CONTROL_TLB_CONTROL = 0x05C
VMCB_CONTROL_VINTERRUPT = 0x060     
VMCB_CONTROL_INRT_SHADOW = 0x068
VMCB_CONTROL_EXITCODE = 0x070
VMCB_CONTROL_EXITINFO1 = 0x078      
VMCB_CONTROL_EXITINFO2 = 0x080      
VMCB_CONTROL_EXITINTINFO = 0x088    
VMCB_CONTROL_NESTED_PAGING = 0x090       
VMCB_CONTROL_EVENTINJ = 0x0A8     
VMCB_CONTROL_N_CR3 = 0x0B0          
VMCB_CONTROL_LBR_VIRT = 0x0B8       
VMCB_CONTROL_CLEAN_BITS = 0x0C0     
VMCB_CONTROL_NRIP = 0x0C8          



VMCB_GUEST_STATE_OFFSET = 0x400
VMCB_GUEST_ES_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x000
VMCB_GUEST_CS_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x010
VMCB_GUEST_SS_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x020
VMCB_GUEST_DS_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x030
VMCB_GUEST_FS_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x040
VMCB_GUEST_GS_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x050
VMCB_GUEST_GDTR_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x060
VMCB_GUEST_LDTR_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x070
VMCB_GUEST_IDTR_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x080
VMCB_GUEST_TR_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x090
VMCB_GUEST_EFER_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x0D0
VMCB_GUEST_CR4_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x148
VMCB_GUEST_CR3_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x150
VMCB_GUEST_CR0_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x158
VMCB_GUEST_DR7_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x160
VMCB_GUEST_DR6_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x168
VMCB_GUEST_RFLAGS_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x170
VMCB_GUEST_RIP_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x178
VMCB_GUEST_RSP_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x1D8
VMCB_GUEST_RAX_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x1F8
VMCB_GUEST_STAR_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x200
VMCB_GUEST_LSTAR_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x208
VMCB_GUEST_CSTAR_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x210
VMCB_GUEST_SFMASK_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x218
VMCB_GUEST_KERNELGS_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x220
VMCB_GUEST_SYSENTER_CS_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x228
VMCB_GUEST_SYSENTER_ESP_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x230
VMCB_GUEST_SYSENTER_EIP_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x238
VMCB_GUEST_CR2_OFFSET = VMCB_GUEST_STATE_OFFSET + 0x240



def bin32( dw ):
    return "0b" + "".join( [ ((dw & ( 0x80000000 >> i ) ) == 0) and "0" or "1" for i in xrange(32) ]  )        

def bin16( dw ):
    return "0b" + "".join( [ ((dw & ( 0x8000 >> i ) ) == 0) and "0" or "1" for i in xrange(16) ]  )        

def sysselectstr( val ):
    return "selector=%x  attr=%x  base=%x limit=%x" % ( val[0] & 0xFFFF, (val[0]>>16)&0xFFFF, val[1]&0xFFFFFFFF, (val[0]>>32)&0xFFFFFFFF )


def sysselectload( addr ):
    return (ptrQWord(addr), ptrQWord(addr+8) ) 


class VmcbEntry:

    def __init__( self, desc, offset, loader, formatter ):
        self.desc = desc
        self.offset = offset
        self.loader = loader
        self.formatter = formatter
        self.value = "invalid"
      
    def load( self, address ):
        self.value = self.loader( address + self.offset )


    def __str__( self ):
        return self.formatter( self.value )



class Vmcb:


    def __init__( self, addr ):

         self.addr = addr   

         self.controlArea = [                            
             VmcbEntry( "CR0 Reads/Writes", VMCB_CONTROL_CR0,                ptrDWord,        bin32 ),
             VmcbEntry( "DR0 Reads/Writes", VMCB_CONTROL_DR0,                ptrDWord,        bin32 ),
             VmcbEntry( "Interceptors",     VMCB_CONTROL_00C,                ptrDWord,        bin32 ),
             VmcbEntry( "Interceptors",     VMCB_CONTROL_010,                ptrDWord,        bin32 ),
	     VmcbEntry( "PAUSE Filter Count",VMCB_CONTROL_PAUSE_COUNT,       ptrWord,         hex ),
	     VmcbEntry( "IOPM_BASE_PA",     VMCB_CONTROL_IOPM_BASE_PA,       ptrQWord,        hex ),
             VmcbEntry( "MSRPM_BASE_PA",    VMCB_CONTROL_MSRPM_BASE_PA,      ptrQWord,        hex ),
             VmcbEntry( "Guest ASID",       VMCB_CONTROL_GUEST_ASID,         ptrDWord,        hex ),
             VmcbEntry( "TLB_CONTROL",      VMCB_CONTROL_TLB_CONTROL,        ptrDWord,        hex ),
             VmcbEntry( "Virtual interrupt",VMCB_CONTROL_VINTERRUPT,         ptrQWord,        hex ),
             VmcbEntry( "Interrupt Shadow", VMCB_CONTROL_INRT_SHADOW,        ptrQWord,        hex ),
             VmcbEntry( "EXITCODE",         VMCB_CONTROL_EXITCODE,           ptrQWord,        hex ),
             VmcbEntry( "EXITINFO1",        VMCB_CONTROL_EXITINFO1,          ptrQWord,        hex ),
             VmcbEntry( "EXITINFO2",        VMCB_CONTROL_EXITINFO2,          ptrQWord,        hex ),
             VmcbEntry( "EXITINTINFO",      VMCB_CONTROL_EXITINTINFO,        ptrQWord,        hex ),
             VmcbEntry( "Nested Paging",    VMCB_CONTROL_NESTED_PAGING,      ptrQWord,        hex ),     
             VmcbEntry( "EVENTINJ",         VMCB_CONTROL_EVENTINJ,           ptrQWord,        hex ),
             VmcbEntry( "N_CR3",            VMCB_CONTROL_N_CR3,              ptrQWord,        hex ),
             VmcbEntry( "LBR_VIRTUALIZATION_ENABLE", VMCB_CONTROL_LBR_VIRT,  ptrQWord,        hex ),
             VmcbEntry( "Clean Bits",       VMCB_CONTROL_CLEAN_BITS,         ptrDWord,        hex ),
             VmcbEntry( "nRIP",             VMCB_CONTROL_NRIP,               ptrQWord,        hex ),
         ]

         self.guestArea = [
             VmcbEntry( "ES",               VMCB_GUEST_ES_OFFSET,            sysselectload,   sysselectstr ),
             VmcbEntry( "CS",               VMCB_GUEST_CS_OFFSET,            sysselectload,   sysselectstr ),
             VmcbEntry( "SS",               VMCB_GUEST_SS_OFFSET,            sysselectload,   sysselectstr ),
             VmcbEntry( "DS",               VMCB_GUEST_DS_OFFSET,            sysselectload,   sysselectstr ),
             VmcbEntry( "FS",               VMCB_GUEST_FS_OFFSET,            sysselectload,   sysselectstr ),
             VmcbEntry( "GS",               VMCB_GUEST_GS_OFFSET,            sysselectload,   sysselectstr ),
             VmcbEntry( "GDTR",             VMCB_GUEST_GDTR_OFFSET,          sysselectload,   sysselectstr ),
             VmcbEntry( "LDTR",             VMCB_GUEST_LDTR_OFFSET,          sysselectload,   sysselectstr ),
             VmcbEntry( "IDTR",             VMCB_GUEST_IDTR_OFFSET,          sysselectload,   sysselectstr ),
             VmcbEntry( "TR",               VMCB_GUEST_TR_OFFSET,            sysselectload,   sysselectstr ),
             VmcbEntry( "EFER",             VMCB_GUEST_EFER_OFFSET,          ptrQWord,        hex ),
             VmcbEntry( "CR4",              VMCB_GUEST_CR4_OFFSET,           ptrQWord,        hex ),
             VmcbEntry( "CR3",              VMCB_GUEST_CR3_OFFSET,           ptrQWord,        hex ),
             VmcbEntry( "CR0",              VMCB_GUEST_CR0_OFFSET,           ptrQWord,        hex ),
             VmcbEntry( "DR6",              VMCB_GUEST_DR6_OFFSET,           ptrQWord,        hex ),             
             VmcbEntry( "DR7",              VMCB_GUEST_DR7_OFFSET,           ptrQWord,        hex ),    
             VmcbEntry( "RFLAGS",           VMCB_GUEST_RFLAGS_OFFSET,        ptrQWord,        hex ),	      
             VmcbEntry( "RIP",              VMCB_GUEST_RIP_OFFSET,           ptrQWord,        hex ),    
             VmcbEntry( "RSP",              VMCB_GUEST_RSP_OFFSET,           ptrQWord,        hex ),	
             VmcbEntry( "RAX",              VMCB_GUEST_RAX_OFFSET,           ptrQWord,        hex ),	
             VmcbEntry( "STAR",             VMCB_GUEST_STAR_OFFSET,          ptrQWord,        hex ),	
             VmcbEntry( "LSTAR",            VMCB_GUEST_LSTAR_OFFSET,         ptrQWord,        hex ),	
             VmcbEntry( "CSTAR",            VMCB_GUEST_CSTAR_OFFSET,         ptrQWord,        hex ),	
             VmcbEntry( "SFMASK",           VMCB_GUEST_SFMASK_OFFSET,        ptrQWord,        hex ),	
             VmcbEntry( "KERNEL GS Base",   VMCB_GUEST_KERNELGS_OFFSET,      ptrQWord,        hex ),	
             VmcbEntry( "SYSENTER CS",      VMCB_GUEST_SYSENTER_CS_OFFSET,   ptrQWord,        hex ),	
             VmcbEntry( "SYSENTER ESP",     VMCB_GUEST_SYSENTER_ESP_OFFSET,  ptrQWord,        hex ),	
             VmcbEntry( "SYSENTER EIP",     VMCB_GUEST_SYSENTER_EIP_OFFSET,  ptrQWord,        hex ),	
             VmcbEntry( "CR2",              VMCB_GUEST_CR2_OFFSET,           ptrQWord,        hex ),	

         ]


         self.controlArea.sort( key=lambda v: v.offset )  
         self.guestArea.sort( key=lambda v: v.offset )  

         for vmcbEntry in self.controlArea: vmcbEntry.load( addr )   
         for vmcbEntry in self.guestArea: vmcbEntry.load( addr )   

    def __str__ ( self ):

         s = "\nVMCB Start Address: %x\n" % self.addr;

         s += "\n*** CONTROL AREA ***\n"


         for vmcbEntry in self.controlArea:
             s +=  "\t+%03x  %s:  %s\n" % ( vmcbEntry.offset, vmcbEntry.desc, str(vmcbEntry) )


         s += "\n*** GUEST STATE AREA ***\n"

         for vmcbEntry in self.guestArea:
             s +=  "\t+%03x  %s:  %s\n" % ( vmcbEntry.offset, vmcbEntry.desc, str(vmcbEntry) )

         return s


def main():

    if not isWindbgExt():
        dprintln( "script is launch out of windbg" )
        return

    if len( sys.argv ) <= 1:
        dprintln( "usage: !py vmcb <addr>" )
        return


    try:

       dprint( str( Vmcb( expr( sys.argv[1] ) ) ) )

    except MemoryException, e:

       dprintln( e.desc() )


if __name__ == "__main__":
    main()
