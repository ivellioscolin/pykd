
#include "stdafx.h"

#include <boost/bind.hpp>

#include "kdlib/kdlib.h"

#include "variant.h"
#include "module.h"
#include "dbgengine.h"
#include "dbgexcept.h"
#include "memaccess.h"
#include "typeinfo.h"
#include "typedvar.h"
#include "windbgext.h"
#include "eventhandler.h"
#include "cpucontext.h"

using namespace pykd;

///////////////////////////////////////////////////////////////////////////////

static const std::string pykdVersion = PYKD_VERSION_BUILD_STR
#ifdef _DEBUG
    " <DBG>"
#endif  // _DEBUG
;

///////////////////////////////////////////////////////////////////////////////

BOOST_PYTHON_FUNCTION_OVERLOADS( detachProcess_,  kdlib::detachProcess, 0, 1 );
BOOST_PYTHON_FUNCTION_OVERLOADS( terminateProcess_,  kdlib::terminateProcess, 0, 1 );

BOOST_PYTHON_FUNCTION_OVERLOADS( dprint_, kdlib::dprint, 1, 2 );
BOOST_PYTHON_FUNCTION_OVERLOADS( dprintln_, kdlib::dprintln, 1, 2 );

//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( Module_findSymbol, Module::getSymbolNameByVa, 1, 2 );

BOOST_PYTHON_FUNCTION_OVERLOADS( loadChars_, kdlib::loadChars, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadWChars_, kdlib::loadWChars, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadBytes_, loadBytes, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadWords_, loadWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadDWords_, loadDWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadQWords_, loadQWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignBytes_, loadSignBytes, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignWords_, loadSignWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignDWords_, loadSignDWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignQWords_, loadSignQWords, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadFloats_, loadFloats, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( loadDoubles_, loadDoubles, 2, 3 );
BOOST_PYTHON_FUNCTION_OVERLOADS( compareMemory_, kdlib::compareMemory, 3, 4 );


BOOST_PYTHON_FUNCTION_OVERLOADS( getSourceLine_, getSourceLine, 0, 1 );
BOOST_PYTHON_FUNCTION_OVERLOADS( getSourceFile_, kdlib::getSourceFile, 0, 1 );

//BOOST_PYTHON_FUNCTION_OVERLOADS( setHardwareBp_, setHardwareBp, 3, 4 );
//
//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( TypeBuilder_createStruct, TypeBuilder::createStruct, 1, 2 );
//
BOOST_PYTHON_FUNCTION_OVERLOADS( Module_enumSymbols, ModuleAdapter::enumSymbols, 1, 2 );
BOOST_PYTHON_FUNCTION_OVERLOADS( Module_findSymbol, ModuleAdapter::findSymbol, 2, 3 );

BOOST_PYTHON_FUNCTION_OVERLOADS( findSymbol_, TypeInfoAdapter::findSymbol, 1, 2 );

//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( Module_findSymbol, Module::getSymbolNameByVa, 1, 2 );


BOOST_PYTHON_MODULE( pykd )
{
    python::scope().attr("version") = pykdVersion;

   // // DbgEng services 
   // python::def( "setSymSrvDir", &setSymSrvDir,
   //     "Set directory of SYMSRV.dll library.\nUsually this is a directory of WinDbg");
   // python::def( "loadExt", &loadExtension,
   //     "Load a WinDBG extension. Return handle of the loaded extension" );
   // python::def( "removeExt", &removeExtension,
   //     "Unload a WinDBG extension. Parameters: handle returned by loadExt" );
   // python::def( "callExt", &callExtension,
   //     "Call a WinDBG extension's routine. Parameters: handle returned by loadExt; string command line" );


   // Manage debug target 

    python::def( "startProcess", &startProcess,
        "Start process for debugging" ); 
    python::def( "attachProcess", &attachProcess,
        "Attach debugger to a exsisting process" );
    python::def( "detachProcess", &kdlib::detachProcess, detachProcess_( boost::python::args( "pid" ),
        "Stop process debugging") ); 
    python::def( "killProcess", &kdlib::terminateProcess, terminateProcess_( boost::python::args( "pid" ),
        "Stop debugging and terminate current process" ) );
    python::def( "loadDump", &loadDump,
        "Load crash dump");
    python::def( "isDumpAnalyzing", &kdlib::isDumpAnalyzing,
        "Check if it is a dump analyzing ( not living debuggee )" );
    python::def( "isKernelDebugging", &kdlib::isKernelDebugging,
        "Check if kernel dubugging is running" );
    python::def( "isWindbgExt", &PykdExt::isInit,
        "Check if script works in windbg context" );
    python::def( "writeDump", &kdlib::writeDump,
        "Create memory dump file" );

    python::def( "breakin", &targetBreak,
        "Break into debugger" );
    python::def( "expr", &evaluate,
        "Evaluate windbg expression" );
    python::def( "dbgCommand", &debugCommand,
        "Run a debugger's command and return it's result as a string" );
    python::def( "go", &targetGo,
        "Go debugging"  );
    python::def( "step", &targetStep,
        "The target is executing a single instruction or--if that instruction is a subroutine call--subroutine" );
    python::def( "trace", &targetStepIn,
        "The target is executing a single instruction" );

   // Debug output
    python::def( "dprint", &kdlib::dprint, dprint_( python::args( "str", "dml" ), 
        "Print out string. If dml = True string is printed with dml highlighting ( only for windbg )" ) );
    python::def( "dprintln", &kdlib::dprintln, dprintln_( python::args( "str", "dml" ), 
        "Print out string and insert end of line symbol. If dml = True string is printed with dml highlighting ( only for windbg )" ) );

    // Python debug output console helper classes
    python::class_<kdlib::DbgOut, boost::noncopyable >( "dout", "dout", python::no_init )
        .def( "write", &kdlib::DbgOut::write );
    python::class_<kdlib::DbgIn, boost::noncopyable>( "din", "din", python::no_init )
        .def( "readline", &kdlib::DbgIn::readline );

   // system properties
    python::def( "ptrSize", &kdlib::ptrSize,
        "Return effective pointer size" );
    python::def( "is64bitSystem", &kdlib::is64bitSystem,
       "Check if target system has 64 address space" );
    python::def( "pageSize", &kdlib::getPageSize,
        "Get the page size for the currently executing processor context" );
    python::def( "systemUptime", &kdlib::getSystemUptime,
        "Return the number of seconds the computer has been running" );
    python::def( "currentTime", &kdlib::getCurrentTime,
        "Return the number of seconds since the beginning of 1970" );
    //python::def("getSystemVersion", &getSystemVersion,
    //    "Return systemVersion");

    // Manage target memory access
    python::def( "addr64", &kdlib::addr64,
        "Extend address to 64 bits formats" );
    python::def( "isValid", &kdlib::isVaValid,
        "Check if the virtual address is valid" );
    python::def( "compareMemory", &kdlib::compareMemory, compareMemory_( python::args( "offset1", "offset2", "length", "phyAddr" ),
        "Compare two memory buffers by virtual or physical addresses" ) );
    //python::def( "findMemoryRegion", &kdlib::findMemoryRegion,
    //    "Return address of begining valid memory region nearest to offset" );
    //python::def( "getVaProtect", &kdlib::getVaProtect,
    //    "Return memory attributes" );

    python::def( "ptrByte", &kdlib::ptrByte,
        "Read an unsigned 1-byte integer from the target memory" );
    python::def( "ptrWord", &kdlib::ptrWord,
        "Read an unsigned 2-byte integer from the target memory" );
    python::def( "ptrDWord", &kdlib::ptrDWord,
        "Read an unsigned 4-byte integer from the target memory" );
    python::def( "ptrQWord", &kdlib::ptrQWord,
        "Read an unsigned 8-byte integer from the target memory" );
    python::def( "ptrMWord", &kdlib::ptrMWord,
        "Read an unsigned mashine's word wide integer from the target memory" );
    python::def( "ptrSignByte", &ptrSignByte,
        "Read an signed 1-byte integer from the target memory" );
    python::def( "ptrSignWord", &kdlib::ptrSignWord,
        "Read an signed 2-byte integer from the target memory" );
    python::def( "ptrSignDWord", &kdlib::ptrSignDWord,
        "Read an signed 4-byte integer from the target memory" );
    python::def( "ptrSignQWord", &kdlib::ptrSignQWord,
        "Read an signed 8-byte integer from the target memory" );
    python::def( "ptrSignMWord", &kdlib::ptrSignMWord,
        "Read an signed mashine's word wide integer from the target memory" );
    python::def( "ptrFloat", &kdlib::ptrSingleFloat,
        "Read a float with single precision from the target memory" );
   python::def( "ptrDouble", &kdlib::ptrDoubleFloat,
        "Read a float with single precision from the target memory" );

    python::def( "loadBytes", &loadBytes, loadBytes_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of unsigned bytes" ) );
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
    python::def( "loadChars", &kdlib::loadChars, loadChars_( python::args( "address", "count", "phyAddr" ),
        "Load string from target memory" ) );
    python::def( "loadWChars", &kdlib::loadWChars, loadWChars_( python::args( "address", "count", "phyAddr" ),
        "Load string from target memory" ) );
    python::def( "loadCStr", &kdlib::loadCStr,
        "Load string from the target buffer containing 0-terminated ansi-string" );
    python::def( "loadWStr", &kdlib::loadWStr,
        "Load string from the target buffer containing 0-terminated unicode-string" );
    //python::def( "loadUnicodeString", &loadUnicodeStr,
    //    "Return string represention of windows UNICODE_STRING type" );
    //python::def( "loadAnsiString", &loadAnsiStr,
    //    "Return string represention of windows ANSI_STRING type" );
    python::def( "loadFloats", &loadFloats, loadFloats_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of floats" ) );
    python::def( "loadDoubles", &loadDoubles, loadDoubles_( python::args( "offset", "count", "phyAddr" ),
        "Read the block of the target's memory and return it as list of doubles" ) );

    python::def( "ptrPtr", &ptrPtr,
        "Read an pointer value from the target memory" );
    python::def( "loadPtrList", &loadPtrList,
        "Return list of pointers, each points to next" );
    python::def( "loadPtrs", &loadPtrArray,
        "Read the block of the target's memory and return it as a list of pointers" );

    // types and vaiables
    python::def( "getSourceFile", &kdlib::getSourceFile, getSourceFile_( python::args( "offset"),
        "Return source file by the specified offset" ) );
    python::def( "getSourceLine", &getSourceLine, getSourceLine_( python::args( "offset"),
        "Return source file name, line and displacement by the specified offset" ) );
    python::def( "getOffset", &kdlib::getSymbolOffset,
        "Return traget virtual address for specified symbol" );
    python::def( "findSymbol", &TypeInfoAdapter::findSymbol, findSymbol_( python::args( "offset", "showDisplacement"),
        "Find symbol by the target virtual memory offset" ) );
    python::def("findSymbolAndDisp", &findSymbolAndDisp,
        "Return tuple(symbol_name, displacement) by virtual address" );
    python::def( "sizeof", &kdlib::getSymbolSize,
        "Return a size of the type or variable" );
    python::def("typedVarList", &TypedVarAdapter::getTypedVarListByTypeName,
        "Return a list of the typedVar class instances. Each item represents an item of the linked list in the target memory" );
    python::def("typedVarList", &TypedVarAdapter::getTypedVarListByType,
        "Return a list of the typedVar class instances. Each item represents an item of the linked list in the target memory" );
    python::def("typedVarArray", &TypedVarAdapter::getTypedVarArrayByTypeName,
        "Return a list of the typedVar class instances. Each item represents an item of the counted array in the target memory" );
    python::def("typedVarArray", &TypedVarAdapter::getTypedVarArrayByType,
        "Return a list of the typedVar class instances. Each item represents an item of the counted array in the target memory" );
    python::def("containingRecord", &TypedVarAdapter::containingRecordByName,
        "Return instance of the typedVar class. It's value are loaded from the target memory."
        "The start address is calculated by the same method as the standard macro CONTAINING_RECORD does" );
    python::def("containingRecord", &TypedVarAdapter::containingRecordByType,
        "Return instance of the typedVar class. It's value are loaded from the target memory."
        "The start address is calculated by the same method as the standard macro CONTAINING_RECORD does" );


   // // CPU registers
   // python::def( "reg", &getRegByName,
   //     "Return a CPU regsiter value by the register's name" );
   // python::def( "reg", &getRegByIndex,
   //     "Return a CPU regsiter value by the register's value" );
   // python::def ( "rdmsr", &loadMSR,
   //     "Return MSR value" );
   // python::def( "wrmsr", &setMSR,
   //     "Set MSR value" );
   // python::def( "getProcessorMode", &getProcessorMode, 
   //     "Return current processor mode as string: X86, ARM, IA64 or X64" );
   // python::def( "getProcessorType", &getProcessorType,
   //     "Return type of physical processor: X86, ARM, IA64 or X64" );
   // python::def( "setProcessorMode", &setProcessorMode,
   //     "Set current processor mode by string (X86, ARM, IA64 or X64)" );

   // // stack and local variables
   // python::def( "getStack", &getCurrentStack,
   //     "Return a current stack as a list of stackFrame objects" );
   // python::def( "getStackWow64", &getCurrentStackWow64,
   //     "Return a stack for wow64 context as a list of stackFrame objects" );
   // python::def( "getFrame", &getCurrentStackFrame,
   //     "Return a current stack frame" );
   // python::def( "getLocals", &getLocals, 
   //     "Get list of local variables" );
   // python::def( "getParams", &getParams, 
   //     "Get list of function arguments" );

    // breakpoints
    python::def( "setBp", &kdlib::softwareBreakPointSet,
        "Set software breakpoint on executiont" );
    python::def( "removeBp", &kdlib::breakPointRemove,
        "Remove breapoint by IDs" );

    //python::def( "setBp", &setHardwareBp, setHardwareBp_( python::args( "offset", "size", "accsessType", "callback" ) ,
    //    "Set hardware breakpoint" ) );
   // //python::def( "removeAllBp", &removeAllBp,
   // //    "Remove all breapoints" );

   // // processes and threads
   // python::def( "getCurrentProcess", &getCurrentProcess,
   //     "Return pointer to current process's block" );
   // python::def( "getImplicitThread", &getImplicitThread, 
   //     "Return implicit thread for current process" );
   // python::def( "setCurrentProcess", &setCurrentProcess, 
   //     "Set current process by address" );
   // python::def( "setImplicitThread", &setImplicitThread, 
   //     "Set implicit thread for current process" );
   // python::def( "getProcessThreads", &pysupport::getProcessThreads,
   //     "Get all process's threads ( user mode only )" );
   //python::def( "getCurrentProcessId", &kdlib::getCurrentProcessId,
   //     "Return PID of the current process ( user mode only )" );
   //python::def( "getCurrentThreadId", &kdlib::getCurrentThreadId,
   //     "Return TID of the current thread ( user mode only )" );
   // python::def( "getCurrentProcessExeName", &getCurrentProcessExecutableName,
   //     "Return name of executable file loaded in the current process");

   // // symbol path
   // python::def( "getSymbolPath", &getSymbolPath, "Returns current symbol path");
   // python::def( "setSymbolPath", &setSymbolPath, "Set current symbol path");
   // python::def( "appendSymbolPath", &appendSymbolPath, "Append current symbol path");

    python::class_<kdlib::NumBehavior, boost::noncopyable>( "numVariant", "numVariant", python::no_init )
        .def("__init__", python::make_constructor(&NumVariantAdaptor::getVariant) )
        .def( "__eq__", &NumVariantAdaptor::eq )
        .def( "__ne__", &NumVariantAdaptor::ne)
        .def( "__lt__", &NumVariantAdaptor::lt)
        .def( "__gt__", &NumVariantAdaptor::gt )
        .def( "__le__", &NumVariantAdaptor::le )
        .def( "__ge__", &NumVariantAdaptor::ge )
        .def( "__add__", &NumVariantAdaptor::add )
        .def( "__radd__", &NumVariantAdaptor::add )
        .def( "__sub__", &NumVariantAdaptor::sub )
        .def( "__rsub__", &NumVariantAdaptor::rsub )
        .def( "__mul__", &NumVariantAdaptor::mul )
        .def( "__rmul__", &NumVariantAdaptor::mul )
        .def( "__div__", &NumVariantAdaptor::div )
        .def( "__rdiv__", &NumVariantAdaptor::rdiv )
        .def( "__mod__", &NumVariantAdaptor::mod )
        .def( "__rmod__", &NumVariantAdaptor::rmod )
        .def( "__rshift__", &NumVariantAdaptor::rshift )
        .def( "__rrshift__", &NumVariantAdaptor::rrshift )
        .def( "__lshift__", &NumVariantAdaptor::lshift )
        .def( "__rlshift__", &NumVariantAdaptor::rlshift )
        .def( "__and__", &NumVariantAdaptor::and )
        .def( "__rand__", &NumVariantAdaptor::and )
        .def( "__or__", &NumVariantAdaptor::or )
        .def( "__ror__", &NumVariantAdaptor::or )
        .def( "__xor__", &NumVariantAdaptor::xor )
        .def( "__rxor__", &NumVariantAdaptor::xor )
        .def( "__neg__", &NumVariantAdaptor::neg )
        .def( "__pos__", &NumVariantAdaptor::pos ) 
        .def( "__invert__", &NumVariantAdaptor::invert ) 
        .def( "__nonzero__", &NumVariantAdaptor::nonzero )
        //.def( "__str__", &pykd::NumVariant::str )
       // .def( "__hex__", &pykd::NumVariant::hex )
        .def( "__long__", &NumVariantAdaptor::long_ )
        .def( "__int__", &NumVariantAdaptor::int_ )
        .def( "__index__", &NumVariantAdaptor::long_ )
        .def( "__hash__", &NumVariantAdaptor::long_ )
        ;

        NumVariantAdaptor::registerNumConvertion();

    python::class_<kdlib::Module, kdlib::ModulePtr, python::bases<kdlib::NumBehavior>, boost::noncopyable>("module", "Class representing executable module", python::no_init )
        .def("__init__", python::make_constructor(&ModuleAdapter::loadModuleByName ) )
        .def("__init__", python::make_constructor(&ModuleAdapter::loadModuleByOffset) )
        .def("begin", &kdlib::Module::getBase,
             "Return start address of the module" )
        .def("end", &kdlib::Module::getEnd,
             "Return end address of the module" )
        .def("size", &kdlib::Module::getSize,
              "Return size of the module" )
        .def("name", &kdlib::Module::getName,
             "Return name of the module" )
        .def("reload", &kdlib::Module::reloadSymbols,
            "(Re)load symbols for the module" )
        .def("image", &kdlib::Module::getImageName,
            "Return name of the image of the module" )
        .def("symfile", &kdlib::Module::getSymFile,
             "Return the full path to the module's symbol information" )
        .def("offset", &kdlib::Module::getSymbolVa,
            "Return offset of the symbol" )
        .def("findSymbol", ModuleAdapter::findSymbol, Module_findSymbol( python::args("offset", "showDisplacement"),
            "Return symbol name by virtual address" ) )
        .def("findSymbolAndDisp", ModuleAdapter::findSymbolAndDisp,
            "Return tuple(symbol_name, displacement) by virtual address" )
        .def("rva", &kdlib::Module::getSymbolRva,
            "Return rva of the symbol" )
        .def("sizeof", &kdlib::Module::getSymbolSize,
            "Return a size of the type or variable" )
        .def("type", &kdlib::Module::getTypeByName,
            "Return typeInfo class by type name" )
        //.def("getUdts", &Module::getUdts,
        //    "Return a list of all user-defined type names" )
        //.def("getEnums", &Module::getEnums,
        //    "Return a list of all enumeration names" )
        .def("typedVar", &kdlib::Module::getTypedVarByAddr,
            "Return a typedVar class instance" )
        .def("typedVar",&kdlib::Module::getTypedVarByName,
            "Return a typedVar class instance" )
        .def("typedVar",&kdlib::Module::getTypedVarByTypeName,
            "Return a typedVar class instance" )
        .def("typedVarList", &ModuleAdapter::getTypedVarListByTypeName,
            "Return a list of the typedVar class instances. Each item represents an item of the linked list in the target memory" )
        .def("typedVarArray", &ModuleAdapter::getTypedVarArrayByTypeName,
            "Return a list of the typedVar class instances. Each item represents an item of the counted array in the target memory" )
        .def("containingRecord", &kdlib::Module::containingRecord,
            "Return instance of the typedVar class. It's value are loaded from the target memory."
            "The start address is calculated by the same method as the standard macro CONTAINING_RECORD does" )
        .def("enumSymbols", ModuleAdapter::enumSymbols, Module_enumSymbols( python::args("mask"),
             "Return list of tuple ( symbolname, offset )" ) )
        //.def("checksum", &Module::getCheckSum,
        //    "Return a image file checksum: IMAGE_OPTIONAL_HEADER.CheckSum" )
        //.def("timestamp", &Module::getTimeDataStamp,
        //    "Return a low 32 bits of the time stamp of the image: IMAGE_FILE_HEADER.TimeDateStamp" )
        //.def("unloaded", &Module::isUnloaded,
        //    "Returns a flag that the module was unloaded")
        //.def("um", &Module::isUserMode,
        //    "Returns a flag that the module is a user-mode module")
        //.def("queryVersion", &Module::queryVersion,
        //    "Return string from the module's version resources" )
        //.def("getVersion",  &Module::getVersion,
        //    "Return tuple of the module's file version" )
        .def("__getattr__", &kdlib::Module::getSymbolVa,
            "Return address of the symbol" )
        .def( "__str__", &ModuleAdapter::print );


    python::class_<kdlib::TypeInfo, kdlib::TypeInfoPtr, python::bases<kdlib::NumBehavior>, boost::noncopyable >("typeInfo", "Class representing typeInfo", python::no_init )
        .def("__init__", python::make_constructor( TypeInfoAdapter::getTypeInfoByName ) )
        .def( "name", &kdlib::TypeInfo::getName,
            "Return type name" )
        .def( "size", &kdlib::TypeInfo::getSize,
            "Return type size" )
        .def( "staticOffset", TypeInfoAdapter::getStaticOffset,
            "Return offset of the static field" )
        .def( "fieldOffset", TypeInfoAdapter::getElementOffset,
            "Return offset of the nonstatic field" )
        .def( "bitOffset", &kdlib::TypeInfo::getBitOffset,
            "Return bit field's offset" )
        .def( "bitWidth", &kdlib::TypeInfo::getBitWidth,
            "Return bit field's length" )
        .def( "field", TypeInfoAdapter::getElementByName,
            "Return field's type" )
        .def( "fieldName", &kdlib::TypeInfo::getElementName,
         "Return name of struct field by index" )
        //.def( "asMap", &kdlib::TypeInfo::asMap,
        //    "Return type as python dict ( for enum types )" )
        .def( "deref", &kdlib::TypeInfo::deref,
            "Return type of pointer" )
        //.def( "append", &kdlib::TypeInfo::appendField,
        //    "Add a new field to custom defined struct" )
        .def( "ptrTo", &kdlib::TypeInfo::ptrTo,
            "Return pointer to the type" )
        .def( "arrayOf", &kdlib::TypeInfo::arrayOf,
            "Return array of the type" )
        .def( "__str__", &TypeInfoAdapter::print,
            "Return typa as a printable string" )
        .def( "__getattr__", TypeInfoAdapter::getElementByName )
        .def("__len__", &kdlib::TypeInfo::getElementCount )
        .def("__getitem__", TypeInfoAdapter::getElementByIndex )
        ;

    python::class_<kdlib::TypedVar, kdlib::TypedVarPtr, python::bases<kdlib::NumBehavior>, boost::noncopyable >("typedVar", 
        "Class of non-primitive type object, child class of typeClass. Data from target is copied into object instance", python::no_init  )
        .def("__init__", python::make_constructor(TypedVarAdapter::getTypedVarByName) )
        .def("__init__", python::make_constructor(TypedVarAdapter::getTypedVarByTypeName) )
        .def("__init__", python::make_constructor(TypedVarAdapter::getTypedVarByTypeInfo) )
        .def("getAddress", &kdlib::TypedVar::getAddress, 
            "Return virtual address" )
        .def("sizeof", &kdlib::TypedVar::getSize,
            "Return size of a variable in the target memory" )
        .def("fieldOffset", TypedVarAdapter::getFieldOffsetByName,
            "Return target field offset" )
        .def("field", TypedVarAdapter::getField,
            "Return field of structure as an object attribute" )
        .def("field", TypedVarAdapter::getElementByIndex,
            "Return field of structure as an object attribute" )
        .add_property( "fields", TypedVarAdapter::getFields,
            "Return list of tuple ( filedName, fieldOffset, fieldValue )" )
        .def( "fieldName", &kdlib::TypedVar::getElementName,
            "Return name of struct field by index" )
        //.def( "dataKind", &kdlib::TypedVar::getDataKind,
        //    "Retrieves the variable classification of a data: DataIsXxx")
         .def("deref", &kdlib::TypedVar::deref,
            "Return value by pointer" )
        .def("type", &kdlib::TypedVar::getType,
            "Return typeInfo instance" )
        .def("__getattr__", TypedVarAdapter::getField,
            "Return field of structure as an object attribute" )
        .def( "__str__", &TypedVarAdapter::print )
        .def("__len__", &kdlib::TypedVar::getElementCount )
        .def("__getitem__", &TypedVarAdapter::getElementByIndex )
        //.def("__getitem__", &kdlib::TypedVar::getElementByIndexPtr )
        ;

   // python::class_<TypeBuilder>("typeBuilder",
   //     "Class for building dynamically defined types", boost::python::no_init  )
   //     .def( python::init<ULONG>() )
   //     .def( python::init<>() )
   //     .add_property( "UInt1B", &TypeBuilder::getUInt1B )
   //     .add_property( "UInt2B", &TypeBuilder::getUInt2B )
   //     .add_property( "UInt4B", &TypeBuilder::getUInt4B )
   //     .add_property( "UInt8B", &TypeBuilder::getUInt8B )
   //     .add_property( "Int1B", &TypeBuilder::getInt1B )
   //     .add_property( "Int2B", &TypeBuilder::getInt2B )
   //     .add_property( "Int4B", &TypeBuilder::getInt4B )
   //     .add_property( "Int8B", &TypeBuilder::getInt8B )
   //     .add_property( "Long", &TypeBuilder::getLong )
   //     .add_property( "ULong", &TypeBuilder::getULong )
   //     .add_property( "Bool", &TypeBuilder::getBool )
   //     .add_property( "Char", &TypeBuilder::getChar )
   //     .add_property( "WChar", &TypeBuilder::getWChar )
   //     .add_property( "VoidPtr", &TypeBuilder::getVoidPtr )
   //     .def( "createStruct", &TypeBuilder::createStruct, TypeBuilder_createStruct( python::args( "name", "align" ),
   //         "Create custom struct" ) )
   //     .def( "createUnion", &TypeBuilder::createUnion, 
   //         "Create custom union" );

    python::class_<Breakpoint, BreakpointPtr, boost::noncopyable>( "breakpoint",
        "class for breakpoint representation", python::no_init  )
        .def("__init__", python::make_constructor(Breakpoint::setSoftwareBreakpoint) )
        ;


    python::class_<kdlib::CPUContext, kdlib::CPUContextPtr, boost::noncopyable>( "cpu",
        "class for CPU context representation", python::no_init  )
         .def("__init__", python::make_constructor(&kdlib::loadCPUCurrentContext) )
         .def("__init__", python::make_constructor(&kdlib::loadCPUContextByIndex) )
         .add_property("ip", &kdlib::CPUContext::getIP )
         .add_property("sp", &kdlib::CPUContext::getSP )
         .add_property("fp", &kdlib::CPUContext::getSP )
         .def("__getattr__",  &CPUContextAdaptor::getRegisterByName )
         .def("__getitem__",  &CPUContextAdaptor::getRegisterByIndex );


   // python::class_<CpuReg, python::bases<intBase> >( 
   //     "cpuReg", "CPU regsiter class", boost::python::no_init )
   //         .def( "name", &CpuReg::name, "The name of the regsiter" )
   //         .def( "index", &CpuReg::index, "The index of thr register" );

   // python::class_<ScopeVars,ScopeVarsPtr,boost::noncopyable>( "locals",
   //     "Class for access to local vars",  python::no_init  )
   //         .def("__len__", &ScopeVars::getVarCount )
   //         .def("__getitem__", &ScopeVars::getVarByIndex )
   //         .def("__getitem__", &ScopeVars::getVarByName );

   // python::class_<StackFrame, StackFramePtr,boost::noncopyable>( "stackFrame", 
   //      "Class representing a frame of the call stack", python::no_init )
   //     .def_readonly( "instructionOffset", &StackFrame::m_instructionOffset,
   //         "Return a frame's instruction offset" )
   //     .def_readonly( "returnOffset", &StackFrame::m_returnOffset,
   //         "Return a frame's return offset" )
   //     .def_readonly( "frameOffset", &StackFrame::m_frameOffset,
   //         "Return a frame's offset" )
   //     .def_readonly( "stackOffset", &StackFrame::m_stackOffset,
   //         "Return a frame's stack offset" )
   //     .def_readonly( "frameNumber", &StackFrame::m_frameNumber,
   //         "Return a frame's number" )
   //     .add_property( "locals", &StackFrame::getLocals, 
   //         "Get list of local variables for this stack frame" )
   //     .add_property( "params", &StackFrame::getParams,
   //         "Get list of function params" )
   //     .def( "__str__", &StackFrame::print,
   //         "Return stacks frame as a string");

   // python::class_< SystemVersion, SystemVersionPtr, boost::noncopyable >(
   //     "systemVersion", "Operation system version", python::no_init)
   //     .def_readonly( "platformId", &SystemVersion::platformId,
   //         "Platform ID: VER_PLATFORM_WIN32_NT for NT-based Windows")
   //     .def_readonly( "win32Major", &SystemVersion::win32Major,
   //         "Major version number of the target's operating system")
   //     .def_readonly( "win32Minor", &SystemVersion::win32Minor,
   //         "Minor version number of the target's operating system")
   //     .def_readonly( "buildNumber", &SystemVersion::buildNumber,
   //         "Build number for the target's operating system")
   //     .def_readonly( "buildString", &SystemVersion::buildString,
   //         "String that identifies the build of the system")
   //     .def_readonly( "servicePackString", &SystemVersion::servicePackString,
   //         "String for the service pack level of the target computer")
   //     .def_readonly( "isCheckedBuild", &SystemVersion::isCheckedBuild,
   //         "Checked build flag")
   //     .def("__str__", pysupport::printSystemVersion,
   //         "Return object as a string");


   // python::class_< ExceptionInfo, ExceptionInfoPtr, boost::noncopyable >(
   //     "exceptionInfo", "Exception information", python::no_init )
   //     .def_readonly( "FirstChance", &ExceptionInfo::FirstChance,
   //         "Specifies whether this exception has been previously encountered")
   //     .def_readonly( "ExceptionCode", &ExceptionInfo::ExceptionCode,
   //         "The reason the exception occurred")
   //     .def_readonly( "ExceptionFlags", &ExceptionInfo::ExceptionFlags,
   //         "The exception flags")
   //     .def_readonly( "ExceptionRecord", &ExceptionInfo::ExceptionRecord,
   //         "A pointer to an associated EXCEPTION_RECORD structure")
   //     .def_readonly( "ExceptionAddress", &ExceptionInfo::ExceptionAddress,
   //         "The address where the exception occurred")
   //     .add_property( "Parameters", &ExceptionInfo::getParameters,
   //         "An array of additional arguments that describe the exception")
   //     .def( "__str__", &ExceptionInfo::print,
   //         "Return object as a string");


   // python::enum_<EVENT_TYPE>("eventType", "Type of debug event")
   //     .value("Breakpoint", EventTypeBreakpoint)
   //     .value("Exception", EventTypeException)
   //     .value("CreateThread", EventTypeCreateThread)
   //     .value("ExitThread", EventTypeExitThread)
   //     .value("CreateProcess", EventTypeCreateProcess)
   //     .value("ExitProcess", EventTypeExitProcess)
   //     .value("LoadModule", EventTypeLoadModule)
   //     .value("UnloadModule", EventTypeUnloadModule)
   //     .value("SystemError", EventTypeSystemError)
   //     .value("SessionStatus", EventTypeSessionStatus)
   //     .value("ChangeDebuggeeState", EventTypeChangeDebuggeeState)
   //     .value("ChangeEngineState", EventTypeChangeEngineState)
   //     .value("ChangeSymbolState", EventTypeChangeSymbolState)
   //     .export_values();

   // python::def( "lastEvent", &getLastEventType,
   //     "Return type of last event: eventType" );
   // python::def( "lastException", &getLastExceptionInfo,
   //     "Return data of last exception event: exceptionInfo" );
   // python::def( "bugCheckData", &pysupport::getBugCheckData,
   //     "Function reads the kernel bug check code and related parameters\n"
   //     "And return tuple: (code, arg1, arg2, arg3, arg4)" );

    python::class_<kdlib::Disasm>("disasm", "Class disassemble a processor instructions" )
        .def( python::init<>( "constructor" ) )
        .def( python::init<ULONG64>( boost::python::args("offset"), "constructor" ) )
        .def( "disasm", &kdlib::Disasm::disassemble, "Disassemble next instruction" )
        .def( "disasm", &kdlib::Disasm::jump, "Disassemble from the specified offset" )
        .def( "asm", &kdlib::Disasm::assembly, "Insert assemblied instuction to current offset" )
        .def( "begin", &kdlib::Disasm::begin, "Return begin offset" )
        .def( "current", &kdlib::Disasm::current, "Return current offset" )
        .def( "length", &kdlib::Disasm::length, "Return current instruction length" )
        .def( "instruction", &kdlib::Disasm::instruction, "Returm current disassembled instruction" )
        .def( "ea", &kdlib::Disasm::ea, "Return effective address for last disassembled instruction or 0" )
        .def( "reset", &kdlib::Disasm::reset, "Reset current offset to begin" )
        .def( "findOffset", &kdlib::Disasm::getNearInstruction, "Return the location of a processor instruction relative to a given location" )
        .def( "jump", &kdlib::Disasm::jump, "Change the current instruction" )
        .def( "jumprel", &kdlib::Disasm::jumprel, "Change the current instruction" );


    python::enum_<kdlib::DebugCallbackResult>("eventResult", "Return value of event handler")
        .value("Proceed", kdlib::DebugCallbackProceed)
        .value("NoChange", kdlib::DebugCallbackNoChange)
        .value("Break", kdlib::DebugCallbackBreak)
        .export_values();

    python::enum_<kdlib::ExecutionStatus>("executionStatus", "Execution Status")
        .value("NoChange", kdlib::DebugStatusNoChange )
        .value("Go", kdlib::DebugStatusGo )
        .value("Break", kdlib::DebugStatusBreak )
        .value("NoDebuggee", kdlib::DebugStatusNoDebuggee )
        .export_values();

    python::class_<EventHandler, EventHandlerPtr, boost::noncopyable>(
        "eventHandler", "Base class for overriding and handling debug notifications" )
         .def( "onBreakpoint", &EventHandler::onBreakpoint,
            "Triggered breakpoint event. Parameter is int: ID of breakpoint\n"
            "For ignore event method must return eventResult.noChange" )
   //     .def( "onModuleLoad", &EventHandlerWrap::OnModuleLoad,
   //         "Triggered module load event. Parameter are long: module base, string: module name\n"
   //         "For ignore event method must return eventResult.noChange" )
   //     .def( "onModuleUnload", &EventHandlerWrap::OnModuleUnload,
   //         "Triggered module unload event. Parameter are  long: module base, string: module name\n"
   //         "For ignore event method must return eventResult.noChange" )
   //     .def( "onException", &EventHandlerWrap::OnException,
   //         "Triggered exception event. Parameter - exceptionInfo\n"
   //         "For ignore event method must return eventResult.noChange" )
        .def( "onExecutionStatusChange", &EventHandler::onExecutionStatusChange,
            "Triggered execution status changed. Parameter - execution status.\n"
            "There is no return value" )
   //     .def( "onSymbolsLoaded", &EventHandlerWrap::onSymbolsLoaded,
   //         "Triggered debug symbols loaded. Parameter - module base or 0\n"
   //         "There is no return value")
   //     .def( "onSymbolsUnloaded", &EventHandlerWrap::onSymbolsUnloaded,
   //         "Triggered debug symbols unloaded. Parameter - module base or 0 (all modules)\n"
   //         "There is no return value");
      ;

    python::register_exception_translator<kdlib::IndexException>( &ExceptionTranslator::indexTranslate );

    // kdlib exception
    pykd::exception<kdlib::DbgException>( "DbgException", "Pykd base exception class" );
    pykd::exception<kdlib::MemoryException,kdlib::DbgException>( "MemoryException", "Target memory access exception class" );
    pykd::exception<kdlib::SymbolException,kdlib::DbgException>( "SymbolException", "Symbol exception" );
    pykd::exception<kdlib::TypeException,kdlib::SymbolException>( "TypeException", "type exception" );
}

//////////////////////////////////////////////////////////////////////////////////