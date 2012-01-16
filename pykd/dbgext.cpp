#include "stdafx.h"

#include <dbgeng.h>
#include <dia2.h>

#include <boost/tokenizer.hpp>

#include "windbg.h"
#include "module.h"
#include "diawrapper.h"
#include "dbgclient.h"
#include "dbgio.h"
#include "dbgpath.h"
#include "dbgcmd.h"
#include "dbgevent.h"
#include "typeinfo.h"
#include "typedvar.h"
#include "dbgmem.h"
#include "intbase.h"
#include "process.h"
#include "pykdver.h"

using namespace pykd;

////////////////////////////////////////////////////////////////////////////////

static const std::string pykdVersion = PYKD_VERSION_BUILD;

////////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(
  __in  HINSTANCE /*hinstDLL*/,
  __in  DWORD fdwReason,
  __in  LPVOID /*lpvReserved*/
)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        CoInitialize(NULL);
        break;

    case DLL_PROCESS_DETACH:
        CoUninitialize();
        break;
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

static python::dict genDict(const pyDia::Symbol::ValueNameEntry srcValues[], size_t cntValues)
{
    python::dict resDict;
    for (size_t i = 0; i < cntValues; ++i)
        resDict[srcValues[i].first] = srcValues[i].second;
    return resDict;
}

////////////////////////////////////////////////////////////////////////////////

std::string
getDebuggerImage()
{
    std::vector<char>   buffer(MAX_PATH);
    GetModuleFileNameExA( GetCurrentProcess(), NULL, &buffer[0], (DWORD)buffer.size() );
    return std::string( &buffer[0] );
}

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
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( pyDia_Symbol_findChildrenEx, pyDia::Symbol::findChildrenEx, 1, 3 );

#define DEF_PY_CONST_ULONG(x)    \
    python::scope().attr(#x) = ULONG(##x)

BOOST_PYTHON_MODULE( pykd )
{
    python::scope().attr("version") = pykdVersion;

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
        .def( "getImplicitThread", &getImplicitThread, 
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
    python::def( "debuggerPath", &getDebuggerImage,
        "Return full path to the process image that uses pykd" );
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

    python::class_<TypeInfo, TypeInfoPtr, python::bases<intBase>, boost::noncopyable >("typeInfo", "Class representing typeInfo", python::no_init )
        .def( "name", &TypeInfo::getName )
        .def( "size", &TypeInfo::getSize )
        .def( "offset", &TypeInfo::getOffset )
        .def( "bitOffset", &TypeInfo::getBitOffset )
        .def( "bitWidth", &TypeInfo::getBitWidth )
        .def( "field", &TypeInfo::getField )
        .def( "__getattr__", &TypeInfo::getField )
        .def( "asMap", &TypeInfo::asMap )
        .def( "deref", &TypeInfo::deref );

    python::class_<TypedVar, TypedVarPtr, python::bases<intBase>, boost::noncopyable >("typedVar", 
        "Class of non-primitive type object, child class of typeClass. Data from target is copied into object instance", 
        python::no_init )
        .def("getAddress", &TypedVar::getAddress, 
            "Return virtual address" )
        .def("sizeof", &TypedVar::getSize,
            "Return size of a variable in the target memory" )
        .def("offset", &TypedVar::getOffset,
            "Return offset to parent" )
        .def("field", &TypedVar::getField,
            "Return field of structure as an object attribute" )
        .def( "dataKind", &TypedVar::getDataKind,
            "Retrieves the variable classification of a data: DataIsXxx")
        .def("deref", &TypedVar::deref,
            "Return value by pointer" )
        .def("__getattr__", &TypedVar::getField,
            "Return field of structure as an object attribute" )
        .def( "__str__", &TypedVar::print )
        .def("__len__", &TypedVar::getElementCount )
        .def("__getitem__", &TypedVar::getElementByIndex )
        .def("__getitem__", &TypedVar::getElementByIndexPtr );

    python::class_<Module, python::bases<intBase> >("module", "Class representing executable module", python::no_init )
        .def("begin", &Module::getBase,
             "Return start address of the module" )
        .def("end", &Module::getEnd,
             "Return end address of the module" )
        .def("size", &Module::getSize,
              "Return size of the module" )
        .def("name", &Module::getName,
             "Return name of the module" )      
        .def("image", &Module::getImageName,
             "Return name of the image of the module" )
        .def("pdb", &Module::getPdbName,
             "Return the full path to the module's pdb file ( symbol information )" )
        .def("reload", &Module::reloadSymbols,
            "(Re)load symbols for the module" )
        .def("offset", &Module::getSymbol,
            "Return offset of the symbol" )
        .def("rva", &Module::getSymbolRva,
            "Return rva of the symbol" )
        .def("type", &Module::getTypeByName,
            "Return typeInfo class by type name" )
        .def("typedVar", &Module::getTypedVarByAddr,
            "Return a typedVar class instance" )
        .def("typedVar",&Module::getTypedVarByName,
            "Return a typedVar class instance" )
        .def("typedVar",&Module::getTypedVarByType,
            "Return a typedVar class instance" )
        .def("typedVar",&Module::getTypedVarByTypeName,
            "Return a typedVar class instance" )
        .def("typedVarList", &Module::getTypedVarListByTypeName,
            "Return a list of the typedVar class instances. Each item represents an item of the linked list in the target memory" )
        .def("typedVarList", &Module::getTypedVarListByType,
            "Return a list of the typedVar class instances. Each item represents an item of the linked list in the target memory" )
        .def("typedVarArray", &Module::getTypedVarArrayByTypeName,
            "Return a list of the typedVar class instances. Each item represents an item of the counted array in the target memory" )
        .def("typedVarArray", &Module::getTypedVarArrayByType,
            "Return a list of the typedVar class instances. Each item represents an item of the counted array in the target memory" )
        .def("containingRecord", &Module::containingRecordByName,
            "Return instance of the typedVar class. It's value are loaded from the target memory."
            "The start address is calculated by the same method as the standard macro CONTAINING_RECORD does" )
        .def("containingRecord", &Module::containingRecordByType,
            "Return instance of the typedVar class. It's value are loaded from the target memory."
            "The start address is calculated by the same method as the standard macro CONTAINING_RECORD does" )
        .def("__getattr__", &Module::getSymbol,
            "Return address of the symbol" );

    python::class_<DbgOut>( "dout", "dout", python::no_init )
        .def( "write", &DbgOut::write );

    python::class_<DbgIn>( "din", "din", python::no_init )
        .def( "readline", &DbgIn::readline );

    python::class_<DbgExtension, DbgExtensionPtr>("ext", python::no_init )
        .def( "call", &DbgExtension::call,
            "Call debug extension command end return it's result as a string" );

    python::class_<EventHandlerWrap, boost::noncopyable>(
        "eventHandler", "Base class for overriding and handling debug notifications" )
        .def( python::init<>() )
        .def( python::init<DebugClientPtr&>() )
        .def( "onBreakpoint", &pykd::EventHandlerWrap::onBreakpoint,
            "Triggered breakpoint event. Parameter is dict:\n"
            "{\"Id\":int, \"BreakType\":int, \"ProcType\":int, \"Flags\":int, \"Offset\":int,"
            " \"Size\":int, \"AccessType\":int, \"PassCount\":int, \"CurrentPassCount\":int,"
            " \"MatchThreadId\":int, \"Command\":str, \"OffsetExpression\":str}\n"
            "Detailed information: http://msdn.microsoft.com/en-us/library/ff539284(VS.85).aspx \n"
            "For ignore event method must return DEBUG_STATUS_NO_CHANGE value" )
        .def( "onException", &pykd::EventHandlerWrap::onException,
            "Exception event. Parameter is dict:\n"
            "{\"Code\":int, \"Flags\":int, \"Record\":int, \"Address\":int,"
            " \"Parameters\":[int], \"FirstChance\":bool}\n"
            "Detailed information: http://msdn.microsoft.com/en-us/library/aa363082(VS.85).aspx \n"
            "For ignore event method must return DEBUG_STATUS_NO_CHANGE value" )
        .def( "onLoadModule", &pykd::EventHandlerWrap::onLoadModule,
            "Load module event. Parameter is instance of module.\n"
            "For ignore event method must return DEBUG_STATUS_NO_CHANGE value" )
        .def( "onUnloadModule", &pykd::EventHandlerWrap::onUnloadModule,
            "Unload module event. Parameter is base address of unloaded module.\n"
            "For ignore event method must return DEBUG_STATUS_NO_CHANGE value" );

    python::class_<Disasm>("disasm", "Class disassemble a processor instructions" )
        .def( python::init<>( "constructor" ) )
        .def( python::init<ULONG64>( boost::python::args("offset"), "constructor" ) )
        .def( "disasm", &Disasm::disassemble, "Disassemble next instruction" )
        .def( "asm", &Disasm::assembly, "Insert assemblied instuction to current offset" )
        .def( "begin", &Disasm::begin, "Return begin offset" )
        .def( "current", &Disasm::current, "Return current offset" )
        .def( "length", &Disasm::length, "Return current instruction length" )
        .def( "instruction", &Disasm::instruction, "Returm current disassembled instruction" )
        .def( "ea", &Disasm::ea, "Return effective address for last disassembled instruction or 0" )
        .def( "reset", &Disasm::reset, "Reset current offset to begin" );

   python::class_<DEBUG_STACK_FRAME>( "stackFrame", 
         "Class representing a frame of the call satck", python::no_init )
        .def_readonly( "instructionOffset", &DEBUG_STACK_FRAME::InstructionOffset,
            "Return a frame's instruction offset" )
        .def_readonly( "returnOffset", &DEBUG_STACK_FRAME::ReturnOffset,
            "Return a frame's return offset" )
        .def_readonly( "frameOffset", &DEBUG_STACK_FRAME::FrameOffset,
            "Return a frame's offset" )
        .def_readonly( "stackOffset", &DEBUG_STACK_FRAME::StackOffset,
            "Return a frame's stack offset" )
        .def_readonly( "frameNumber", &DEBUG_STACK_FRAME::FrameNumber,
            "Return a frame's number" );

   python::class_<Ctx::Registers, Ctx::ContextPtr>(
        "Context", "Context of thread (register values)", python::no_init )
        .def( "ip", &Ctx::Registers::getIp, 
            "Get instruction pointer register" )
        .def( "retreg", &Ctx::Registers::getIp, 
            "Get primary return value register" )
        .def( "csp", &Ctx::Registers::getSp, 
            "Get current stack pointer" )
        .def( "get", &Ctx::Registers::getValue, 
            "Get register value by ID (CV_REG_XXX)" )
        .def( "processorType", &Ctx::Registers::getProcessorType,
            "Get processor type as string")
        .def("__len__", &Ctx::Registers::getCount,
            "Return count of registers")
        .def("__getitem__", &Ctx::Registers::getByIndex,
            "Return tuple<ID, VALUE> by index");

    python::def( "diaLoadPdb", &pyDia::GlobalScope::loadPdb, 
        "Open pdb file for querying debug symbols. Return DiaSymbol of global scope");

    python::class_<pyDia::Symbol, pyDia::SymbolPtr>(
        "DiaSymbol", "class wrapper for MS DIA Symbol", python::no_init )
        .def( "findEx", &pyDia::Symbol::findChildrenEx, pyDia_Symbol_findChildrenEx( python::args( "symTag", "name", "cmpFlags" ) ,
            "Retrieves the children of the symbol" ) )
        .def( "find", &pyDia::Symbol::findChildren, 
            "Retrieves the children of the symbol" )
        .def( "size", &pyDia::Symbol::getSize, 
            "Retrieves the number of bits or bytes of memory used by the object represented by this symbol" )
        .def( "name", &pyDia::Symbol::getName, 
            "Retrieves the name of the symbol" )
        .def( "type", &pyDia::Symbol::getType, 
            "Retrieves the symbol that represents the type for this symbol" )
        .def( "indexType", &pyDia::Symbol::getIndexType, 
            "Retrieves a reference to the class parent of the symbol" )
        .def( "rva", &pyDia::Symbol::getRva,
            "Retrieves the relative virtual address (RVA) of the location")
        .def( "symTag", &pyDia::Symbol::getSymTag, 
            "Retrieves the symbol type classifier: SymTagXxx" )
        .def( "locType", &pyDia::Symbol::getLocType, 
            "Retrieves the location type of a data symbol: LocIsXxx" )
        .def( "offset", &pyDia::Symbol::getOffset, 
            "Retrieves the offset of the symbol location" )
        .def( "count", &pyDia::Symbol::getCount, 
            "Retrieves the number of items in a list or array" )
        .def( "value", (python::object(pyDia::Symbol::*)())&pyDia::Symbol::getValue,
            "Retrieves the value of a constant")
        .def( "isBasic", &pyDia::Symbol::isBasicType,
            "Retrieves a flag of basic type for symbol")
        .def( "baseType", &pyDia::Symbol::getBaseType,
            "Retrieves the base type for this symbol")
        .def( "bitPos", &pyDia::Symbol::getBitPosition,
            "Retrieves the base type for this symbol")
        .def( "indexId", &pyDia::Symbol::getIndexId,
            "Retrieves the unique symbol identifier")
        .def( "udtKind", &pyDia::Symbol::getUdtKind,
            "Retrieves the variety of a user-defined type")
        .def( "dataKind", &pyDia::Symbol::getDataKind,
            "Retrieves the variable classification of a data symbol")
        .def("registerId", &pyDia::Symbol::getRegisterId,
            "Retrieves the register designator of the location:\n"
            "CV_REG_XXX (for IMAGE_FILE_MACHINE_I386) or CV_AMD64_XXX (for IMAGE_FILE_MACHINE_AMD64)")
        .def("machineType", &pyDia::Symbol::getMachineType, 
            "Retrieves the type of the target CPU: IMAGE_FILE_MACHINE_XXX")
        .def( "__str__", &pyDia::Symbol::print)
        .def("__getitem__", &pyDia::Symbol::getChildByName)
        .def("__len__", &pyDia::Symbol::getChildCount )
        .def("__getitem__", &pyDia::Symbol::getChildByIndex);

    python::class_<pyDia::GlobalScope, pyDia::GlobalScopePtr, python::bases<pyDia::Symbol> >(
        "DiaScope", "class wrapper for MS DIA Symbol", python::no_init )
        .def("findByRva", &pyDia::GlobalScope::findByRva, 
            "Find symbol by RVA. Return tuple: (DiaSymbol, offset)")
        .def("symbolById", &pyDia::GlobalScope::getSymbolById, 
            "Retrieves a symbol by its unique identifier: DiaSymbol::indexId()");

    // CPU type:
    DEF_PY_CONST_ULONG(IMAGE_FILE_MACHINE_I386);
    DEF_PY_CONST_ULONG(IMAGE_FILE_MACHINE_IA64);
    DEF_PY_CONST_ULONG(IMAGE_FILE_MACHINE_AMD64);

    // type of symbol
    DEF_PY_CONST_ULONG(SymTagNull);
    DEF_PY_CONST_ULONG(SymTagExe);
    DEF_PY_CONST_ULONG(SymTagCompiland);
    DEF_PY_CONST_ULONG(SymTagCompilandDetails);
    DEF_PY_CONST_ULONG(SymTagCompilandEnv);
    DEF_PY_CONST_ULONG(SymTagFunction);
    DEF_PY_CONST_ULONG(SymTagBlock);
    DEF_PY_CONST_ULONG(SymTagData);
    DEF_PY_CONST_ULONG(SymTagAnnotation);
    DEF_PY_CONST_ULONG(SymTagLabel);
    DEF_PY_CONST_ULONG(SymTagPublicSymbol);
    DEF_PY_CONST_ULONG(SymTagUDT);
    DEF_PY_CONST_ULONG(SymTagEnum);
    DEF_PY_CONST_ULONG(SymTagFunctionType);
    DEF_PY_CONST_ULONG(SymTagPointerType);
    DEF_PY_CONST_ULONG(SymTagArrayType);
    DEF_PY_CONST_ULONG(SymTagBaseType);
    DEF_PY_CONST_ULONG(SymTagTypedef);
    DEF_PY_CONST_ULONG(SymTagBaseClass);
    DEF_PY_CONST_ULONG(SymTagFriend);
    DEF_PY_CONST_ULONG(SymTagFunctionArgType);
    DEF_PY_CONST_ULONG(SymTagFuncDebugStart);
    DEF_PY_CONST_ULONG(SymTagFuncDebugEnd);
    DEF_PY_CONST_ULONG(SymTagUsingNamespace);
    DEF_PY_CONST_ULONG(SymTagVTableShape);
    DEF_PY_CONST_ULONG(SymTagVTable);
    DEF_PY_CONST_ULONG(SymTagCustom);
    DEF_PY_CONST_ULONG(SymTagThunk);
    DEF_PY_CONST_ULONG(SymTagCustomType);
    DEF_PY_CONST_ULONG(SymTagManagedType);
    DEF_PY_CONST_ULONG(SymTagDimension);
    python::scope().attr("diaSymTagName") = 
        genDict(pyDia::Symbol::symTagName, _countof(pyDia::Symbol::symTagName));

    DEF_PY_CONST_ULONG(DataIsUnknown);
    DEF_PY_CONST_ULONG(DataIsLocal);
    DEF_PY_CONST_ULONG(DataIsStaticLocal);
    DEF_PY_CONST_ULONG(DataIsParam);
    DEF_PY_CONST_ULONG(DataIsObjectPtr);
    DEF_PY_CONST_ULONG(DataIsFileStatic);
    DEF_PY_CONST_ULONG(DataIsGlobal);
    DEF_PY_CONST_ULONG(DataIsMember);
    DEF_PY_CONST_ULONG(DataIsStaticMember);
    DEF_PY_CONST_ULONG(DataIsConstant);
    python::scope().attr("diaDataKind") = 
        genDict(pyDia::Symbol::dataKindName, _countof(pyDia::Symbol::dataKindName));

    // search options for symbol and file names
    DEF_PY_CONST_ULONG(nsfCaseSensitive);
    DEF_PY_CONST_ULONG(nsfCaseInsensitive);
    DEF_PY_CONST_ULONG(nsfFNameExt);
    DEF_PY_CONST_ULONG(nsfRegularExpression);
    DEF_PY_CONST_ULONG(nsfUndecoratedName);
    DEF_PY_CONST_ULONG(nsCaseSensitive);
    DEF_PY_CONST_ULONG(nsCaseInsensitive);
    DEF_PY_CONST_ULONG(nsFNameExt);
    DEF_PY_CONST_ULONG(nsRegularExpression);
    DEF_PY_CONST_ULONG(nsCaseInRegularExpression);

    // location type
    DEF_PY_CONST_ULONG(LocIsNull);
    DEF_PY_CONST_ULONG(LocIsStatic);
    DEF_PY_CONST_ULONG(LocIsTLS);
    DEF_PY_CONST_ULONG(LocIsRegRel);
    DEF_PY_CONST_ULONG(LocIsThisRel);
    DEF_PY_CONST_ULONG(LocIsEnregistered);
    DEF_PY_CONST_ULONG(LocIsBitField);
    DEF_PY_CONST_ULONG(LocIsSlot);
    DEF_PY_CONST_ULONG(LocIsIlRel);
    DEF_PY_CONST_ULONG(LocInMetaData);
    DEF_PY_CONST_ULONG(LocIsConstant);
    python::scope().attr("diaLocTypeName") = 
        genDict(pyDia::Symbol::locTypeName, _countof(pyDia::Symbol::locTypeName));

    DEF_PY_CONST_ULONG(btNoType);
    DEF_PY_CONST_ULONG(btVoid);
    DEF_PY_CONST_ULONG(btChar);
    DEF_PY_CONST_ULONG(btWChar);
    DEF_PY_CONST_ULONG(btInt);
    DEF_PY_CONST_ULONG(btUInt);
    DEF_PY_CONST_ULONG(btFloat);
    DEF_PY_CONST_ULONG(btBCD);
    DEF_PY_CONST_ULONG(btBool);
    DEF_PY_CONST_ULONG(btLong);
    DEF_PY_CONST_ULONG(btULong);
    DEF_PY_CONST_ULONG(btCurrency);
    DEF_PY_CONST_ULONG(btDate);
    DEF_PY_CONST_ULONG(btVariant);
    DEF_PY_CONST_ULONG(btComplex);
    DEF_PY_CONST_ULONG(btBit);
    DEF_PY_CONST_ULONG(btBSTR);
    DEF_PY_CONST_ULONG(btHresult);
    python::scope().attr("diaBasicType") = 
        genDict(pyDia::Symbol::basicTypeName, pyDia::Symbol::cntBasicTypeName);

    DEF_PY_CONST_ULONG(UdtStruct);
    DEF_PY_CONST_ULONG(UdtClass);
    DEF_PY_CONST_ULONG(UdtUnion);
    python::scope().attr("diaUdtKind") = 
        genDict(pyDia::Symbol::udtKindName, pyDia::Symbol::cntUdtKindName);

    // i386/amd64 cpu registers
#include "diaregs.h"
    python::scope().attr("diaI386Regs") = 
        genDict(pyDia::Symbol::i386RegName, pyDia::Symbol::cntI386RegName);
    python::scope().attr("diaAmd64Regs") = 
        genDict(pyDia::Symbol::amd64RegName, pyDia::Symbol::cntAmd64RegName);

   // exception:

    // wrapper for standart python exceptions
    python::register_exception_translator<PyException>( &PyException::exceptionTranslate );

    // BaseException
    ExceptionTranslator<DbgException>::setTypeObject( 
        python::class_<DbgException>( "BaseException", "Pykd base exception class", python::no_init )
            .def( "__str__", &DbgException::getDesc )
            .def( "__repr__", &DbgException::getDesc )
            .ptr()
            );
  
    // DIA exception
    ExceptionTranslator<pyDia::Exception>::setTypeObject(
        python::class_<pyDia::Exception, python::bases<DbgException> >( 
            "DiaException", "Debug interface access exception", python::no_init)
            .ptr()
            );

    // Memory exception
    ExceptionTranslator<MemoryException>::setTypeObject(
        python::class_<MemoryException, python::bases<DbgException> >(
            "MemoryException", "Target memory access exception class", python::no_init )
            .def( "getAddress", &pykd::MemoryException::getAddress, "Return a target address where the exception occurs" )
            .ptr()
            );

    // WaitEventException
    ExceptionTranslator<WaitEventException>::setTypeObject(
        python::class_<WaitEventException, python::bases<DbgException> >( 
            "WaitEventException", "Debug interface access exception", python::no_init)
            .ptr()
            );

    // AddSyntheticSymbolException
    ExceptionTranslator<AddSyntheticSymbolException>::setTypeObject(
        python::class_<AddSyntheticSymbolException, python::bases<DbgException> >( 
            "AddSynSymbolException", "Add new synthetic symbol exception", python::no_init)
            .ptr()
            );


//
//#define _DECL_BASE_EXCEPT_STR   .def( "__repr__", &pykd::DbgException::print )
//
//    // base exception
//    python::class_<pykd::DbgException>  dbgExceptionClass( "BaseException",
//        "Pykd base exception class",
//        python::no_init );
//    dbgExceptionClass
//        .def( python::init<std::string>( python::args("desc"), "constructor" ) )
//        .def( "desc", &pykd::DbgException::getDesc,
//            "Get exception description" )
//        _DECL_BASE_EXCEPT_STR;
//    pykd::DbgException::setTypeObject( dbgExceptionClass.ptr() );
//    python::register_exception_translator<pykd::DbgException>( 
//        &pykd::DbgException::exceptionTranslate );
//
//    // DIA exceptions
//    python::class_<pyDia::Exception, python::bases<DbgException> > diaException( 
//        "DiaException", "Debug interface access exception", python::no_init);
//    diaException
//        .def( "hres", &pyDia::Exception::getRes )
//        _DECL_BASE_EXCEPT_STR;
//    pyDia::Exception::setTypeObject( diaException.ptr() );
//    python::register_exception_translator<pyDia::Exception>( 
//        &pyDia::Exception::exceptionTranslate );
//
//    // Memory exception
//    python::class_<pykd::MemoryException, python::bases<DbgException> > memException(
//        "MemoryException", "Target memory access exception class",
//        python::no_init );
//    memException.def( "getAddress", &pykd::MemoryException::getAddress, "Return a target address where the exception occurs" );
//    pykd::MemoryException::setTypeObject( memException.ptr() );
//    python::register_exception_translator<pykd::MemoryException>(
//        &pykd::MemoryException::exceptionTranslate );
//
//    // Wait debug event exception
//    python::class_<WaitEventException, python::bases<DbgException> > waitEventException( 
//        "WaitEventException", "Debug interface access exception", python::no_init);
//    waitEventException
//        _DECL_BASE_EXCEPT_STR;
//    WaitEventException::setTypeObject( waitEventException.ptr() );
//    python::register_exception_translator<WaitEventException>( 
//        &WaitEventException::exceptionTranslate );

    DEF_PY_CONST_ULONG( DEBUG_CLASS_UNINITIALIZED );
    DEF_PY_CONST_ULONG( DEBUG_CLASS_KERNEL );
    DEF_PY_CONST_ULONG( DEBUG_CLASS_USER_WINDOWS );

    DEF_PY_CONST_ULONG( DEBUG_KERNEL_CONNECTION );
    DEF_PY_CONST_ULONG( DEBUG_KERNEL_LOCAL );
    DEF_PY_CONST_ULONG( DEBUG_KERNEL_EXDI_DRIVER );
    DEF_PY_CONST_ULONG( DEBUG_KERNEL_SMALL_DUMP );
    DEF_PY_CONST_ULONG( DEBUG_KERNEL_DUMP );
    DEF_PY_CONST_ULONG( DEBUG_KERNEL_FULL_DUMP );

    DEF_PY_CONST_ULONG( DEBUG_USER_WINDOWS_PROCESS );
    DEF_PY_CONST_ULONG( DEBUG_USER_WINDOWS_PROCESS_SERVER );
    DEF_PY_CONST_ULONG( DEBUG_USER_WINDOWS_SMALL_DUMP );
    DEF_PY_CONST_ULONG( DEBUG_USER_WINDOWS_DUMP );

    // debug status
    DEF_PY_CONST_ULONG(DEBUG_STATUS_NO_CHANGE);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_GO);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_GO_HANDLED);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_GO_NOT_HANDLED);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_STEP_OVER);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_STEP_INTO);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_BREAK);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_NO_DEBUGGEE);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_STEP_BRANCH);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_RESTART_REQUESTED);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_REVERSE_GO);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_REVERSE_STEP_BRANCH);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_REVERSE_STEP_OVER);
    DEF_PY_CONST_ULONG(DEBUG_STATUS_REVERSE_STEP_INTO);
}

#undef DEF_PY_CONST_ULONG

////////////////////////////////////////////////////////////////////////////////

WindbgGlobalSession::WindbgGlobalSession() {
                 
    PyImport_AppendInittab("pykd", initpykd ); 

    PyEval_InitThreads();

    Py_Initialize();    

    main = boost::python::import("__main__");
    
    python::object   main_namespace = main.attr("__dict__");

    // делаем аналог from pykd import *        
    python::object   pykd = boost::python::import( "pykd" );
    
    python::dict     pykd_namespace( pykd.attr("__dict__") ); 
    
    python::list     iterkeys( pykd_namespace.iterkeys() );
    
    for (int i = 0; i < boost::python::len(iterkeys); i++)
    {
        std::string     key = boost::python::extract<std::string>(iterkeys[i]);
               
        main_namespace[ key ] = pykd_namespace[ key ];
    }   

    pyState = PyEval_SaveThread();
}


volatile LONG            WindbgGlobalSession::sessionCount = 0;

WindbgGlobalSession     *WindbgGlobalSession::windbgGlobalSession = NULL; 

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

////////////////////////////////////////////////////////////////////////////////

VOID
CALLBACK
DebugExtensionUninitialize()
{
    WindbgGlobalSession::StopWindbgSession();
}

////////////////////////////////////////////////////////////////////////////////

HRESULT 
CALLBACK
py( PDEBUG_CLIENT4 client, PCSTR args )
{
    DebugClientPtr      dbgClient = DebugClient::createDbgClient( client );
    DebugClientPtr      oldClient = DebugClient::setDbgClientCurrent( dbgClient );

    WindbgGlobalSession::RestorePyState();

    PyThreadState   *globalInterpreter = PyThreadState_Swap( NULL );
    PyThreadState   *localInterpreter = Py_NewInterpreter();

    try {

        // получаем достпу к глобальному мапу ( нужен для вызова exec_file )
        python::object       main =  python::import("__main__");

        python::object       global(main.attr("__dict__"));

        // настраиваем ввод/вывод ( чтобы в скрипте можно было писать print )

        python::object       sys = python::import("sys");
       
        sys.attr("stdout") = python::object( dbgClient->dout() );
        sys.attr("stderr") = python::object( dbgClient->dout() );
        sys.attr("stdin") = python::object( dbgClient->din() );

        // импортируем модуль обработки исключений ( нужен для вывода traceback а )
        boost::python::object       tracebackModule = python::import("traceback");
        
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
        std::string     fullScriptName;
        DbgPythonPath   dbgPythonPath;        
        
        if ( !dbgPythonPath.getFullFileName( argsList[0], fullScriptName ) )
        {
            dbgClient->eprintln( L"script file not found" );            
        }
        else
        try {             
      
            python::object       result;
    
            result =  python::exec_file( fullScriptName.c_str(), global, global );
        }                
        catch( boost::python::error_already_set const & )
        {
            // ошибка в скрипте
            PyObject    *errtype = NULL, *errvalue = NULL, *traceback = NULL;
            
            PyErr_Fetch( &errtype, &errvalue, &traceback );

            PyErr_NormalizeException( &errtype, &errvalue, &traceback );

            if(errvalue != NULL) 
            {
                if ( PyErr_GivenExceptionMatches(errtype, PyExc_SyntaxError))
                {
                    PyObject *filenamePtr = PyObject_GetAttrString(errvalue ,"filename" );
                    PyObject *filenameStr = PyUnicode_FromObject(filenamePtr);
                    PyObject *linenoPtr = PyObject_GetAttrString(errvalue ,"lineno");
                    PyObject *offsetPtr = PyObject_GetAttrString(errvalue ,"offset");
                    PyObject *msg = PyObject_GetAttrString(errvalue, "msg");
                    PyObject *msgStr = PyUnicode_FromObject(msg);    

                    std::wstringstream       sstr;
                    sstr << std::endl << L"Syntax error" << std::endl;
                    sstr << std::endl << PyUnicode_AS_UNICODE( msgStr ) << std::endl << std::endl;
                    sstr << "Filename: " << PyUnicode_AS_UNICODE( filenameStr );
                    sstr << "  Line: " << PyInt_AsLong( linenoPtr );
                    sstr << "  Pos: " << PyInt_AsLong( offsetPtr );
                    sstr << std::endl;

                    dbgClient->eprintln( sstr.str() );

                    Py_XDECREF( linenoPtr );
                    Py_XDECREF( offsetPtr );
                    Py_XDECREF( msgStr );
                    Py_XDECREF( msg );
                }
                else    
                {
                    PyObject *errvalueStr= PyUnicode_FromObject(errvalue);    

                    if ( errvalueStr )
                    {
                        dbgClient->eprintln( PyUnicode_AS_UNICODE( errvalueStr ) );
                        Py_DECREF(errvalueStr);
                    }

                    if ( traceback )
                    {
                        python::object    traceObj( python::handle<>( python::borrowed( traceback ) ) );
                        
                        dbgClient->eprintln( L"\nTraceback:" );

                        python::object   pFunc( tracebackModule.attr("format_tb") );
                        python::list     traceList( pFunc( traceObj ) );

                        for ( long i = 0; i < python::len(traceList); ++i )
                            dbgClient->eprintln( python::extract<std::wstring>(traceList[i]) );
                    }
                }                
            }

            Py_XDECREF(errvalue);
            Py_XDECREF(errtype);
            Py_XDECREF(traceback);        
        }  

    }
    catch(...)
    {      
        dbgClient->eprintln( L"unexpected error" );
    }    

    Py_EndInterpreter( localInterpreter ); 
    PyThreadState_Swap( globalInterpreter );

    WindbgGlobalSession::SavePyState();

    DebugClient::setDbgClientCurrent( oldClient );

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////

HRESULT 
CALLBACK
pycmd( PDEBUG_CLIENT4 client, PCSTR args )
{
    DebugClientPtr      dbgClient = DebugClient::createDbgClient( client );
    DebugClientPtr      oldClient = DebugClient::setDbgClientCurrent( dbgClient );

    WindbgGlobalSession::RestorePyState();
    
    ULONG    mask = 0;
    client->GetOutputMask( &mask );

    try {

        // перенаправление стандартных потоков ВВ
        python::object       sys = python::import("sys");
       
        sys.attr("stdout") = python::object( DbgOut( client ) );
        sys.attr("stderr") = python::object( DbgOut( client ) );
        sys.attr("stdin") = python::object( DbgIn( client ) );

        client->SetOutputMask( DEBUG_OUTPUT_NORMAL );

        PyRun_String(
            "__import__('code').InteractiveConsole(__import__('__main__').__dict__).interact()", 
            Py_file_input,
            WindbgGlobalSession::global().ptr(),
            WindbgGlobalSession::global().ptr()
            );
    }
    catch(...)
    {      
        dbgClient->eprintln( L"unexpected error" );
    }    

    client->SetOutputMask( mask );

    WindbgGlobalSession::SavePyState();

    DebugClient::setDbgClientCurrent( oldClient );

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
