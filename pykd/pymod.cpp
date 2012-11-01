// 
// Python module initialization
// 

#include "stdafx.h"

#include "pykdver.h"

#include "dbgengine.h"
#include "symengine.h"

#include "module.h"
#include "variant.h"
#include "dbgexcept.h"
#include "dbgmem.h"
#include "typeinfo.h"
#include "customtypes.h"
#include "typedvar.h"
#include "cpureg.h"
#include "disasm.h"
#include "stkframe.h"
#include "localvar.h"
#include "bpoint.h"

#include "win/dbgio.h"
#include "win/windbg.h"

using namespace pykd;

////////////////////////////////////////////////////////////////////////////////


static const std::string pykdVersion = PYKD_VERSION_BUILD_STR
#ifdef _DEBUG
    " <DBG>"
#endif  // _DEBUG
;

////////////////////////////////////////////////////////////////////////////////


BOOST_PYTHON_FUNCTION_OVERLOADS( dprint_, dprint, 1, 2 );
BOOST_PYTHON_FUNCTION_OVERLOADS( dprintln_, dprintln, 1, 2 );

BOOST_PYTHON_FUNCTION_OVERLOADS( loadChars_, loadChars, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadWChars_, loadWChars, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadBytes_, loadBytes, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadWords_, loadWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadDWords_, loadDWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadQWords_, loadQWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignBytes_, loadSignBytes, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignWords_, loadSignWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignDWords_, loadSignDWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignQWords_, loadSignQWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( compareMemory_, compareMemory, 3, 4 );

BOOST_PYTHON_FUNCTION_OVERLOADS( getSourceLine_, getSourceLine, 0, 1 );
BOOST_PYTHON_FUNCTION_OVERLOADS( getSourceFile_, getSourceFile, 0, 1 );

BOOST_PYTHON_FUNCTION_OVERLOADS( setSoftwareBp_, setSoftwareBp, 1, 2 );
BOOST_PYTHON_FUNCTION_OVERLOADS( setHardwareBp_, setHardwareBp, 3, 4 );

BOOST_PYTHON_FUNCTION_OVERLOADS( CustomStruct_create, CustomStruct::create, 1, 2 );

BOOST_PYTHON_FUNCTION_OVERLOADS( findSymbol_, TypeInfo::findSymbol, 1, 2 );

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( Module_enumSymbols, Module::enumSymbols, 0, 1 );
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( Module_enumTypes, Module::enumTypes, 0, 1 );


BOOST_PYTHON_MODULE( pykd )
{
    python::scope().attr("version") = pykdVersion;

    // DbgEng services 
    python::def( "setSymSrvDir", &setSymSrvDir,
        "Set directory of SYMSRV.dll library.\nUsually this is a directory of WinDbg");
    python::def( "loadExt", &loadExtension,
        "Load a WinDBG extension. Return handle of the loaded extension" );
    python::def( "removeExt", &removeExtension,
        "Unload a WinDBG extension. Parameters: handle returned by loadExt" );
    python::def( "callExt", &callExtension,
        "Call a WinDBG extension's routine. Parameters: handle returned by loadExt; string command line" );


    // Manage debug target 

    python::def( "startProcess", &startProcess,
        "Start process for debugging"); 
    python::def( "attachProcess", &attachProcess,
        "Attach debugger to a exsisting process" );
    python::def( "detachProcess", &detachProcess, 
        "Stop process debugging"); 
    python::def( "killProcess", &terminateProcess,
        "Stop debugging and terminate current process" );
    python::def( "loadDump", &loadDump,
        "Load crash dump");
    python::def( "isDumpAnalyzing", &isDumpAnalyzing,
        "Check if it is a dump analyzing ( not living debuggee )" );
    python::def( "isKernelDebugging", &isKernelDebugging,
        "Check if kernel dubugging is running" );
    python::def( "isWindbgExt", &WindbgGlobalSession::isInit,
        "Check if script works in windbg context" );

    python::def( "breakin", &debugBreak,
        "Break into debugger" );
    python::def( "expr", &evaluate,
        "Evaluate windbg expression" );
    python::def( "dbgCommand", &debugCommand,
        "Run a debugger's command and return it's result as a string" );
    python::def( "go", &debugGo,
        "Go debugging"  );
    python::def( "step", &debugStep,
        "The target is executing a single instruction or--if that instruction is a subroutine call--subroutine" );
    python::def( "trace", &debugStepIn,
        "The target is executing a single instruction" );

    // Debug output
    python::def( "dprint", &pykd::dprint, dprint_( boost::python::args( "str", "dml" ), 
        "Print out string. If dml = True string is printed with dml highlighting ( only for windbg )" ) );
    python::def( "dprintln", &pykd::dprintln, dprintln_( boost::python::args( "str", "dml" ), 
        "Print out string and insert end of line symbol. If dml = True string is printed with dml highlighting ( only for windbg )" ) );

    // Python debug output console helper classes
    python::class_<DbgOut>( "dout", "dout", python::no_init )
        .def( "write", &DbgOut::write );
    python::class_<DbgErr>( "dout", "dout", python::no_init )
        .def( "write", &DbgErr::write );
    python::class_<DbgIn>( "din", "din", python::no_init )
        .def( "readline", &DbgIn::readline );

    // system properties
    python::def( "ptrSize", &ptrSize,
        "Return effective pointer size" );
    python::def( "is64bitSystem", &is64bitSystem,
       "Check if target system has 64 address space" );
    python::def( "pageSize", &getPageSize,
        "Get the page size for the currently executing processor context" );

    // Manage target memory access
    python::def( "addr64", &addr64,
        "Extend address to 64 bits formats" );
    python::def( "isValid", &isVaValid,
        "Check if the virtual address is valid" );
    python::def( "compareMemory", &compareMemory, compareMemory_( python::args( "offset1", "offset2", "length", "phyAddr" ),
        "Compare two memory buffers by virtual or physical addresses" ) );

    python::def( "ptrByte", &ptrByte,
        "Read an unsigned 1-byte integer from the target memory" );
    python::def( "ptrWord", &ptrWord,
        "Read an unsigned 2-byte integer from the target memory" );
    python::def( "ptrDWord", (ULONG64(*)(ULONG64))&ptrDWord,
        "Read an unsigned 4-byte integer from the target memory" );
    python::def( "ptrQWord", (ULONG64(*)(ULONG64))&ptrQWord,
        "Read an unsigned 8-byte integer from the target memory" );
    python::def( "ptrMWord", (ULONG64(*)(ULONG64))&ptrMWord,
        "Read an unsigned mashine's word wide integer from the target memory" );
    python::def( "ptrSignByte", &ptrSignByte,
        "Read an signed 1-byte integer from the target memory" );
    python::def( "ptrSignWord", &ptrSignWord,
        "Read an signed 2-byte integer from the target memory" );
    python::def( "ptrSignDWord", &ptrSignDWord,
        "Read an signed 4-byte integer from the target memory" );
    python::def( "ptrSignQWord", &ptrSignQWord,
        "Read an signed 8-byte integer from the target memory" );
    python::def( "ptrSignMWord", &ptrSignMWord,
        "Read an signed mashine's word wide integer from the target memory" );
    python::def( "ptrPtr", (ULONG64(*)(ULONG64))&ptrPtr,
        "Read an pointer value from the target memory" );

    python::def( "loadBytes", &loadBytes, loadBytes_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as liat of unsigned bytes" ) );
    python::def( "loadWords", &loadWords, loadWords_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of unsigned shorts" ) );
    python::def( "loadDWords", &loadDWords, loadDWords_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of unsigned long ( double word )" ) );
    python::def( "loadQWords", &loadQWords, loadQWords_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of unsigned long long ( quad word )" ) );
    python::def( "loadSignBytes", &loadSignBytes, loadSignBytes_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of signed bytes" ) );
    python::def( "loadSignWords", &loadSignWords, loadSignWords_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of signed words" ) );
    python::def( "loadSignDWords", &loadSignDWords, loadSignDWords_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of signed longs" ) );
    python::def( "loadSignQWords", &loadSignQWords, loadSignQWords_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of signed long longs" ) );
    python::def( "loadChars", &loadChars, loadChars_( python::args( "address", "count", "phyAddr" ),
        "Load string from target memory" ) );
    python::def( "loadWChars", &loadWChars, loadWChars_( python::args( "address", "count", "phyAddr" ),
        "Load string from target memory" ) );
    python::def( "loadCStr", &loadCStr,
        "Load string from the target buffer containing 0-terminated ansi-string" );
    python::def( "loadWStr", &loadWStr,
        "Load string from the target buffer containing 0-terminated unicode-string" );
    python::def( "loadUnicodeString", &loadUnicodeStr,
        "Return string represention of windows UNICODE_STRING type" );
    python::def( "loadAnsiString", &loadAnsiStr,
        "Return string represention of windows ANSI_STRING type" );
    python::def( "loadPtrList", &loadPtrList,
        "Return list of pointers, each points to next" );
    python::def( "loadPtrs", &loadPtrArray,
        "Read the block of the target's memory and return it as a list of pointers" );

    // types and vaiables
    python::def( "getSourceFile", &getSourceFile, getSourceFile_( python::args( "offset"),
        "Return source file by the specified offset" ) );
    python::def( "getSourceLine", &getSourceLine, getSourceLine_( python::args( "offset"),
        "Return source file name, line and displacement by the specified offset" ) );
    python::def( "getOffset", &TypeInfo::getOffset,
        "Return traget virtual address for specified symbol" );
    python::def( "findSymbol", &TypeInfo::findSymbol, findSymbol_( python::args( "offset", "safe"),
        "Find symbol by the target virtual memory offset" ) );
    python::def( "sizeof", &TypeInfo::getSymbolSize,
        "Return a size of the type or variable" );
    python::def("typedVarList", &getTypedVarListByTypeName,
        "Return a list of the typedVar class instances. Each item represents an item of the linked list in the target memory" );
    python::def("typedVarList", &getTypedVarListByType,
        "Return a list of the typedVar class instances. Each item represents an item of the linked list in the target memory" );
    python::def("typedVarArray", &getTypedVarArrayByTypeName,
        "Return a list of the typedVar class instances. Each item represents an item of the counted array in the target memory" );
    python::def("typedVarArray", &getTypedVarArrayByType,
        "Return a list of the typedVar class instances. Each item represents an item of the counted array in the target memory" );
    python::def("containingRecord", &containingRecordByName,
        "Return instance of the typedVar class. It's value are loaded from the target memory."
        "The start address is calculated by the same method as the standard macro CONTAINING_RECORD does" );
    python::def("containingRecord", &containingRecordByType,
        "Return instance of the typedVar class. It's value are loaded from the target memory."
        "The start address is calculated by the same method as the standard macro CONTAINING_RECORD does" );

    // CPU registers
    python::def( "reg", &getRegByName,
        "Return a CPU regsiter value by the register's name" );
    python::def( "reg", &getRegByIndex,
        "Return a CPU regsiter value by the register's value" );
    python::def ( "rdmsr", &loadMSR,
        "Return MSR value" );
    python::def( "wrmsr", &setMSR,
        "Set MSR value" );
    python::def( "getProcessorMode", &getProcessorMode, 
        "Return current processor mode as string: X86, ARM, IA64 or X64" );
    python::def( "getProcessorType", &getProcessorType,
        "Return type of physical processor: X86, ARM, IA64 or X64" );

    // stack and local variables
    python::def( "getCurrentStack", &getCurrentStack,
        "Return a current stack as a list of stackFrame objects" );
    python::def( "getLocals", &getLocals, "Get list of local variables" );

    // breakpoints
    python::def( "setBp", &setSoftwareBp, setSoftwareBp_( python::args( "offset", "callback" ),
        "Set software breakpoint on executiont" ) );
    python::def( "setBp", &setHardwareBp, setHardwareBp_( python::args( "offset", "size", "accsessType", "callback" ) ,
        "Set hardware breakpoint" ) );
    python::def( "removeBp", &removeBp,
        "Remove breapoint by IDs" );
    //python::def( "removeAllBp", &removeAllBp,
    //    "Remove all breapoints" );

    // processes and threads
    python::def( "getCurrentProcess", &getCurrentProcess,
        "Return pointer to current process's block" );
    python::def( "getImplicitThread", &getImplicitThread, 
        "Return implicit thread for current process" );
    python::def( "setCurrentProcess", &setCurrentProcess, 
        "Set current process by address" );
    python::def( "setImplicitThread", &setImplicitThread, 
        "Set implicit thread for current process" );
    
    // symbol path
    python::def( "getSymbolPath", &getSymbolPath, "Returns current symbol path");
    python::def( "setSymbolPath", &setSymbolPath, "Set current symbol path");
    python::def( "appendSymbolPath", &appendSymbolPath, "Append current symbol path");

    // custom types
    python::def( "createStruct", &CustomStruct::create, CustomStruct_create( python::args( "name", "align" ), 
        "Create empty structure. Use append() method for building" ) );
    python::def( "createUnion", &CustomUnion::create,
        "Create empty union. Use append() method for building" );
    python::def( "pVoid", &PtrToVoid,
        "Create \"Void *\" type" );

    python::class_<intBase>( "intBase", "intBase", python::no_init )
        .def( python::init<python::object&>() )
        .def( "__eq__", &intBase::eq )
        .def( "__ne__", &intBase::ne)
        .def( "__lt__", &intBase::lt)
        .def( "__gt__", &intBase::gt )
        .def( "__le__", &intBase::le )
        .def( "__ge__", &intBase::ge )
        .def( "__add__", &intBase::add )
        .def( "__radd__", &intBase::add )
        .def( "__sub__", &intBase::sub )
        .def( "__rsub__", &intBase::rsub )
        .def( "__mul__", &intBase::mul )
        .def( "__rmul__", &intBase::mul )
        .def( "__div__", &intBase::div )
        .def( "__rdiv__", &intBase::rdiv )
        .def( "__mod__", &intBase::mod )
        .def( "__rmod__", &intBase::rmod )
        .def( "__rshift__", &intBase::rshift )
        .def( "__rrshift__", &intBase::rrshift )
        .def( "__lshift__", &intBase::lshift )
        .def( "__rlshift__", &intBase::rlshift )
        .def( "__and__", &intBase::and )
        .def( "__rand__", &intBase::and )
        .def( "__or__", &intBase::or )
        .def( "__ror__", &intBase::or )
        .def( "__xor__", &intBase::xor )
        .def( "__rxor__", &intBase::xor )
        .def( "__neg__", &intBase::neg )
        .def( "__pos__", &intBase::pos ) 
        .def( "__invert__", &intBase::invert ) 
        .def( "__nonzero__", &intBase::nonzero )
        .def( "__str__", &intBase::str )
        .def( "__hex__", &intBase::hex )
        .def( "__long__", &intBase::long_ )
        .def( "__int__", &intBase::int_ )
        .def( "__index__", &intBase::long_ )
        .def( "__hash__", &intBase::long_ );

    python::implicitly_convertible<intBase,ULONG64>();
    python::implicitly_convertible<intBase,LONG64>();
    python::implicitly_convertible<intBase,ULONG>();
    python::implicitly_convertible<intBase,LONG>();

    python::class_<Module, ModulePtr, python::bases<intBase> >("module", "Class representing executable module", python::no_init )
        .def("__init__", python::make_constructor(Module::loadModuleByName) )
        .def("__init__", python::make_constructor(Module::loadModuleByOffset) )
        .def("begin", &Module::getBase,
             "Return start address of the module" )
        .def("end", &Module::getEnd,
             "Return end address of the module" )
        .def("size", &Module::getSize,
              "Return size of the module" )
        .def("name", &Module::getName,
             "Return name of the module" )
        .def("reload", &Module::reloadSymbols,
            "(Re)load symbols for the module" )
        .def("image", &Module::getImageName,
            "Return name of the image of the module" )
        .def("symfile", &Module::getSymFile,
             "Return the full path to the module's symbol information" )
        .def("offset", &Module::getSymbolOffset,
            "Return offset of the symbol" )
        .def("findSymbol", &Module::getSymbolNameByVa,
            "Return symbol name by virtual address" )
        .def("rva", &Module::getSymbolRva,
            "Return rva of the symbol" )
        .def("sizeof", &Module::getSymbolSize,
            "Return a size of the type or variable" )
        .def("type", &Module::getTypeByName,
            "Return typeInfo class by type name" )
        .def("typedVar", &Module::getTypedVarByAddr,
            "Return a typedVar class instance" )
        .def("typedVar",&Module::getTypedVarByName,
            "Return a typedVar class instance" )
        .def("typedVar",&Module::getTypedVarByTypeName,
            "Return a typedVar class instance" )
        .def("typedVarList", &Module::getTypedVarListByTypeName,
            "Return a list of the typedVar class instances. Each item represents an item of the linked list in the target memory" )
        .def("typedVarArray", &Module::getTypedVarArrayByTypeName,
            "Return a list of the typedVar class instances. Each item represents an item of the counted array in the target memory" )
        .def("containingRecord", &Module::containingRecordByName,
            "Return instance of the typedVar class. It's value are loaded from the target memory."
            "The start address is calculated by the same method as the standard macro CONTAINING_RECORD does" )
        .def("enumSymbols", &Module::enumSymbols, Module_enumSymbols( python::args("mask"),
             "Return list of tuple ( symbolname, offset )" ) )
        .def("enumTypes", &Module::enumTypes, Module_enumTypes( python::args("mask"),
            "Return list of type's names" ))
        .def("checksum",&Module::getCheckSum,
            "Return a image file checksum: IMAGE_OPTIONAL_HEADER.CheckSum" )
        .def("timestamp",&Module::getTimeDataStamp,
            "Return a low 32 bits of the time stamp of the image: IMAGE_FILE_HEADER.TimeDateStamp" )
        .def("__getattr__", &Module::getSymbolOffset,
            "Return address of the symbol" )
        .def( "__str__", &Module::print );

    python::class_<TypeInfo, TypeInfoPtr, python::bases<intBase>, boost::noncopyable >("typeInfo", "Class representing typeInfo", python::no_init )
        .def("__init__", python::make_constructor(TypeInfo::getTypeInfoByName ) )
        .def( "name", &TypeInfo::getName )
        .def( "size", &TypeInfo::getSize )
        .def( "staticOffset", &TypeInfo::getStaticOffset )
        .def( "fieldOffset", &TypeInfo::getFieldOffsetByNameRecirsive )
        .def( "bitOffset", &TypeInfo::getBitOffset )
        .def( "bitWidth", &TypeInfo::getBitWidth )
        .def( "field", &TypeInfo::getField )
        .def( "asMap", &TypeInfo::asMap )
        .def( "deref", &TypeInfo::deref )
        .def( "append", &TypeInfo::appendField )
        .def( "__str__", &TypeInfo::print )
        .def( "__getattr__", &TypeInfo::getField )
        .def("__len__", &TypeInfo::getElementCount )
        .def("__getitem__", &TypeInfo::getElementByIndex );

    python::class_<TypedVar, TypedVarPtr, python::bases<intBase>, boost::noncopyable >("typedVar", 
        "Class of non-primitive type object, child class of typeClass. Data from target is copied into object instance", python::no_init  )
        .def("__init__", python::make_constructor(TypedVar::getTypedVarByName) )
        .def("__init__", python::make_constructor(TypedVar::getTypedVarByTypeName) )
        .def("__init__", python::make_constructor(TypedVar::getTypedVarByTypeInfo) )
        .def("getAddress", &TypedVar::getAddress, 
            "Return virtual address" )
        .def("sizeof", &TypedVar::getSize,
            "Return size of a variable in the target memory" )
        .def("fieldOffset", &TypedVar::getFieldOffsetByNameRecirsive,
            "Return target field offset" )
        .def("field", &TypedVar::getField,
            "Return field of structure as an object attribute" )
        .def( "dataKind", &TypedVar::getDataKind,
            "Retrieves the variable classification of a data: DataIsXxx")
        .def("deref", &TypedVar::deref,
            "Return value by pointer" )
        .def("type", &TypedVar::getType,
            "Return typeInfo instance" )
        .def("__getattr__", &TypedVar::getField,
            "Return field of structure as an object attribute" )
        .def( "__str__", &TypedVar::print )
        .def("__len__", &TypedVar::getElementCount )
        .def("__getitem__", &TypedVar::getElementByIndex )
        .def("__getitem__", &TypedVar::getElementByIndexPtr );

    python::class_<CpuReg, python::bases<intBase> >( 
        "cpuReg", "CPU regsiter class", boost::python::no_init )
            .def( "name", &CpuReg::name, "The name of the regsiter" )
            .def( "index", &CpuReg::index, "The index of thr register" );

    python::class_<StackFrame>( "stackFrame", 
         "Class representing a frame of the call stack", python::no_init )
        .def_readonly( "instructionOffset", &StackFrame::m_instructionOffset,
            "Return a frame's instruction offset" )
        .def_readonly( "returnOffset", &StackFrame::m_returnOffset,
            "Return a frame's return offset" )
        .def_readonly( "frameOffset", &StackFrame::m_frameOffset,
            "Return a frame's offset" )
        .def_readonly( "stackOffset", &StackFrame::m_stackOffset,
            "Return a frame's stack offset" )
        .def_readonly( "frameNumber", &StackFrame::m_frameNumber,
            "Return a frame's number" )
        .def( "getLocals", &StackFrame::getLocals,
             "Get list of local variables for this stack frame" )
        .def( "__str__", &StackFrame::print,
            "Return stacks frame as a string");

    python::class_<Disasm>("disasm", "Class disassemble a processor instructions" )
        .def( python::init<>( "constructor" ) )
        .def( python::init<ULONG64>( boost::python::args("offset"), "constructor" ) )
        .def( "disasm", &Disasm::disassemble, "Disassemble next instruction" )
        .def( "disasm", &Disasm::jump, "Disassemble from the specified offset" )
        .def( "asm", &Disasm::assembly, "Insert assemblied instuction to current offset" )
        .def( "begin", &Disasm::begin, "Return begin offset" )
        .def( "current", &Disasm::current, "Return current offset" )
        .def( "length", &Disasm::length, "Return current instruction length" )
        .def( "instruction", &Disasm::instruction, "Returm current disassembled instruction" )
        .def( "ea", &Disasm::ea, "Return effective address for last disassembled instruction or 0" )
        .def( "reset", &Disasm::reset, "Reset current offset to begin" );

    python::class_<EventHandlerWrap, EventHandlerPtr, boost::noncopyable>(
        "eventHandler", "Base class for overriding and handling debug notifications" )
        .def( "onBreakpoint", &EventHandlerWrap::OnBreakpoint,
            "Triggered breakpoint event. Parameter is int: ID of breakpoint\n"
            "For ignore event method must return False value" )
        .def( "onModuleLoad", &EventHandlerWrap::OnModuleLoad,
            "Triggered module load event. Parameter are long: module base, string: module name\n"
            "For ignore event method must return False value" )
        .def( "onModuleUnload", &EventHandlerWrap::OnModuleUnload,
            "Triggered module unload event. Parameter are  long: module base, string: module name\n"
            "For ignore event method must return False value" )
        .def( "onException", &EventHandlerWrap::OnException,
            "Triggered exception event. Parameters are long: exception address, long: exception code\n"
            "For ignore event method must return False value" );

    // wrapper for standart python exceptions
    python::register_exception_translator<PyException>( &PyException::exceptionTranslate );

    pykd::exception<DbgException>( "BaseException", "Pykd base exception class" );
    pykd::exception<MemoryException,DbgException>( "MemoryException", "Target memory access exception class" );
    //pykd::exception<WaitEventException,DbgException>( "WaitEventException", "Debug interface access exception" );
    pykd::exception<SymbolException,DbgException>( "SymbolException", "Symbol exception" );
    //pykd::exception<pyDia::Exception,SymbolException>( "DiaException", "Debug interface access exception" );
    pykd::exception<TypeException,SymbolException>( "TypeException", "type exception" );
    //pykd::exception<AddSyntheticSymbolException,DbgException>( "AddSynSymbolException", "synthetic symbol exception" );
    //pykd::exception<ImplementException,DbgException>( "ImplementException", "implementation exception" );
}

//////////////////////////////////////////////////////////////////////////////////
