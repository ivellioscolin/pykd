#include "stdafx.h"

#include <wdbgexts.h>

#include <string>

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/tokenizer.hpp>
#include <boost/python/overloads.hpp>

#include "dbgext.h"
#include "dbgreg.h"
#include "dbgtype.h"
#include "dbgmodule.h"
#include "dbgsym.h"
#include "dbgmem.h"
#include "dbgsystem.h"
#include "dbgcmd.h"
#include "dbgdump.h"
#include "dbgexcept.h"
#include "dbgeventcb.h"
#include "dbgpath.h"
#include "dbgprocess.h"
#include "dbgsynsym.h"
#include "dbgclient.h"
#include "dbgevent.h"
#include "dbgbreak.h"
#include "dbgio.h"
#include "intbase.h"
#include "disasm.h"

//////////////////////////////////////////////////////////////////////////////

// указатель на текущий интерфейс
DbgExt    *dbgExt = NULL;

// глобальный клиент 
dbgClient    g_dbgClient;

// контескт исполнения нити питона
PyThreadStateSaver       g_pyThreadState;

//////////////////////////////////////////////////////////////////////////////

BOOST_PYTHON_FUNCTION_OVERLOADS( dprint, dbgPrint::dprint, 1, 2 )
BOOST_PYTHON_FUNCTION_OVERLOADS( dprintln, dbgPrint::dprintln, 1, 2 )

BOOST_PYTHON_FUNCTION_OVERLOADS( loadCharsOv, loadChars, 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadWCharsOv, loadWChars, 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadBytes, loadArray<unsigned char>, 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadWords, loadArray<unsigned short>, 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadDWords, loadArray<unsigned long>, 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadQWords, loadArray<unsigned __int64> , 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignBytes, loadArray<char> , 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignWords, loadArray<short> , 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignDWords, loadArray<long> , 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignQWords, loadArray<__int64>, 2, 3 )

BOOST_PYTHON_FUNCTION_OVERLOADS( compareMemoryOver, compareMemory, 3, 4 )

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( appendOver, TypeInfo::appendField, 2, 3 )
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( loadOver, TypeInfo::loadVar, 1, 2 )


#define DEF_PY_CONST(x)    \
    boost::python::scope().attr(#x) = ##x
    
#define DEF_PY_GLOBAL(x,y)  \
    boost::python::scope().attr(x) = ##y


BOOST_PYTHON_MODULE( pykd )
{
    boost::python::def( "go", &setExecutionStatus<DEBUG_STATUS_GO>, 
        "Change debugger status to DEBUG_STATUS_GO"  );
    boost::python::def( "trace", &setExecutionStatus<DEBUG_STATUS_STEP_INTO>, 
        "Change debugger status to DEBUG_STATUS_STEP_INTO" );
    boost::python::def( "step", &setExecutionStatus<DEBUG_STATUS_STEP_OVER>, 
        "Change debugger status to DEBUG_STATUS_STEP_OVER" );   
    boost::python::def( "breakin", &breakin,
        "Break into debugger" );
    boost::python::def( "expr", &evaluate, 
        "Evaluate windbg expression" ); 
    boost::python::def( "isWindbgExt", &isWindbgExt, 
        "Check if script works in windbg context" );
    boost::python::def( "symbolsPath", &dbgSymPath, 
        "Return symbol path" );
    boost::python::def( "dprint", &dbgPrint::dprint, dprint( boost::python::args( "str", "dml" ), 
        "Print out string. If dml = True string is printed with dml highlighting ( only for windbg )") );
    boost::python::def( "dprintln", &dbgPrint::dprintln, dprintln( boost::python::args( "str", "dml" ), 
        "Print out string and insert end of line symbol. If dml = True string is printed with dml highlighting ( only for windbg )"  ) );
    boost::python::def( "loadDump", &dbgLoadDump,
        "Load crash dump (only for console)");
    boost::python::def( "startProcess", &startProcess,
        "Start process for debugging (only for console)"); 
    boost::python::def( "attachProcess", &attachProcess,
        "Attach debugger to a exsisting process" );
    boost::python::def( "attachKernel", &attachKernel,
        "Attach debugger to a kernel target" );
    boost::python::def( "dbgCommand", &dbgCommand,
        "Execute debugger command. For example: dbgCommand( \"lmvm nt\" )" );
    boost::python::def( "isValid", &isOffsetValid,
        "Check if virtual address is valid" );
    boost::python::def( "is64bitSystem", &is64bitSystem,
        "Check if target system has 64 address space" );
    boost::python::def( "isKernelDebugging", &isKernelDebugging,
        "Check if kernel dubugging is running" );
    boost::python::def( "isDumpAnalyzing", &isDumpAnalyzing,
        "Check if it is a dump analyzing ( not living debuggee )" );
    boost::python::def( "ptrSize", ptrSize,
        "Return pointer size ( in bytes )" );
    boost::python::def( "reg", &loadRegister,
        "Return CPU's register value" );
    boost::python::def( "rdmsr", &loadMSR,
        "Return MSR value" );
    boost::python::def( "wrmsr", &setMSR,
        "Set MSR value" );
    boost::python::def( "typedVarList", &loadTypedVarList,
        "Return list of typedVarClass instances. Each item represents one item of the linked list in the target memory" );        
    boost::python::def( "typedVarArray", &loadTypedVarArray,
        "Return list of typedVarClass instances. Each item represents one item of the counted array the target memory" );
    boost::python::def( "containingRecord", &containingRecord,
        "Return instance of the typedVarClass. It's value are loaded from the target memory."
        "The start address is calculated by the same method as standard macro CONTAINING_RECORD" );
    boost::python::def( "sizeof", &sizeofType,
        "Return size of type" );
    boost::python::def( "loadModule", &loadModule,
        "Return instance of the dbgModuleClass" );
    boost::python::def( "findSymbol", &findSymbolForAddress,
        "Return symbol for specified target address if it exists" );
    boost::python::def( "getOffset", &findAddressForSymbol,
        "Return target address for specified symbol" );
    boost::python::def( "findModule", &findModule,
        "Return instance of the dbgModuleClass which posseses specified address" );
    boost::python::def( "addr64", &addr64,
        "Extend address to 64 bits formats ( for x86 )" );
    boost::python::def( "loadChars", loadChars, loadCharsOv( boost::python::args( "address", "number",  "phyAddr" ),
        "Load string from the target buffer" ) );
    boost::python::def( "loadWChars", loadWChars, loadWCharsOv( boost::python::args( "address", "number",  "phyAddr" ),        
        "Load unicode string from the target buffer" ) );
    boost::python::def( "loadBytes", &loadArray<unsigned char>, loadBytes( boost::python::args( "address", "number",  "phyAddr"  ), 
        "Return list of unsigned bytes" ) );
    boost::python::def( "loadWords", &loadArray<unsigned short>, loadWords( boost::python::args( "address", "number",  "phyAddr"  ), 
        "Return list of unsigned words (2-bytes )" ) );
    boost::python::def( "loadDWords", &loadArray<unsigned long>, loadDWords( boost::python::args( "address", "number",  "phyAddr"  ),
        "Return list of unsigned dwords (4-bytes)" ) );
    boost::python::def( "loadQWords", &loadArray<unsigned __int64>, loadQWords( boost::python::args( "address", "number",  "phyAddr"  ),
        "Return list of unsigned qwords (8-butes)" ) );
    boost::python::def( "loadSignBytes", &loadArray<char>, loadSignBytes( boost::python::args( "address", "number",  "phyAddr"  ),
        "Return list of signed bytes" ) );
    boost::python::def( "loadSignWords", &loadArray<short>, loadSignWords( boost::python::args( "address", "number",  "phyAddr"  ), 
        "Return list of signed words (2-bytes)" ) );
    boost::python::def( "loadSignDWords", &loadArray<long>, loadSignDWords( boost::python::args( "address", "number",  "phyAddr"  ), 
        "Return list of signed dwords (4-bytes)" ) );
    boost::python::def( "loadSignQWords", &loadArray<__int64>, loadSignQWords( boost::python::args( "address", "number",  "phyAddr"  ),
        "Return list of signed qwords (8-bytes)" ) );
    boost::python::def( "loadPtrs", &loadPtrArray,
        "Return list of pointers" );
    boost::python::def( "loadUnicodeString", &loadUnicodeStr,
        "Return string represention of windows UNICODE_STRING type" );
    boost::python::def( "loadAnsiString", &loadAnsiStr,
        "Return string represention of windows ANSU_STRING type" );
    boost::python::def( "loadCStr", &loadCStr,
        "Load string from the target buffer containing 0-terminated ansi-string" );
    boost::python::def( "loadWStr", &loadWStr,
        "Load string from the target buffer containing 0-terminated unicode-string" );
    boost::python::def( "loadLinkedList", &loadLinkedList,
        "Return list of instances of the typedVarClass loaded from linked list in the target memory" );
    boost::python::def( "ptrByte", &loadByPtr<unsigned char>,
        "Return 1-byte unsigned value loaded by pointer" );
    boost::python::def( "ptrSignByte", &loadByPtr<char>,
        "Return 1-byte signed value loaded by pointer" );
    boost::python::def( "ptrWord", &loadByPtr<unsigned short>,
        "Return 2-byte unsigned value loaded by pointer" );
    boost::python::def( "ptrSignWord", &loadByPtr<short>,
        "Return 2-byte signed value loaded by pointer" );
    boost::python::def( "ptrDWord", &loadByPtr<unsigned long>,
        "Return 4-byte unsigned value loaded by pointer" );
    boost::python::def( "ptrSignDWord", &loadByPtr<long>,
        "Return 4-byte signed value loaded by pointer" );
    boost::python::def( "ptrQWord", &loadByPtr<unsigned __int64>,
        "Return 8-byte unsigned value loaded by pointer" );        
    boost::python::def( "ptrSignQWord", &loadByPtr<__int64>,
        "Return 8-byte signed value loaded by pointer" );      
    boost::python::def( "ptrPtr", &loadPtrByPtr,
        "Return pointer value loaded by pointer" );  
    boost::python::def( "ptrMWord", &loadMWord,
        "Return unsigned machine word ( 4-bytes for x86 and 8-bytes for x64 ) loaded by pointer" ); 
    boost::python::def( "ptrSignMWord", &loadSignMWord,
        "Return signed machine word ( 4-bytes for x86 and 8-bytes for x64 ) loaded by pointer" );     
    boost::python::def( "compareMemory", &compareMemory, compareMemoryOver( boost::python::args( "addr1", "addr2", "length", "phyAddr" ), 
        "Compare two memory buffers by virtual or physical addresses" ) );
    boost::python::def( "getCurrentStack", &getCurrentStack, 
        "Return list of dbgStackFrameClass for current stack" );
    boost::python::def( "locals", &getLocals, 
        "Return dict of locals variables (each item is typedVarClass)" );
    boost::python::def( "reloadModule", &reloadModule, 
        "Reload symbols by module name" );
    boost::python::def( "getPdbFile", &getPdbFile, 
        "Return full path to PDB (Program DataBase, debug information) file" );
    boost::python::def( "getImplicitThread", &getImplicitThread, 
        "Return implicit thread for current process" );
    boost::python::def( "setImplicitThread", &setImplicitThread, 
        "Set implicit thread for current process" );
    boost::python::def( "getThreadList", &getThreadList, 
        "Return list of threads (each item is numeric address of thread)" );
    boost::python::def( "getCurrentProcess", &getCurrentProcess, 
        "Return current process (numeric address)" );
    boost::python::def( "setCurrentProcess", &setCurrentProcess, 
        "Set current process by address" );
    boost::python::def( "getProcessorMode", &getProcessorMode, 
        "Return current processor mode as string: X86, ARM, IA64 or X64" );
    boost::python::def( "setProcessorMode", &setProcessorMode, 
        "Set current processor mode by string (X86, ARM, IA64 or X64)" );
    boost::python::def( "addSynSymbol", &addSyntheticSymbol,
        "Add new synthetic symbol for virtual address" );
    boost::python::def( "delAllSynSymbols", &delAllSyntheticSymbols, 
        "Delete all synthetic symbol for all modules");
    boost::python::def( "delSynSymbol", &delSyntheticSymbol, 
        "Delete synthetic symbols by virtual address" );
    boost::python::def( "delSynSymbolsMask", &delSyntheticSymbolsMask, 
        "Delete synthetic symbols by mask of module and symbol name");
    boost::python::def( "debuggerPath", &getDebuggerImage,
        "Return full path to the process image that uses pykd" );

    boost::python::class_<TypeInfo>( "typeInfo",
        "Class representing non-primitive type info: structure, union, etc. attributes is a fields of non-primitive type" )
        .def(boost::python::init<std::string,std::string>( boost::python::args("module", "type"), "constructor" ) )
        .def(boost::python::init<std::string>( boost::python::args("typeName"), "constructor" ) )
        .def(boost::python::init<std::string,ULONG>( boost::python::args("typeName", "align"), "constructor" ) )
        .def("size", &TypeInfo::size,
            "Return full size of non-primitive type" )
        .def("name", &TypeInfo::name,
            "Return type's name" )
        .def("__str__", &TypeInfo::print,
            "Return a nice string represention: print names and offsets of fields" )
        .def("__getattr__", &TypeInfo::getField )
        .def("__len__", &TypeInfo::getFieldCount )
        .def("__getitem__", &TypeInfo::getFieldByIndex )
        .def("append", &TypeInfo::appendField, appendOver( boost::python::args("type", "fieldName", "count"),
            "add new field for typeInfo" ) )
        .def("offset", &TypeInfo::getFieldOffset,
            "Return offset while type is part of the more complex type" )
        .def("setAlignReq", &TypeInfo::setAlignReq,
            "Set alignment requirement" )
        .def("load", &TypeInfo::loadVar, loadOver( boost::python::args( "offset", "count"),
            "Create instance of the typedVar class with this typeInfo" ) );

    DEF_PY_GLOBAL( "char_t", TypeInfo("", "char") );
    DEF_PY_GLOBAL( "uchar_t", TypeInfo("", "unsigned char") );
    DEF_PY_GLOBAL( "short_t", TypeInfo("", "short") );
    DEF_PY_GLOBAL( "ushort_t", TypeInfo("", "unsigned short") );
    DEF_PY_GLOBAL( "long_t", TypeInfo("", "long") );
    DEF_PY_GLOBAL( "ulong_t", TypeInfo("", "unsigned long") );
    DEF_PY_GLOBAL( "int_t", TypeInfo("", "int") );
    DEF_PY_GLOBAL( "uint_t", TypeInfo("", "unsigned int") );
    //DEF_PY_GLOBAL( "ptr_t", TypeInfo("", "void*") );
    DEF_PY_GLOBAL( "double_t", TypeInfo("", "double") );
    DEF_PY_GLOBAL( "longlong_t", TypeInfo("", "int64") );
    DEF_PY_GLOBAL( "ulonglong_t", TypeInfo("", "unsigned int64") );
    
    boost::python::class_<TypedVar>( "typedVar", 
        "Class of non-primitive type object, child class of typeClass. Data from target is copied into object instance" )
        .def(boost::python::init<TypeInfo,ULONG64>(boost::python::args("typeInfo", "address"),
            "constructor" ) )
        .def(boost::python::init<std::string,std::string,ULONG64>(boost::python::args("moduleName", "typeName", "address"), 
            "constructor" ) )
        .def(boost::python::init<ULONG64>(boost::python::args("address"),
            "constructor" ) )
        .def(boost::python::init<std::string>(boost::python::args("symbolName"), 
            "constructor" ) )
        .def("getAddress", &TypedVar::getAddress, 
            "Return virtual address" )
        .def("sizeof", &TypedVar::getSize,            
            "Return size of a variable in the target memory" )
        .def("data", &TypedVar::data,
            "Return raw string object with data stream" )
        .def("__getattr__", &TypedVar::getFieldWrap,
            "Return field of structure as an object attribute" )
        .def("__str__", &TypedVar::print,
            "Return a nice string represention: print names and offsets of fields" );     
    
    boost::python::class_<dbgModuleClass>( "dbgModuleClass",
        "Class representing module in the target memory" )
        .def("begin", &dbgModuleClass::getBegin,
            "Return start address of the module" )
        .def("end", &dbgModuleClass::getEnd,
            "Return end address of the module" )
        .def("size", &dbgModuleClass::getSize,
            "Return size of the module" )
        .def("name", &dbgModuleClass::getName,
            "Return name of the module" )
        .def("contain", &dbgModuleClass::contain,
            "Check if the address belongs to the module")
        .def("image", &dbgModuleClass::getImageSymbolName,
            "Return the full path to the module's image file" )
        .def("pdb", &dbgModuleClass::getPdbName,
            "Return the full path to the module's pdb file ( symbol information )" )
        .def("checksum", &dbgModuleClass::getCheckSum,
            "Return checksum of the module ( from PE header )" )
        .def("timestamp", &dbgModuleClass::getTimeStamp,
            "Return timestamp of the module ( from PE header )" )
        .def("addSynSymbol", &dbgModuleClass::addSyntheticSymbol,
            "Add synthetic symbol for the module" )
        .def("delAllSynSymbols", &dbgModuleClass::delAllSyntheticSymbols,
            "Remove all synthetic symbols for the module" )
        .def("delSynSymbol", &dbgModuleClass::delSyntheticSymbol,
            "Remove specified synthetic symbol for the module" )
        .def("delSynSymbolsMask", &dbgModuleClass::delSyntheticSymbolsMask )
        .def("__getattr__", &dbgModuleClass::getOffset,
            "Return address of the symbol" )
        .def("__str__", &dbgModuleClass::print,
            "Return a nice string represention of the dbgModuleClass" );

    boost::python::class_<dbgExtensionClass>(
            "ext",
            "windbg extension wrapper",
             boost::python::init<const char*>( boost::python::args("path"), "__init__  dbgExtensionClass" ) )
        .def("call", &dbgExtensionClass::call,
            "Call extension command" )
        .def("__str__", &dbgExtensionClass::print,
            "Return a nice string represention of the dbgExtensionClass" );

    boost::python::class_<dbgStackFrameClass>( "dbgStackFrameClass", 
         "Class representing a frame of the call satck" )
        .def_readonly( "instructionOffset", &dbgStackFrameClass::InstructionOffset,
            "Return a frame's instruction offset" )
        .def_readonly( "returnOffset", &dbgStackFrameClass::ReturnOffset,
            "Return a frame's return offset" )
        .def_readonly( "frameOffset", &dbgStackFrameClass::FrameOffset,
            "Return a frame's offset" )
        .def_readonly( "stackOffset", &dbgStackFrameClass::StackOffset,
            "Return a frame's stack offset" )
        .def_readonly( "frameNumber", &dbgStackFrameClass::FrameNumber,
            "Return a frame's number" )
        .def( "__str__", &dbgStackFrameClass::print,
            "Return a nice string represention of the dbgStackFrameClass" );
            
    boost::python::class_<dbgOut>( "windbgOut", "windbgOut" )
        .def( "write", &dbgOut::write );
        
    boost::python::class_<dbgIn>( "windbgIn", "windbgIn" )
        .def( "readline", &dbgIn::readline );
        
    boost::python::class_<dbgBreakpointClass>( "bp",
         "Class representing breakpoint",
         boost::python::init<ULONG64,boost::python::object&>( boost::python::args("offset", "callback"), 
         "Break point: user callback" ) )
        .def( boost::python::init< ULONG64 >( boost::python::args("offset"), 
            "Break point constructor: always break" ) )
        .def( "set", &dbgBreakpointClass::set,
            "Set a breakpoint at the specified address" )
        .def( "remove", &dbgBreakpointClass::remove,
            "Remove a breakpoint set before" )
        .def( "__str__", &dbgBreakpointClass::print,
            "Return a nice string represention of the breakpoint class"  );

    boost::python::class_<debugEventWrap, boost::noncopyable>( "debugEvent",
        "Base class for debug events handlers" )
        .def( "onBreakpoint", &debugEventWrap::onBreakpoint,
            "Triggered breakpoint event. Parameter is dict:\n"
            "{\"Id\":int, \"BreakType\":int, \"ProcType\":int, \"Flags\":int, \"Offset\":int,"
            " \"Size\":int, \"AccessType\":int, \"PassCount\":int, \"CurrentPassCount\":int,"
            " \"MatchThreadId\":int, \"Command\":str, \"OffsetExpression\":str}\n"
            "Detailed information: http://msdn.microsoft.com/en-us/library/ff539284(VS.85).aspx \n"
            "For ignore event method must return DEBUG_STATUS_NO_CHANGE value" )
        .def( "onException", &debugEventWrap::onException,
            "Exception event. Parameter is dict:\n"
            "{\"Code\":int, \"Flags\":int, \"Record\":int, \"Address\":int,"
            " \"Parameters\":[int], \"FirstChance\":bool}\n"
            "Detailed information: http://msdn.microsoft.com/en-us/library/aa363082(VS.85).aspx \n"
            "For ignore event method must return DEBUG_STATUS_NO_CHANGE value" )
        .def( "onLoadModule", &debugEventWrap::onLoadModule,
            "Load module event. Parameter is instance of dbgModuleClass.\n"
            "For ignore event method must return DEBUG_STATUS_NO_CHANGE value" )
        .def( "onUnloadModule", &debugEventWrap::onUnloadModule,
            "Unload module event. Parameter is instance of dbgModuleClass.\n"
            "For ignore event method must return DEBUG_STATUS_NO_CHANGE value" );

    boost::python::class_<disasm>("disasm", "Class disassemble a processor instructions", boost::python::no_init )
        .def( boost::python::init<>( "constructor" ) )
        .def( boost::python::init<ULONG64>( boost::python::args("offset"), "constructor" ) )
        .def( "disasm", &disasm::disassemble, "Disassemble next instruction" )
        .def( "sasm", &disasm::assembly, "Insert assemblied instuction to current offset" )
        .def( "begin", &disasm::begin, "Return begin offset" )
        .def( "current", &disasm::current, "Return current offset" )
        .def( "length", &disasm::length, "Return current instruction length" )
        .def( "instruction", &disasm::instruction, "Returm current disassembled instruction" )
        .def( "ea", &disasm::ea, "Return effective address for last disassembled instruction or 0" )
        .def( "reset", &disasm::reset, "Reset current offset to begin" );


    // исключения
    boost::python::class_<DbgException>  dbgExceptionClass( "BaseException",
        "Pykd base exception class",
        boost::python::no_init );
         //boost::python::init<std::string>() );

    dbgExceptionClass     
        .def( boost::python::init<std::string>( boost::python::args("desc"), "constructor" ) )
        .def( "desc", &DbgException::getDesc,
            "Get exception description" );

    boost::python::class_<WaitEventException, boost::python::bases<DbgException> >  waitExceptionClass( "WaitEventException",
        "Type exception class",
        boost::python::no_init );

    boost::python::class_<TypeException, boost::python::bases<DbgException> >  typeExceptionClass( "TypeException",
        "Type exception class",
        boost::python::no_init );

    boost::python::class_<MemoryException, boost::python::bases<DbgException> > memoryExceptionClass( "MemoryException",
        "Memory exception class",
        boost::python::no_init );
        
    memoryExceptionClass
        .def( boost::python::init<ULONG64>( boost::python::args("targetAddress"), "constructor" ) )
        .def( "getAddress", &MemoryException::getAddress,
            "Return target address" );

    baseExceptionType = dbgExceptionClass.ptr();
    eventExceptionType = waitExceptionClass.ptr();
    typeExceptionType = typeExceptionClass.ptr();
    memoryExceptionType = memoryExceptionClass.ptr();

    boost::python::register_exception_translator<DbgException>( &DbgException::exceptionTranslate );
    boost::python::register_exception_translator<WaitEventException>( &WaitEventException::exceptionTranslate );
    boost::python::register_exception_translator<TypeException>( &TypeException::exceptionTranslate );
    boost::python::register_exception_translator<IndexException>( &IndexException::translate); 
    boost::python::register_exception_translator<MemoryException>( &MemoryException::translate );
    
    boost::python::class_<intBase>( 
        "intBase",
        "intBase")
            .def( boost::python::init<>() )
            .def( boost::python::init<ULONG64>( boost::python::args("value"), "constructor" ) )

            .def( "value", &intBase::value )
            .def( int_( boost::python::self ) )
            //.def( boost::python::self = long() )

            .def( boost::python::self + long() )
            .def( long() + boost::python::self )
            .def( boost::python::self += long() )
            .def( boost::python::self + boost::python::self )
            .def( boost::python::self += boost::python::self )

            .def( boost::python::self - long() )
            .def( long() - boost::python::self )
            .def( boost::python::self -= long() )
            .def( boost::python::self - boost::python::self )
            .def( boost::python::self -= boost::python::self )

            .def( boost::python::self * long() )
            .def( long() * boost::python::self )
            .def( boost::python::self *= long() )
            .def( boost::python::self * boost::python::self )
            .def( boost::python::self *= boost::python::self )

            .def( boost::python::self / long() )
            .def( long() / boost::python::self )
            .def( boost::python::self /= long() )
            .def( boost::python::self / boost::python::self )
            .def( boost::python::self /= boost::python::self )
            
            .def( boost::python::self % long() )
            .def( long() % boost::python::self )
            .def( boost::python::self %= long() )
            .def( boost::python::self % boost::python::self )
            .def( boost::python::self %= boost::python::self )

            .def( boost::python::self & long() )
            .def( long() & boost::python::self )
            .def( boost::python::self &= long() )
            .def( boost::python::self & boost::python::self )
            .def( boost::python::self &= boost::python::self )

            .def( boost::python::self | long() )
            .def( long() | boost::python::self )
            .def( boost::python::self |= long() )
            .def( boost::python::self | boost::python::self )
            .def( boost::python::self |= boost::python::self )

            .def( boost::python::self ^ long() )
            .def( long() ^ boost::python::self )
            .def( boost::python::self ^= long() )
            .def( boost::python::self ^ boost::python::self )
            .def( boost::python::self ^= boost::python::self )

            .def( boost::python::self << long() )
            .def( boost::python::self <<= long() )

            .def( boost::python::self >> long() )
            .def( boost::python::self >>= long() ) 

            .def( boost::python::self < long() )
            .def( boost::python::self < boost::python::self )

            .def( boost::python::self <= long() )
            .def( boost::python::self <= boost::python::self )

            .def( boost::python::self == long() )           
            .def( boost::python::self == boost::python::self )

            .def( boost::python::self >= long() )
            .def( boost::python::self >= boost::python::self )

            .def( boost::python::self > long() )
            .def( boost::python::self > boost::python::self )

            .def( boost::python::self != long() )
            .def( boost::python::self != boost::python::self )

            .def( ~boost::python::self )
            .def( !boost::python::self )

            .def( "__str__", &intBase::str )
            .def( "__hex__", &intBase::hex );
            
    boost::python::class_<cpuReg, boost::python::bases<intBase> >(
        "cpuReg",
        "CPU regsiter class",
        boost::python::no_init )
            .def( boost::python::init<std::string>(boost::python::args("name"), "constructor" ) )
            .def( boost::python::init<ULONG>(boost::python::args("index"), "constructor" ) )
            .def( "name", &cpuReg::name, "The name of the regsiter" )
            .def( "index", &cpuReg::index, "The index of thr register" )
            .def( "beLive", &cpuReg::beLive, "Turn the object to live: its value will be following the target register value" );
            

    // debug status
    DEF_PY_CONST(DEBUG_STATUS_NO_CHANGE);
    DEF_PY_CONST(DEBUG_STATUS_GO);
    DEF_PY_CONST(DEBUG_STATUS_GO_HANDLED);
    DEF_PY_CONST(DEBUG_STATUS_GO_NOT_HANDLED);
    DEF_PY_CONST(DEBUG_STATUS_STEP_OVER);
    DEF_PY_CONST(DEBUG_STATUS_STEP_INTO);
    DEF_PY_CONST(DEBUG_STATUS_BREAK);
    DEF_PY_CONST(DEBUG_STATUS_NO_DEBUGGEE);
    DEF_PY_CONST(DEBUG_STATUS_STEP_BRANCH);
    DEF_PY_CONST(DEBUG_STATUS_IGNORE_EVENT);
    DEF_PY_CONST(DEBUG_STATUS_RESTART_REQUESTED);
    DEF_PY_CONST(DEBUG_STATUS_REVERSE_GO);
    DEF_PY_CONST(DEBUG_STATUS_REVERSE_STEP_BRANCH);
    DEF_PY_CONST(DEBUG_STATUS_REVERSE_STEP_OVER);
    DEF_PY_CONST(DEBUG_STATUS_REVERSE_STEP_INTO);

    // debug status additional mask
    DEF_PY_CONST(DEBUG_STATUS_INSIDE_WAIT);
    DEF_PY_CONST(DEBUG_STATUS_WAIT_TIMEOUT);

    // break point type
    DEF_PY_CONST(DEBUG_BREAKPOINT_CODE);
    DEF_PY_CONST(DEBUG_BREAKPOINT_DATA);
    DEF_PY_CONST(DEBUG_BREAKPOINT_TIME);

    // break point flag
    DEF_PY_CONST(DEBUG_BREAKPOINT_GO_ONLY);
    DEF_PY_CONST(DEBUG_BREAKPOINT_DEFERRED);
    DEF_PY_CONST(DEBUG_BREAKPOINT_ENABLED);
    DEF_PY_CONST(DEBUG_BREAKPOINT_ADDER_ONLY);
    DEF_PY_CONST(DEBUG_BREAKPOINT_ONE_SHOT);

    // break point access type
    DEF_PY_CONST(DEBUG_BREAK_READ);
    DEF_PY_CONST(DEBUG_BREAK_WRITE);
    DEF_PY_CONST(DEBUG_BREAK_EXECUTE);
    DEF_PY_CONST(DEBUG_BREAK_IO);

    // exception flags
    DEF_PY_CONST(EXCEPTION_NONCONTINUABLE);

    // debug events
    DEF_PY_CONST(DEBUG_EVENT_BREAKPOINT);
    DEF_PY_CONST(DEBUG_EVENT_EXCEPTION);
    DEF_PY_CONST(DEBUG_EVENT_CREATE_THREAD);
    DEF_PY_CONST(DEBUG_EVENT_EXIT_THREAD);
    DEF_PY_CONST(DEBUG_EVENT_CREATE_PROCESS);
    DEF_PY_CONST(DEBUG_EVENT_EXIT_PROCESS);
    DEF_PY_CONST(DEBUG_EVENT_LOAD_MODULE);
    DEF_PY_CONST(DEBUG_EVENT_UNLOAD_MODULE);
    DEF_PY_CONST(DEBUG_EVENT_SYSTEM_ERROR);
    DEF_PY_CONST(DEBUG_EVENT_SESSION_STATUS);
    DEF_PY_CONST(DEBUG_EVENT_CHANGE_DEBUGGEE_STATE);
    DEF_PY_CONST(DEBUG_EVENT_CHANGE_ENGINE_STATE);
    DEF_PY_CONST(DEBUG_EVENT_CHANGE_SYMBOL_STATE);
    
    // debugger type
    //_DEF_PY_CONST(DEBUG_CLASS_UNINITIALIZED);
    //_DEF_PY_CONST(DEBUG_CLASS_KERNEL);
    //_DEF_PY_CONST(DEBUG_CLASS_USER_WINDOWS);
    //_DEF_PY_CONST(DEBUG_CLASS_IMAGE_FILE);
    //
    //_DEF_PY_CONST(DEBUG_KERNEL_CONNECTION);
    //_DEF_PY_CONST(DEBUG_KERNEL_LOCAL);
    //_DEF_PY_CONST(DEBUG_KERNEL_EXDI_DRIVER);
    //_DEF_PY_CONST(DEBUG_KERNEL_IDNA);
    //_DEF_PY_CONST(DEBUG_KERNEL_SMALL_DUMP); 
    //_DEF_PY_CONST(DEBUG_KERNEL_DUMP); 
    //_DEF_PY_CONST(DEBUG_KERNEL_FULL_DUMP);

    //_DEF_PY_CONST(DEBUG_USER_WINDOWS_PROCESS);    
    //_DEF_PY_CONST(DEBUG_USER_WINDOWS_PROCESS_SERVER);
    //_DEF_PY_CONST(DEBUG_USER_WINDOWS_IDNA);
    //_DEF_PY_CONST(DEBUG_USER_WINDOWS_SMALL_DUMP);
    //_DEF_PY_CONST(DEBUG_USER_WINDOWS_SMALL_DUMP);
    //_DEF_PY_CONST(DEBUG_USER_WINDOWS_DUMP);
    //_DEF_PY_CONST(DEBUG_USER_WINDOWS_DUMP_WINDOWS_CE);
}

#undef  _DEF_PY_CONST

/////////////////////////////////////////////////////////////////////////////////

class WindbgGlobalSession 
{
public:
    
    static
    boost::python::object
    global() {
        return windbgGlobalSession->main.attr("__dict__");
    }
    
    static 
    VOID
    StartWindbgSession() {
        if ( 1 == InterlockedIncrement( &sessionCount ) )
        {
            windbgGlobalSession = new WindbgGlobalSession();
        }
    }
    
    static
    VOID
    StopWindbgSession() {
        if ( 0 == InterlockedDecrement( &sessionCount ) )
        {
            delete windbgGlobalSession;
            windbgGlobalSession = NULL;
        }            
    }
    
    static
    bool isInit() {
        return windbgGlobalSession != NULL;
    }
    

private:

    WindbgGlobalSession() {
                 
        PyImport_AppendInittab("pykd", initpykd ); 

        Py_Initialize();    
    
        main = boost::python::import("__main__");
        
        boost::python::object   main_namespace = main.attr("__dict__");


        // делаем аналог from pykd import *        
        boost::python::object   pykd = boost::python::import( "pykd" );
        
        boost::python::dict     pykd_namespace( pykd.attr("__dict__") ); 
        
        boost::python::list     iterkeys( pykd_namespace.iterkeys() );
        
        for (int i = 0; i < boost::python::len(iterkeys); i++)
        {
            std::string     key = boost::python::extract<std::string>(iterkeys[i]);
                   
            main_namespace[ key ] = pykd_namespace[ key ];
        }            
        
        //// перенаправление стандартных потоков ВВ
        //boost::python::object       sys = boost::python::import( "sys");
        //
        //dbgOut                      dout;
        //sys.attr("stdout") = boost::python::object( dout );

        //dbgIn                       din;
        //sys.attr("stdin") = boost::python::object( din );
        //
        g_dbgClient.startEventsMgr();
    }
    
    ~WindbgGlobalSession() {
        Py_Finalize();
        g_dbgClient.removeEventsMgr();
    }
   
    boost::python::object           main;

    static volatile LONG            sessionCount;      
    
    static WindbgGlobalSession      *windbgGlobalSession;     
};   

volatile LONG            WindbgGlobalSession::sessionCount = 0;

WindbgGlobalSession     *WindbgGlobalSession::windbgGlobalSession = NULL; 

bool isWindbgExt() {
    return WindbgGlobalSession::isInit();
}

/////////////////////////////////////////////////////////////////////////////////

HRESULT
CALLBACK
DebugExtensionInitialize(
    OUT PULONG  Version,
    OUT PULONG  Flags )
{
    *Version = DEBUG_EXTENSION_VERSION( 1, 0 );
    *Flags = 0;
 
    WindbgGlobalSession::StartWindbgSession();
    
    return S_OK;
}


VOID
CALLBACK
DebugExtensionUninitialize()
{
    WindbgGlobalSession::StopWindbgSession();
}

DbgExt::DbgExt( IDebugClient4 *masterClient ) 
{
    client = NULL;
    masterClient->QueryInterface( __uuidof(IDebugClient), (void **)&client );
  
    client4 = NULL;
    masterClient->QueryInterface( __uuidof(IDebugClient4), (void **)&client4 );
    
    client5 = NULL;
    masterClient->QueryInterface( __uuidof(IDebugClient5), (void **)&client5 );    
   
    control = NULL; 
    masterClient->QueryInterface( __uuidof(IDebugControl), (void **)&control );
    
    control4 = NULL;
    masterClient->QueryInterface( __uuidof(IDebugControl4), (void **)&control4 );
    
    registers = NULL;
    masterClient->QueryInterface( __uuidof(IDebugRegisters), (void **)&registers );
    
    symbols = NULL;
    masterClient->QueryInterface( __uuidof(IDebugSymbols), (void ** )&symbols );
    
    symbols2 = NULL;
    masterClient->QueryInterface( __uuidof(IDebugSymbols2), (void ** )&symbols2 );    
    
    symbols3 = NULL;
    masterClient->QueryInterface( __uuidof(IDebugSymbols3), (void ** )&symbols3 );      
    
    dataSpaces = NULL;
    masterClient->QueryInterface( __uuidof(IDebugDataSpaces), (void **)&dataSpaces );
    
    dataSpaces4 = NULL;
    masterClient->QueryInterface( __uuidof(IDebugDataSpaces4), (void **)&dataSpaces4 );
    
    advanced2 = NULL;
    masterClient->QueryInterface( __uuidof(IDebugAdvanced2), (void **)&advanced2 );
    
    system = NULL;
    masterClient->QueryInterface( __uuidof(IDebugSystemObjects), (void**)&system );
    
    system2 = NULL;
    masterClient->QueryInterface( __uuidof(IDebugSystemObjects2), (void**)&system2 );

    m_previosExt = dbgExt;
    dbgExt = this;
}

DbgExt::~DbgExt()
{
    BOOST_ASSERT(dbgExt == this);
    dbgExt = m_previosExt;

    if ( client )
        client->Release();

    if ( client4 )
        client4->Release();

    if ( client5 )
        client5->Release();

    if ( control )
        control->Release();

    if ( control4 )
        control4->Release();

    if ( registers )
        registers->Release();
            
    if ( symbols )
        symbols->Release();
        
    if ( symbols2 )
        symbols2->Release();

    if ( symbols3 )
        symbols3->Release();

    if ( dataSpaces )
        dataSpaces->Release();

    if ( dataSpaces4 )
        dataSpaces4->Release();

    if ( advanced2 )
        advanced2->Release();

    if ( system )
        system->Release();

    if ( system2 )
        system2->Release();
}

/////////////////////////////////////////////////////////////////////////////////    
    
HRESULT 
CALLBACK
py( PDEBUG_CLIENT4 client, PCSTR args)
{
    DbgExt      ext( client );

    PyThreadState   *globalInterpreter = PyThreadState_Swap( NULL );
    PyThreadState   *localInterpreter = Py_NewInterpreter();

    try {

        boost::python::import( "pykd" ); 

        boost::python::object       main =  boost::python::import("__main__");

        boost::python::object       global(main.attr("__dict__"));
        
        // перенаправление стандартных потоков ВВ
        boost::python::object       sys = boost::python::import("sys");

        boost::python::object       tracebackModule = boost::python::import("traceback");
        
        dbgOut                      dout;
        sys.attr("stdout") = boost::python::object( dout );

        dbgIn                       din;
        sys.attr("stdin") = boost::python::object( din );   
       
        // разбор параметров
        typedef  boost::escaped_list_separator<char>    char_separator_t;
        typedef  boost::tokenizer< char_separator_t >   char_tokenizer_t;  
        
        std::string                 argsStr( args );
        
        char_tokenizer_t            token( argsStr , char_separator_t( "", " \t", "\"" ) );
        std::vector<std::string>    argsList;
        
        for ( char_tokenizer_t::iterator   it = token.begin(); it != token.end(); ++it )
        {
            if ( *it != "" )
                argsList.push_back( *it );
        }            
            
        if ( argsList.size() == 0 )
            return S_OK;      
            
        char    **pythonArgs = new char* [ argsList.size() ];
     
        for ( size_t  i = 0; i < argsList.size(); ++i )
            pythonArgs[i] = const_cast<char*>( argsList[i].c_str() );
            
        PySys_SetArgv( (int)argsList.size(), pythonArgs );

        delete[]  pythonArgs;            

        
        // найти путь к файлу
        std::string     fullFileName;
        std::string     filePath;
        DbgPythonPath   dbgPythonPath;
        
        
        if ( dbgPythonPath.findPath( argsList[0], fullFileName, filePath ) )
        {
            try {             
          
                boost::python::object       result;
        
                result =  boost::python::exec_file( fullFileName.c_str(), global, global );
            }                
            catch( boost::python::error_already_set const & )
            {
                // ошибка в скрипте
                PyObject    *errtype = NULL, *errvalue = NULL, *traceback = NULL;
                
                PyErr_Fetch( &errtype, &errvalue, &traceback );
                
                if(errvalue != NULL) 
                {
                    PyObject *errvalueStr= PyObject_Str(errvalue);

                    dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "%s\n", PyString_AS_STRING( errvalueStr ) );

                    if ( traceback )
                    {
                        boost::python::object    traceObj( boost::python::handle<>( boost::python::borrowed( traceback ) ) );
                        
                        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "\nTraceback:\n" );

                        boost::python::object   pFunc( tracebackModule.attr("format_tb") );
                        boost::python::list     traceList( pFunc( traceObj ) );

                        for ( long i = 0; i < boost::python::len(traceList); ++i )
                        {
                            std::string     traceLine = boost::python::extract<std::string>(traceList[i]);
                            dbgExt->control->Output( DEBUG_OUTPUT_ERROR, traceLine.c_str() );
                        }
                    }

                    Py_DECREF(errvalueStr);
                }

                Py_XDECREF(errvalue);
                Py_XDECREF(errtype);
                Py_XDECREF(traceback);        
            }  
        }
        else
        {
      		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "script file not found\n" );
        }  
    }
   
    catch(...)
    {      
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "unexpected error" );         
    }     
    
    Py_EndInterpreter( localInterpreter ); 
    PyThreadState_Swap( globalInterpreter );
    
    return S_OK;  
}

/////////////////////////////////////////////////////////////////////////////////  

HRESULT 
CALLBACK
pycmd( PDEBUG_CLIENT4 client, PCSTR args )
{
    DbgExt      ext( client );

    try {
        
        // перенаправление стандартных потоков ВВ
        boost::python::object       sys = boost::python::import("sys");
        
        dbgOut                      dout;
        sys.attr("stdout") = boost::python::object( dout );

        dbgIn                       din;
        sys.attr("stdin") = boost::python::object( din );    
        
        boost::python::object   syntaxError = boost::python::import("exceptions").attr("SyntaxError");

        boost::python::object   tracebackModule = boost::python::import("traceback");

        std::string             commandBuffer;

        bool                    commandCompleted = true;

        do {

            if ( commandCompleted )
            {
                dbgExt->control->Output( DEBUG_OUTPUT_NORMAL, ">>>" );
            }
            else
            {
                dbgExt->control->Output( DEBUG_OUTPUT_NORMAL, "..." );
            }

            {
                char        str[100];
                ULONG       inputSize;

                OutputReader     outputReader( (IDebugClient*)client );
                HRESULT   hres = dbgExt->control->Input( str, sizeof(str), &inputSize );
                
                if ( FAILED( hres ) ) 
                    throw;

                if ( commandCompleted )
                {
                    if ( std::string( str ) == "" )
                        break;  

                    commandBuffer = str;
                }
                else
                {
                    if ( std::string( str ) == "" )
                        commandCompleted = true;
                    else
                    {
                       commandBuffer.append("\n"); 
                       commandBuffer.append( str );  
                    }
                }
            }

            if ( commandCompleted )
            {
                try {

                    boost::python::exec( commandBuffer.c_str(), WindbgGlobalSession::global(), WindbgGlobalSession::global() );
                        
                    commandBuffer.clear();
                        
                }
                catch( boost::python::error_already_set const & )
                {
                    PyObject    *errtype = NULL, *errvalue = NULL, *traceback = NULL, *errvalueStr = NULL;
                    
                    PyErr_Fetch( &errtype, &errvalue, &traceback );

                    if ( errtype && errvalue )
                    {
                        errvalueStr = PyObject_Str(errvalue);
    
                        do {
                        
                            if ( PyErr_GivenExceptionMatches( syntaxError.ptr(), errtype ) )
                            {
                                boost::python::tuple   errValueObj( boost::python::handle<>( boost::python::borrowed(errvalue) ) );
                            
                                if ( errValueObj[0] == "unexpected EOF while parsing" )
                                {
                                    commandCompleted = false;       
                                    break;
                                }
                            }
               
                            dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "%s\n", PyString_AS_STRING(errvalueStr)  );

                            if ( traceback )
                            {
                                boost::python::object    traceObj( boost::python::handle<>( boost::python::borrowed( traceback ) ) );
                                
                                dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "\nTraceback:\n" );

                                boost::python::object   pFunc( tracebackModule.attr("format_tb") );
                                boost::python::list     traceList( pFunc( traceObj ) );

                                for ( long i = 0; i < boost::python::len(traceList); ++i )
                                {
                                    std::string     traceLine = boost::python::extract<std::string>(traceList[i]);
                                    dbgExt->control->Output( DEBUG_OUTPUT_ERROR, traceLine.c_str() );
                                }
                            }

                        } while( FALSE );

                        Py_XDECREF(errvalueStr);   
                        Py_XDECREF(errvalue);
                        Py_XDECREF(errtype);
                        Py_XDECREF(traceback);
                    }
                    else
                    {
                        Py_XDECREF(errvalue);
                        Py_XDECREF(errtype);
                        Py_XDECREF(traceback);

                        throw;
                    }
                }
            }

        } while( true );
    } 
    catch(...)    
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "unexpected error" );
    }
    
    return S_OK;    
}

///////////////////////////////////////////////////////////////////////////////// 

HRESULT 
CALLBACK
pythonpath( PDEBUG_CLIENT4 client, PCSTR args )
{
    //DbgExt      ext;

    //SetupDebugEngine( client, &ext );  
    //dbgExt = &ext;

    ////DbgPrint::dprintln( dbgPythonPath.getStr() );

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////// 

