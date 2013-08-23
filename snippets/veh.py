import sys
import re

from pykd import *

tbuilder = typeBuilder()

IMAGE_RUNTIME_FUNCTION_ENTRY = tbuilder.createStruct( "IMAGE_RUNTIME_FUNCTION_ENTRY", 1 )
IMAGE_RUNTIME_FUNCTION_ENTRY.append( "BeginAddress", tbuilder.UInt4B )
IMAGE_RUNTIME_FUNCTION_ENTRY.append( "EndAddress", tbuilder.UInt4B )
IMAGE_RUNTIME_FUNCTION_ENTRY.append( "UnwindData", tbuilder.UInt4B )

PIMAGE_RUNTIME_FUNCTION_ENTRY = IMAGE_RUNTIME_FUNCTION_ENTRY.ptrTo()

IMAGE_DOS_HEADER = tbuilder.createStruct( "IMAGE_DOS_HEADER", 1 )
IMAGE_DOS_HEADER.append("e_magic", tbuilder.UInt2B )
IMAGE_DOS_HEADER.append("e_cblp", tbuilder.UInt2B )
IMAGE_DOS_HEADER.append("e_cp", tbuilder.UInt2B )
IMAGE_DOS_HEADER.append("e_crlc", tbuilder.UInt2B )
IMAGE_DOS_HEADER.append("e_cparhdr", tbuilder.UInt2B )
IMAGE_DOS_HEADER.append("e_minalloc", tbuilder.UInt2B )
IMAGE_DOS_HEADER.append("e_maxalloc", tbuilder.UInt2B )
IMAGE_DOS_HEADER.append("e_ss", tbuilder.UInt2B )
IMAGE_DOS_HEADER.append("e_sp", tbuilder.UInt2B )
IMAGE_DOS_HEADER.append("e_csum", tbuilder.UInt2B )
IMAGE_DOS_HEADER.append("e_ip", tbuilder.UInt2B )
IMAGE_DOS_HEADER.append("e_cs", tbuilder.UInt2B )
IMAGE_DOS_HEADER.append("e_lfarlc", tbuilder.UInt2B )
IMAGE_DOS_HEADER.append("e_ovno", tbuilder.UInt2B )
IMAGE_DOS_HEADER.append("e_res", tbuilder.UInt2B.arrayOf(4) )
IMAGE_DOS_HEADER.append("e_oemid", tbuilder.UInt2B )
IMAGE_DOS_HEADER.append("e_oeminfo", tbuilder.UInt2B )
IMAGE_DOS_HEADER.append("e_res2", tbuilder.UInt2B.arrayOf(10) )
IMAGE_DOS_HEADER.append("e_lfanew", tbuilder.UInt4B )

IMAGE_DATA_DIRECTORY = tbuilder.createStruct( "IMAGE_DATA_DIRECTORY", 1 )
IMAGE_DATA_DIRECTORY.append("VirtualAddress", tbuilder.UInt4B)
IMAGE_DATA_DIRECTORY.append("Size", tbuilder.UInt4B)

IMAGE_NUMBEROF_DIRECTORY_ENTRIES = 16

IMAGE_DIRECTORY_ENTRY_EXCEPTION = 3

IMAGE_OPTIONAL_HEADER64 = tbuilder.createStruct( "IMAGE_OPTIONAL_HEADER64", 1 )
IMAGE_OPTIONAL_HEADER64.append( "Magic", tbuilder.UInt2B)
IMAGE_OPTIONAL_HEADER64.append( "MajorLinkerVersion", tbuilder.UInt1B)
IMAGE_OPTIONAL_HEADER64.append( "MinorLinkerVersion", tbuilder.UInt1B)
IMAGE_OPTIONAL_HEADER64.append( "SizeOfCode", tbuilder.UInt4B)
IMAGE_OPTIONAL_HEADER64.append( "SizeOfInitializedData", tbuilder.UInt4B)
IMAGE_OPTIONAL_HEADER64.append( "SizeOfUninitializedData", tbuilder.UInt4B)
IMAGE_OPTIONAL_HEADER64.append( "AddressOfEntryPoint", tbuilder.UInt4B)
IMAGE_OPTIONAL_HEADER64.append( "BaseOfCode", tbuilder.UInt4B)
IMAGE_OPTIONAL_HEADER64.append( "ImageBase", tbuilder.UInt8B)
IMAGE_OPTIONAL_HEADER64.append( "SectionAlignment", tbuilder.UInt4B)
IMAGE_OPTIONAL_HEADER64.append( "FileAlignment", tbuilder.UInt4B)
IMAGE_OPTIONAL_HEADER64.append( "MajorOperatingSystemVersion", tbuilder.UInt2B)
IMAGE_OPTIONAL_HEADER64.append( "MinorOperatingSystemVersion", tbuilder.UInt2B)
IMAGE_OPTIONAL_HEADER64.append( "MajorImageVersion", tbuilder.UInt2B)
IMAGE_OPTIONAL_HEADER64.append( "MinorImageVersion", tbuilder.UInt2B)
IMAGE_OPTIONAL_HEADER64.append( "MajorSubsystemVersion", tbuilder.UInt2B)
IMAGE_OPTIONAL_HEADER64.append( "MinorSubsystemVersion", tbuilder.UInt2B)
IMAGE_OPTIONAL_HEADER64.append( "Win32VersionValue", tbuilder.UInt4B)
IMAGE_OPTIONAL_HEADER64.append( "SizeOfImage", tbuilder.UInt4B)
IMAGE_OPTIONAL_HEADER64.append( "SizeOfHeaders", tbuilder.UInt4B)
IMAGE_OPTIONAL_HEADER64.append( "CheckSum", tbuilder.UInt4B)
IMAGE_OPTIONAL_HEADER64.append( "Subsystem", tbuilder.UInt2B)
IMAGE_OPTIONAL_HEADER64.append( "DllCharacteristics", tbuilder.UInt2B)
IMAGE_OPTIONAL_HEADER64.append( "SizeOfStackReserve", tbuilder.UInt8B)
IMAGE_OPTIONAL_HEADER64.append( "SizeOfStackCommit", tbuilder.UInt8B)
IMAGE_OPTIONAL_HEADER64.append( "SizeOfHeapReserve", tbuilder.UInt8B)
IMAGE_OPTIONAL_HEADER64.append( "SizeOfHeapCommit", tbuilder.UInt8B)
IMAGE_OPTIONAL_HEADER64.append( "LoaderFlags", tbuilder.UInt4B)
IMAGE_OPTIONAL_HEADER64.append( "NumberOfRvaAndSizes", tbuilder.UInt4B)
IMAGE_OPTIONAL_HEADER64.append( "DataDirectory", IMAGE_DATA_DIRECTORY.arrayOf(IMAGE_NUMBEROF_DIRECTORY_ENTRIES) )

IMAGE_FILE_HEADER = tbuilder.createStruct( "IMAGE_FILE_HEADER", 1 )
IMAGE_FILE_HEADER.append( "Machine", tbuilder.UInt2B)
IMAGE_FILE_HEADER.append( "NumberOfSections", tbuilder.UInt2B)
IMAGE_FILE_HEADER.append( "TimeDateStamp", tbuilder.UInt4B)
IMAGE_FILE_HEADER.append( "PointerToSymbolTable", tbuilder.UInt4B)
IMAGE_FILE_HEADER.append( "NumberOfSymbols", tbuilder.UInt4B)
IMAGE_FILE_HEADER.append( "SizeOfOptionalHeader", tbuilder.UInt2B)
IMAGE_FILE_HEADER.append( "Characteristics", tbuilder.UInt2B)

IMAGE_NT_HEADERS64 = tbuilder.createStruct( "IMAGE_NT_HEADERS64", 1 )
IMAGE_NT_HEADERS64.append( "Signature", tbuilder.UInt4B)
IMAGE_NT_HEADERS64.append( "FileHeader", IMAGE_FILE_HEADER )
IMAGE_NT_HEADERS64.append( "OptionalHeader", IMAGE_OPTIONAL_HEADER64 )
        
#def findRuntimeFunctionEntry(addr):
#    pass        

def findRuntimeFunctionTable( mod ):

    dosHeader = typedVar( IMAGE_DOS_HEADER, mod )
    ntHeader = typedVar( IMAGE_NT_HEADERS64, mod + dosHeader.e_lfanew )
    runtimeTablePtr = mod + ntHeader.OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXCEPTION ].VirtualAddress  
    functionCount = ntHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION].Size / IMAGE_RUNTIME_FUNCTION_ENTRY.size()
    
    runtimeTable = typedVarArray( runtimeTablePtr, IMAGE_RUNTIME_FUNCTION_ENTRY, functionCount )
   
    return runtimeTable
    
def getInstruction( funcaddr, prologOffset ):
    dasm = disasm( funcaddr )
    while dasm.current() < funcaddr + prologOffset:
        instr = dasm.instruction()
        offset = dasm.current()
        dasm.disasm()        
       
    match = re.match( r"^[0-9a-f`]+\s+[0-9a-f]+\s+(\w+\s+(?:qword ptr |dword ptr )?[^\s]+)(?:\s+[^\s]+)?$", instr )
    if match:
        return ( offset, match.group(1) )
    
    return ( offset, instr )

def uwopSaveNonvol( arg ):
    code, data = arg
    data.next()
    return "4 - UWOP_SAVE_NONVOL"

def uwopAllocLarge( arg ):
    code, data = arg
    operationInfo = ( code >> 12 ) & 0xF
    if operationInfo == 0:
        data.next()
    else:
        data.next()
        data.next()
    return "1 - UWOP_ALLOC_LARGE"
    
def printUnwindCodes( funcaddr, data, prefixstr = "",  postfixstr = "" ):
    
    try:
    
        while True:
    
            unwindCode = data.next()
       
            prologOffset = ( unwindCode ) & 0xFF
            unwindOperation = ( unwindCode >> 8 ) & 0xF
            operationInfo = ( unwindCode >> 12 ) & 0xF             
        
            unwindCodeStr = {
                0 : lambda x : "0 - UWOP_PUSH_NONVOL",
                1 : lambda x : uwopAllocLarge(x),
                2 : lambda x : "2 - UWOP_ALLOC_SMALL",
                3 : lambda x : "3 - UWOP_SET_FPREG",
                4 : lambda x : uwopSaveNonvol(x),
                10 : lambda x : "10 - UWOP_PUSH_MACHFRAME",
            }.get( unwindOperation, "%d - unknown unwind operation" % unwindOperation )( (unwindCode, data ) )
        
            instrOffset, instrMnem = getInstruction( funcaddr, prologOffset )
        
            dprint( prefixstr )
            dprint(hex(instrOffset) )
            dprint("\t")
            dprint( unwindCodeStr )
            dprint("\t")
            dprint( instrMnem )
            dprint( postfixstr )
            dprintln("")
        
    except StopIteration:
        pass 
    
def getUnwindFlags( flags ):

    return {
        0 : "0",
        1 : "1 - EHANDLER",
        2 : "2 - UHANDLER",
        4 : "4 - CHAININFO"
    }.get(flags, "%x - Unknown flag" % flags)      
 
def printUnwindInfo( modaddr, funcaddr, unwindInfo ):
    dprintln( "Unwind Info:" )
    
    version = ptrByte( unwindInfo ) & 7
    flags = ( ptrByte( unwindInfo ) >> 3 ) & 0x1F
    sizeOfProlog = ptrByte( unwindInfo  + 1 )
    countOfCodes = ptrByte( unwindInfo  + 2 )
    frameRegsiter = ptrByte( unwindInfo  + 3 ) & 0xF
    frameOffset = ( ptrByte( unwindInfo  + 3 ) >> 4 ) & 0xF
    unwindCodes = loadWords( unwindInfo  + 4, countOfCodes )
    
    dprintln( "\tversion: %d" % version )
    dprintln( "\tflags: " + getUnwindFlags( flags ) )
    dprintln( "\tsize of prolog %d" % sizeOfProlog )
    dprintln( "\tcount of unwind code slots %d" % countOfCodes )
    
    if frameRegsiter > 0:
        dprintln( "\tframe regsiter: %s" % reg(frameRegsiter).name() )
        dprintln( "\tframe offset: %d" % ( 16 * frameOffset ) )
    
    dprintln( "\n\tUnwind codes:" )
    
    printUnwindCodes( funcaddr, iter(unwindCodes), "\t\t" )
    
    if flags == 1:
        dprintln( "\n\tException handler:" )
        addr = modaddr + ptrDWord( unwindInfo + 3 +  2*countOfCodes)
        dprintln( "\t\t" + findSymbol(addr) )    
   
        
def printVEHInfo(addr):
    
    dprintln( "\nVEH Info\n" )
    dprintln( "Function address: %x" % addr )
    dprintln( "Function symbolic name: " + findSymbol(addr) )
    
    funcModule = module(addr)
    dprintln( "Module: %s %x-%x" % ( funcModule.name(), funcModule.begin(), funcModule.end() ) )
    
    runtimeFunctionTable = findRuntimeFunctionTable(funcModule)
    dprintln( "Found runtime function table at %x with %d elements" % ( runtimeFunctionTable[0].getAddress(), len(runtimeFunctionTable) ) )
    
    runtimeFunction = filter( lambda x: funcModule + x.BeginAddress == addr, runtimeFunctionTable )[0]
    dprintln( "runtime function info for function:" )
    dprintln( "\tBegin Address: %x ( RVA = %x )" % ( funcModule + runtimeFunction.BeginAddress, runtimeFunction.BeginAddress ) )
    dprintln( "\tEnd Address: %x ( RVA = %x )" % ( funcModule + runtimeFunction.EndAddress, runtimeFunction.EndAddress ) )
    dprintln( "\tUnwind Info: %x ( RVA = %x )" % ( funcModule + runtimeFunction.UnwindData, runtimeFunction.UnwindData ) )
    
    printUnwindInfo( funcModule, addr,  funcModule + runtimeFunction.UnwindData )

def printUsage():
    pass
    
def main():
    if len(sys.argv) <=1 :
        printUsage()
        return
        
    if not is64bitSystem():
        dprintln("for 64-bit system only")
        return
        
    funcAddr = expr( sys.argv[1] )
    printVEHInfo( funcAddr )

if __name__ == "__main__":
    main()


    