
#
# CPU registers 
#

import pykd

CPU = pykd.getProcessorMode()
x86Regs = [ "eax", "ebx", "ecx", "edx", "esi", "edi", "eip", "ebp", "esp" ]
amd64Regs = [ "rax", "rbx", "rcx", "rdx", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", "rsi", "rdi", "rip", "rbp", "rsp" ]


def reloadRegs():

    """ reload register's values"""

    if CPU == "X86":

        for regName in x86Regs:
            globals()[regName] = pykd.reg(regName)                

    elif CPU == "X64":

        for regName in amd64Regs:
            globals()[regName] = pykd.reg(regName)             


def printRegs():

    """ print CPU registers values"""

    if CPU == "X86":
        for regName in x86Regs:
             pykd.dprintln( "%s = %#x( %d )" % ( regName, globals()[regName], globals()[regName] ) )

    elif CPU == "X64":
        for regName in amd64Regs:
             pykd.dprintln( "%s = %#x( %d )" % ( regName, globals()[regName], globals()[regName] ) )



reloadRegs()
