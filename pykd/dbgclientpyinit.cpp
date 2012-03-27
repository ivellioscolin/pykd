// 
// Debug client class (and global function) initialization
// 

#include "stdafx.h"

#include "dbgclient.h"
#include "dbgmem.h"
#include "process.h"
#include "bpoint.h"

namespace pykd {

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
BOOST_PYTHON_FUNCTION_OVERLOADS( getLocals_, getLocals, 0, 1 );
BOOST_PYTHON_FUNCTION_OVERLOADS( setSoftwareBp_, setSoftwareBp, 1, 2 );
BOOST_PYTHON_FUNCTION_OVERLOADS( setHardwareBp_, setHardwareBp, 3, 4 );

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( DebugClient_loadChars, DebugClient::loadChars, 2, 3 );
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( DebugClient_loadWChars, DebugClient::loadWChars, 2, 3 );
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( DebugClient_loadBytes, DebugClient::loadBytes, 2, 3 );
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( DebugClient_loadWords, DebugClient::loadWords, 2, 3 );
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( DebugClient_loadDWords, DebugClient::loadDWords, 2, 3 );
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( DebugClient_loadQWords, DebugClient::loadQWords, 2, 3 );
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( DebugClient_loadSignBytes, DebugClient::loadSignBytes, 2, 3 );
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( DebugClient_loadSignWords, DebugClient::loadSignWords, 2, 3 );
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( DebugClient_loadSignDWords, DebugClient::loadSignDWords, 2, 3 );
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( DebugClient_loadSignQWords, DebugClient::loadSignQWords, 2, 3 );
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( DebugClient_compareMemory, DebugClient::compareMemory, 3, 4 );
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( DebugClient_getLocals, DebugClient::getLocals, 0, 1 );
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( DebugClient_setSoftwareBp, DebugClient::setSoftwareBp, 1, 2 );
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( DebugClient_setHardwareBp, DebugClient::setHardwareBp, 3, 4 );

////////////////////////////////////////////////////////////////////////////////

void initDbgClientPyExports()
{
    python::class_<DebugClient, DebugClientPtr>("dbgClient", "Class representing a debugging session", python::no_init  )
        .def( "addr64", &DebugClient::addr64,
            "Extend address to 64 bits formats" )
        .def( "breakin", &DebugClient::breakin,
            "Break into debugger" )
        .def( "compareMemory", &DebugClient::compareMemory, DebugClient_compareMemory( python::args( "offset1", "offset2", "length", "phyAddr" ),
            "Compare two memory buffers by virtual or physical addresses" ) )
        .def( "loadDump", &DebugClient::loadDump,
            "Load crash dump" )
        .def( "startProcess", &DebugClient::startProcess, 
            "Start process for debugging" )
        .def( "attachProcess", &DebugClient::attachProcess,
            "Attach debugger to a exsisting process" )
        .def( "attachKernel", &DebugClient::attachKernel, 
            "Attach debugger to a target's kernel" )
        .def( "detachProcess", &DebugClient::detachProcess,
            "Detach debugger from the current process" )
        .def( "expr", &DebugClient::evaluate,
            "Evaluate windbg expression" )
        .def( "findSymbol", &DebugClient::findSymbol,
            "Find symbol by the target virtual memory offset" )
        .def( "getCurrentProcess", &DebugClient::getCurrentProcess,
            "Return pointer to current process's block" )
        .def( "getCurrentStack", &DebugClient::getCurrentStack,
            "Return a current stack as a list of stackFrame objects" )
        .def( "getDebuggeeType", &DebugClient::getDebuggeeType,
            "Return type of the debuggee" )
        .def( "getImplicitThread", &DebugClient::getImplicitThread, 
            "Return implicit thread for current process" )
        .def( "getExecutionStatus", &DebugClient::getExecutionStatus,
            "Return information about the execution status of the debugger" )
        .def( "getOffset", &DebugClient::getOffset,
            "Return traget virtual address for specified symbol" )
        .def( "getPdbFile", &DebugClient::getPdbFile, 
            "Return full path to PDB (Program DataBase, debug information) file" )
        .def( "getProcessorMode", &DebugClient::getProcessorMode, 
            "Return current processor mode as string: X86, ARM, IA64 or X64" )
        .def( "getProcessorType", &DebugClient::getProcessorType,
            "Return type of physical processor: X86, ARM, IA64 or X64" )
        .def( "getThreadList", &DebugClient::getThreadList, 
            "Return list of threads (each item is numeric identifier of thread)" )
        .def( "go", &DebugClient::changeDebuggerStatus<DEBUG_STATUS_GO>,
            "Change debugger status to DEBUG_STATUS_GO"  )
        .def( "is64bitSystem", &DebugClient::is64bitSystem,
            "Check if target system has 64 address space" )
        .def( "isDumpAnalyzing", &DebugClient::isDumpAnalyzing,
            "Check if it is a dump analyzing ( not living debuggee )" )
        .def( "isKernelDebugging", &DebugClient::isKernelDebugging,
            "Check if kernel dubugging is running" )
        .def( "isValid", &DebugClient::isVaValid,
            "Check if the virtual address is valid" )
        .def( "killProcess", &DebugClient::terminateProcess,
            "Stop debugging and terminate current process" )
        .def( "loadBytes", &DebugClient::loadBytes, DebugClient_loadBytes( python::args( "offset", "count", "phyAddr" ),
            "Read the block of the target's memory and return it as list of unsigned bytes" ) )
        .def( "loadWords", &DebugClient::loadWords, DebugClient_loadWords( python::args( "offset", "count", "phyAddr" ),
            "Read the block of the target's memory and return it as list of unsigned shorts" ) )
        .def( "loadDWords", &DebugClient::loadDWords, DebugClient_loadDWords( python::args( "offset", "count", "phyAddr" ),
            "Read the block of the target's memory and return it as list of unsigned long ( double word )" ) )
        .def( "loadQWords", &DebugClient::loadQWords, DebugClient_loadQWords( python::args( "offset", "count", "phyAddr" ),
            "Read the block of the target's memory and return it as list of unsigned long long ( quad word )" ) )
        .def( "loadSignBytes", &DebugClient::loadSignBytes, DebugClient_loadSignBytes( python::args( "offset", "count", "phyAddr" ),
            "Read the block of the target's memory and return it as list of signed bytes" ) )
        .def( "loadSignWords", &DebugClient::loadSignWords, DebugClient_loadSignWords( python::args( "offset", "count", "phyAddr" ),
            "Read the block of the target's memory and return it as list of signed shorts" ) )
        .def( "loadSignDWords", &DebugClient::loadSignDWords, DebugClient_loadSignDWords( python::args( "offset", "count", "phyAddr" ),
            "Read the block of the target's memory and return it as list of signed longs" ) )
        .def( "loadSignQWords", &DebugClient::loadSignQWords, DebugClient_loadSignQWords( python::args( "offset", "count", "phyAddr" ),
            "Read the block of the target's memory and return it as list of signed long longs" ) )
        .def( "loadChars", &DebugClient::loadChars, DebugClient_loadChars( python::args( "offset", "count", "phyAddr" ),
            "Load string from target memory" ) )
        .def( "loadWChars", &DebugClient::loadWChars, DebugClient_loadWChars( python::args( "offset", "count", "phyAddr" ),
            "Load string from target memory" ) )
        .def( "loadCStr", &DebugClient::loadCStr,
            "Load string from the target buffer containing 0-terminated ansi-string" )
        .def( "loadWStr", &DebugClient::loadWStr,
            "Load string from the target buffer containing 0-terminated unicode-string" )
        .def( "loadUnicodeString", &DebugClient::loadUnicodeStr,
            "Return string represention of windows UNICODE_STRING type" )
        .def( "loadAnsiString", &DebugClient::loadAnsiStr,
            "Return string represention of windows ANSI_STRING type" )
        .def( "loadPtrList", &DebugClient::loadPtrList,
            "Return list of pointers, each points to next" )
        .def( "loadPtrArray", &DebugClient::loadPtrArray,
            "Read the block of the target's memory and return it as a list of pointers" )
        .def( "ptrByte", &DebugClient::ptrByte,
            "Read an unsigned 1-byte integer from the target memory" )
        .def( "ptrWord", &DebugClient::ptrWord,
            "Read an unsigned 2-byte integer from the target memory" )
        .def( "ptrDWord", &DebugClient::ptrDWord,
            "Read an unsigned 4-byte integer from the target memory" )
        .def( "ptrQWord", &DebugClient::ptrQWord,
            "Read an unsigned 8-byte integer from the target memory" )
        .def( "ptrMWord", &DebugClient::ptrMWord,
            "Read an unsigned mashine's word wide integer from the target memory" )
        .def( "ptrSignByte", &DebugClient::ptrSignByte,
            "Read an signed 1-byte integer from the target memory" )
        .def( "ptrSignWord", &DebugClient::ptrSignWord,
            "Read an signed 2-byte integer from the target memory" )
        .def( "ptrSignDWord", &DebugClient::ptrSignDWord,
            "Read an signed 4-byte integer from the target memory" )
        .def( "ptrSignQWord", &DebugClient::ptrSignQWord,
            "Read an signed 8-byte integer from the target memory" )
        .def( "ptrSignMWord", &DebugClient::ptrSignMWord,
            "Read an signed mashine's word wide integer from the target memory" )
        .def( "ptrPtr", &DebugClient::ptrPtr,
            "Read an pointer value from the target memory" )
        .def("typedVar",&DebugClient::getTypedVarByName,
            "Return a typedVar class instance" )
        .def("typedVar",&DebugClient::getTypedVarByTypeInfo,
            "Return a typedVar class instance" )
        .def("typedVar",&DebugClient::getTypedVarByTypeName,
            "Return a typedVar class instance" )
        .def( "loadExt", &pykd::DebugClient::loadExtension,
            "Load a debuger extension" )
        .def( "loadModule", &pykd::DebugClient::loadModuleByName, 
            "Return instance of Module class" )
        .def( "loadModule", &pykd::DebugClient::loadModuleByOffset, 
            "Return instance of the Module class which posseses specified address" )
        .def( "dbgCommand", &pykd::DebugClient::dbgCommand,
             "Run a debugger's command and return it's result as a string" )
        .def( "dprint", &pykd::DebugClient::dprint,
            "Print out string. If dml = True string is printed with dml highlighting ( only for windbg )" )
        .def( "dprintln", &pykd::DebugClient::dprintln,
            "Print out string and insert end of line symbol. If dml = True string is printed with dml highlighting ( only for windbg )" )
        .def( "ptrSize", &DebugClient::ptrSize,
            "Return effective pointer size" )
         .def ( "rdmsr", &DebugClient::loadMSR,
            "Return MSR value" )
        .def( "reg", &DebugClient::getRegByName,
            "Return a CPU regsiter value by the register's name" )
        .def( "reg", &DebugClient::getRegByIndex,
            "Return a CPU regsiter value by the register's value" )
        .def( "setCurrentProcess", &DebugClient::setCurrentProcess, 
            "Set current process by address" )
        .def( "setExecutionStatus", &DebugClient::setExecutionStatus,
            "Requests that the debugger engine enter an executable state" )
        .def( "setImplicitThread", &DebugClient::setImplicitThread, 
            "Set implicit thread for current process" )
        .def( "setProcessorMode", &DebugClient::setProcessorMode, 
            "Set current processor mode by string (X86, ARM, IA64 or X64)" )
        .def( "step", &DebugClient::changeDebuggerStatus<DEBUG_STATUS_STEP_OVER>, 
            "Change debugger status to DEBUG_STATUS_STEP_OVER" )
        .def( "symbolsPath", &DebugClient::dbgSymPath, 
            "Return symbol path" )
        .def( "trace", &DebugClient::changeDebuggerStatus<DEBUG_STATUS_STEP_INTO>, 
            "Change debugger status to DEBUG_STATUS_STEP_INTO" )
        .def("typedVarList", &DebugClient::getTypedVarListByTypeName,
            "Return a list of the typedVar class instances. Each item represents an item of the linked list in the target memory" )
        .def("typedVarList", &DebugClient::getTypedVarListByType,
            "Return a list of the typedVar class instances. Each item represents an item of the linked list in the target memory" )
        .def("typedVarArray", &DebugClient::getTypedVarArrayByTypeName,
            "Return a list of the typedVar class instances. Each item represents an item of the counted array in the target memory" )
        .def("typedVarArray", &DebugClient::getTypedVarArrayByType,
            "Return a list of the typedVar class instances. Each item represents an item of the counted array in the target memory" )
        .def("containingRecord", &DebugClient::containingRecordByName,
            "Return instance of the typedVar class. It's value are loaded from the target memory."
            "The start address is calculated by the same method as the standard macro CONTAINING_RECORD does" )
        .def("containingRecord", &DebugClient::containingRecordByType,
            "Return instance of the typedVar class. It's value are loaded from the target memory."
            "The start address is calculated by the same method as the standard macro CONTAINING_RECORD does" )
        .def( "waitForEvent", &DebugClient::waitForEvent,
            "Wait for events that breaks into the debugger" )
        .def( "wrmsr", &DebugClient::setMSR,
            "Set MSR value" )
        .def( "getNumberProcessors", &DebugClient::getNumberProcessors,
            "Get the number of actual processors in the machine" )
        .def( "getPageSize", &DebugClient::getPageSize,
            "Get the page size for the currently executing processor context" )
        .def( "getContext", &DebugClient::getThreadContext,
            "Get context of current thread (register values)" )
        .def( "getLocals", &DebugClient::getLocals, DebugClient_getLocals( python::args( "ctx" ),
            "Get list of local variables" ) )
        .def( "setBp", &DebugClient::setSoftwareBp, DebugClient_setSoftwareBp( python::args( "offset", "callback" ),
            "Set software breakpoint on executiont" ) )
        .def( "setBp", &DebugClient::setHardwareBp, DebugClient_setHardwareBp( python::args( "offset", "size", "accsessType", "callback" ),
            "Set hardware breakpoint" ) )
        .def( "getAllBp", &DebugClient::getAllBp,
            "Get all breapoint IDs" )
        .def( "removeBp", &DebugClient::removeBp,
            "Remove breapoint by IDs" )
        .def( "removeBp", &DebugClient::removeAllBp,
            "Remove all breapoints" )
        .def( "addSynSymbol", &DebugClient::addSyntheticSymbol,
            "Add new synthetic symbol for virtual address" )
        .def( "delAllSynSymbols", &DebugClient::delAllSyntheticSymbols, 
            "Delete all synthetic symbol for all modules")
        .def( "delSynSymbol", &DebugClient::delSyntheticSymbol, 
            "Delete synthetic symbols by virtual address" )
        .def( "delSynSymbolsMask", &DebugClient::delSyntheticSymbolsMask, 
            "Delete synthetic symbols by mask of module and symbol name");

    python::def( "addr64", &addr64,
        "Extend address to 64 bits formats" );
    python::def( "breakin", &breakin,
        "Break into debugger" );
    python::def( "compareMemory", &compareMemory, compareMemory_( python::args( "offset1", "offset2", "length", "phyAddr" ),
        "Compare two memory buffers by virtual or physical addresses" ) );
    python::def( "createDbgClient", (DebugClientPtr(*)())&DebugClient::createDbgClient, 
        "create a new instance of the dbgClient class" );
    python::def( "loadDump", &loadDump,
        "Load crash dump (only for console)");
    python::def( "startProcess", &startProcess,
        "Start process for debugging (only for console)"); 
    python::def( "attachProcess", &attachProcess,
        "Attach debugger to a exsisting process" );
    python::def( "attachKernel", &attachKernel,
        "Attach debugger to a kernel target" );
    python::def( "detachProcess", &detachProcess,
        "Detach denugger from the current process" );
    python::def( "expr", &evaluate,
        "Evaluate windbg expression" );
    python::def( "findSymbol", &findSymbol,
        "Find symbol by the target virtual memory offset" );
    python::def( "getCurrentProcess", &getCurrentProcess,
        "Return pointer to current process's block" );
    python::def( "getCurrentStack", &getCurrentStack,
        "Return a current stack as a list of stackFrame objects" );
    python::def( "getDebuggeeType", &getDebuggeeType,
        "Return type of the debuggee" );
    python::def( "getImplicitThread", &getImplicitThread, 
        "Return implicit thread for current process" );
    python::def( "getExecutionStatus", &getExecutionStatus,
        "Return information about the execution status of the debugger" );
    python::def( "getOffset", &getOffset,
        "Return traget virtual address for specified symbol" );
    python::def( "getPdbFile", &getPdbFile, 
        "Return full path to PDB (Program DataBase, debug information) file" );
    python::def( "go", &changeDebuggerStatus<DEBUG_STATUS_GO>,
        "Change debugger status to DEBUG_STATUS_GO"  );
    python::def( "getProcessorMode", &getProcessorMode, 
        "Return current processor mode as string: X86, ARM, IA64 or X64" );
    python::def( "getProcessorType", &getProcessorType,
        "Return type of physical processor: X86, ARM, IA64 or X64" );
    python::def( "getThreadList", &getThreadList, 
        "Return list of threads (each item is numeric identifier of thread)" );
    python::def( "is64bitSystem", &is64bitSystem,
        "Check if target system has 64 address space" );
    python::def( "isDumpAnalyzing", &isDumpAnalyzing,
        "Check if it is a dump analyzing ( not living debuggee )" );
    python::def( "isKernelDebugging", &isKernelDebugging,
        "Check if kernel dubugging is running" );
    python::def( "isWindbgExt", &WindbgGlobalSession::isInit,
        "Check if script works in windbg context" );
    python::def( "isValid", &isVaValid,
        "Check if the virtual address is valid" );
    python::def( "killProcess", &terminateProcess,
            "Stop debugging and terminate current process" );
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
        "Return string represention of windows ANSU_STRING type" );
    python::def( "loadPtrList", &loadPtrList,
        "Return list of pointers, each points to next" );
    python::def( "loadPtrArray", &loadPtrArray,
        "Read the block of the target's memory and return it as a list of pointers" );
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
    boost::python::def( "addSynSymbol", &addSyntheticSymbol,
        "Add new synthetic symbol for virtual address" );
    boost::python::def( "delAllSynSymbols", &delAllSyntheticSymbols, 
        "Delete all synthetic symbol for all modules");
    boost::python::def( "delSynSymbol", &delSyntheticSymbol, 
        "Delete synthetic symbols by virtual address" );
    boost::python::def( "delSynSymbolsMask", &delSyntheticSymbolsMask, 
        "Delete synthetic symbols by mask of module and symbol name");
    python::def( "loadExt", &pykd::loadExtension,
        "Load a debuger extension" );
    python::def( "loadModule", &loadModuleByName,
        "Return instance of Module class"  );
    python::def( "loadModule", &loadModuleByOffset,
        "Return instance of the Module class which posseses specified address" );
    python::def( "dbgCommand", &pykd::dbgCommand,
        "Run a debugger's command and return it's result as a string" ),
    python::def( "dprint", &pykd::dprint, dprint_( boost::python::args( "str", "dml" ), 
        "Print out string. If dml = True string is printed with dml highlighting ( only for windbg )" ) );
    python::def( "dprintln", &pykd::dprintln, dprintln_( boost::python::args( "str", "dml" ), 
        "Print out string and insert end of line symbol. If dml = True string is printed with dml highlighting ( only for windbg )" ) );
    python::def( "ptrSize", &ptrSize,
        "Return effective pointer size" );
    python::def ( "rdmsr", &DebugClient::loadMSR,
        "Return MSR value" );
    python::def( "reg", &getRegByName,
        "Return a CPU regsiter value by the register's name" );
    python::def( "reg", &getRegByIndex,
        "Return a CPU regsiter value by the register's value" );
    python::def( "setExecutionStatus",  &setExecutionStatus,
        "Requests that the debugger engine enter an executable state" );
    python::def( "setCurrentProcess", &setCurrentProcess, 
        "Set current process by address" );
    python::def( "setImplicitThread", &setImplicitThread, 
        "Set implicit thread for current process" );
    python::def( "setProcessorMode", &setProcessorMode, 
        "Set current processor mode by string (X86, ARM, IA64 or X64)" );
    python::def( "step", &changeDebuggerStatus<DEBUG_STATUS_STEP_OVER>, 
        "Change debugger status to DEBUG_STATUS_STEP_OVER" );
    python::def( "symbolsPath", &dbgSymPath, 
        "Return symbol path" );
    python::def( "wrmsr", &setMSR,
        "Set MSR value" );
    python::def( "trace", &changeDebuggerStatus<DEBUG_STATUS_STEP_INTO>, 
        "Change debugger status to DEBUG_STATUS_STEP_INTO" );
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
    python::def( "waitForEvent", &waitForEvent,
        "Wait for events that breaks into the debugger" );
    python::def( "getNumberProcessors", &getNumberProcessors,
        "Get the number of actual processors in the machine" );
    python::def( "getPageSize", &getPageSize,
        "Get the page size for the currently executing processor context" );
    python::def( "getContext", &getThreadContext,
        "Get context of current thread (register values)" );
    python::def( "getLocals", &getLocals, getLocals_( python::args( "ctx" ),
        "Get list of local variables" ) );
    python::def( "setBp", &setSoftwareBp, setSoftwareBp_( python::args( "offset", "callback" ),
        "Set software breakpoint on executiont" ) );
    python::def( "setBp", &setHardwareBp, setHardwareBp_( python::args( "offset", "size", "accsessType", "callback" ) ,
        "Set hardware breakpoint" ) );
    python::def( "getAllBp", &getAllBp,
        "Get all breapoint IDs" );
    python::def( "removeBp", &removeBp,
        "Remove breapoint by IDs" );
    python::def( "removeBp", &removeAllBp,
        "Remove all breapoints" );
}

};  // namespace pykd

////////////////////////////////////////////////////////////////////////////////
