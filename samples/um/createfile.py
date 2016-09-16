
import pykd

GENERIC_READ = 0x80000000
GENERIC_WRITE = 0x40000000
NULL = 0

CREATE_ALWAYS = 2

FILE_ATTRIBUTE_NORMAL  = 0x80

def main():

    kernel32 = pykd.module("kernel32")

    HANDLE = pykd.typeInfo("Void*")
    LPCWSTR = pykd.typeInfo("WChar*")
    DWORD = pykd.typeInfo("UInt4B")
    LPSECURITY_ATTRIBUTES = pykd.typeInfo("Void*")

    CreateFileW_Type = pykd.defineFunction(HANDLE, pykd.callingConvention.NearStd)
    CreateFileW_Type.append("lpFileName", LPCWSTR )
    CreateFileW_Type.append("dwDesiredAccess", DWORD )
    CreateFileW_Type.append("dwShareMode", DWORD )
    CreateFileW_Type.append("lpSecurityAttributes", LPSECURITY_ATTRIBUTES )
    CreateFileW_Type.append("dwCreationDisposition", DWORD )
    CreateFileW_Type.append("dwFlagsAndAttributes", DWORD )
    CreateFileW_Type.append("hTemplateFile", HANDLE )

    fileNameBuf = pykd.stackAlloc(100)
    pykd.writeWStr(fileNameBuf, "C:\\temp\\testfile.txt")

    CreateFileW = pykd.typedVar( CreateFileW_Type, kernel32.CreateFileW )

    fileHandle = CreateFileW( 
        fileNameBuf,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL )

    print "File Handle", hex(fileHandle)

    pykd.stackFree(100)

if __name__ == "__main__":
    main()
