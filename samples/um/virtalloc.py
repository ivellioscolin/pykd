

import pykd


win_src = '''

typedef void*  LPVOID;
typedef size_t  SIZE_T;
typedef unsigned long  DWORD;
typedef bool  BOOL;

LPVOID
__stdcall
VirtualAlloc(
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD flAllocationType,
    DWORD flProtect
    );
    
BOOL 
__stdcall
VirtualFree(
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD dwFreeType
);    


DWORD 
__stdcall
GetLastError(void);l
    
'''

MEM_COMMIT = 0x1000
MEM_RELEASE = 0x8000
PAGE_READWRITE = 0x4

winTypeProvider = pykd.getTypeInfoProviderFromSource(win_src, "-w")

kernel = pykd.module('KERNELBASE')

VirtualAlloc = pykd.typedVar(winTypeProvider.getTypeByName('VirtualAlloc'), kernel.VirtualAlloc )
VirtualFree = pykd.typedVar(winTypeProvider.getTypeByName('VirtualFree'), kernel.VirtualFree )
GetLastError = pykd.typedVar(winTypeProvider.getTypeByName('GetLastError'), kernel.GetLastError )

addr = VirtualAlloc(0, 0x1000, MEM_COMMIT, PAGE_READWRITE)

if addr:
    print("Allocated memory: %x" % addr )

    if VirtualFree(addr, 0, MEM_RELEASE):
        print("Successfully free memory")
    else:
        print("Failed VirtualFree with error %x" % GetLastError() )
else:
    print("Failed VirtualAlloc with error %x" % GetLastError() )
