#include "stdafx.h"

#include <wdbgexts.h>

#include <vector>
#include <string>

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/tokenizer.hpp>
#include <boost/python/overloads.hpp>

#include "dbgext.h"
#include "dbgprint.h"
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
#include "dbgsession.h"
#include "dbgcallback.h"
#include "dbgpath.h"
#include "dbginput.h"
#include "dbgprocess.h"
#include "dbgsynsym.h"

//////////////////////////////////////////////////////////////////////////////

// указатель на текущий интерфейс
DbgExt    *dbgExt = NULL;

static bool isWindbgExt();

//////////////////////////////////////////////////////////////////////////////

BOOST_PYTHON_FUNCTION_OVERLOADS( dprint, DbgPrint::dprint, 1, 2 )
BOOST_PYTHON_FUNCTION_OVERLOADS( dprintln, DbgPrint::dprintln, 1, 2 )

BOOST_PYTHON_FUNCTION_OVERLOADS( loadCharsOv, loadChars, 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadBytes, loadArray<unsigned char>, 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadWords, loadArray<unsigned short>, 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadDWords, loadArray<unsigned long>, 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadQWords, loadArray<unsigned __int64> , 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignBytes, loadArray<char> , 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignWords, loadArray<short> , 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignDWords, loadArray<long> , 2, 3 )
BOOST_PYTHON_FUNCTION_OVERLOADS( loadSignQWords, loadArray<__int64>, 2, 3 )

BOOST_PYTHON_FUNCTION_OVERLOADS( compareMemoryOver, compareMemory, 3, 4 )

#define _DEF_PY_CONST(x)    \
    boost::python::scope().attr(#x) = ##x

BOOST_PYTHON_MODULE( pykd )
{
    boost::python::def( "go", &setExecutionStatus<DEBUG_STATUS_GO>, 
        "Change debugger status to DEBUG_STATUS_GO"  );
    boost::python::def( "trace", &setExecutionStatus<DEBUG_STATUS_STEP_INTO>, 
        "Change debugger status to DEBUG_STATUS_STEP_INTO" );
    boost::python::def( "step", &setExecutionStatus<DEBUG_STATUS_STEP_OVER>, 
        "Change debugger status to DEBUG_STATUS_STEP_OVER" );   
    boost::python::def( "expr", &evaluate, 
        "Evaluate windbg expression" ); 
    boost::python::def( "isWindbgExt", &isWindbgExt, 
        "Check if script works in windbg context" );
    boost::python::def( "symbolsPath", &dbgSymPath, 
        "Return symbol path" );
    boost::python::def( "dprint", &DbgPrint::dprint, dprint( boost::python::args( "str", "dml" ), 
        "Print out string. If dml = True string is printed with dml highlighting ( only for windbg )") );
    boost::python::def( "dprintln", &DbgPrint::dprintln, dprintln( boost::python::args( "str", "dml" ), 
        "Print out string and insert end of line symbol. If dml = True string is printed with dml highlighting ( only for windbg )"  ) );
    boost::python::def( "loadDump", &dbgLoadDump,
        "Load crash dump (only for console)");
    boost::python::def( "startProcess", &startProcess,
        "Start process for debugging(only for console)");    
    boost::python::def( "dbgCommand", &dbgCommand,
        "Execute debugger command. For example: dbgCommand( \"lmvm nt\" )" );
    boost::python::def( "isValid", &isOffsetValid,
        "Check if virtual address is valid" );
    boost::python::def( "is64bitSystem", &is64bitSystem,
        "Check if target system has 64 address space" );
    boost::python::def( "isKernelDebugging", &isKernelDebugging,
        "Check if kernel dubugging is running" );
    boost::python::def( "ptrSize", ptrSize,
        "Return pointer size ( in bytes )" );
    boost::python::def( "reg", &loadRegister,
        "Return CPU's register value" );
    boost::python::def( "typedVar", &loadTypedVar,
        "Return instance of the typedVarClass. It's values are loaded from the target memory" );
    boost::python::def( "typedVarList", &loadTypedVarList,
        "Return list of typedVarClass instances. Each item represents one item of the linked list in the target memory" );        
    boost::python::def( "typedVarArray", &loadTypedVarArray,
        "Return list of typedVarClass instances. Each item represents one item of the counted array the target memory" );
    boost::python::def( "containingRecord", &containingRecord,
        "Return instance of the typedVarClass. It's value are loaded from the target memory."
        "The start address is calculated by the same method as standard macro CONTAINING_RECORD" );
    boost::python::def( "getTypeClass", &getTypeClass,
        "Return instance of the typeClass with information about type" );
    boost::python::def( "sizeof", &sizeofType,
        "Return size of type" );
    boost::python::def( "loadModule", &loadModule,
        "Return instance of the dbgBreakpointClass" );
    boost::python::def( "findSymbol", &findSymbolForAddress,
        "Return symbol for specified target address if it exists" );
    boost::python::def( "getOffset", &findAddressForSymbol,
        "Return target address for specified symbol" );
    boost::python::def( "findModule", &findModule,
        "Return instance of the dbgBreakpointClass which posseses specified address" );
    boost::python::def( "addr64", &addr64,
        "Extend address to 64 bits formats ( for x86 )" );
    boost::python::def( "loadChars", loadChars, loadCharsOv( boost::python::args( "address", "number",  "phyAddr" ),
        "Load string from the target buffer" ) );
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

    boost::python::class_<typeClass, boost::shared_ptr<typeClass> >( "typeClass", 
        "Class representing non-primitive type info: structure, union, etc. attributes is a fields of non-primitive type" )
        .def("sizeof", &typeClass::size, 
            "Return full size of non-primitive type" )
        .def("offset", &typeClass::getOffset, 
            "Return offset as field of parent" )
        .def("__str__", &typeClass::print, 
            "Return a nice string represention: print names and offsets of fields");
        
    boost::python::class_<typedVarClass, boost::python::bases<typeClass>, boost::shared_ptr<typedVarClass> >( "typedVarClass", 
        "Class of non-primitive type object, child class of typeClass. Data from target is copied into object instance" )
        .def("getAddress", &typedVarClass::getAddress, 
            "Return virtual address" );
        
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
            "windbg extension",
             boost::python::init<const char*>( boost::python::args("path"), "__init__  dbgExtensionClass" ) ) 
        .def("call", &dbgExtensionClass::call )
        .def("__str__", &dbgExtensionClass::print );
    boost::python::class_<dbgStackFrameClass>( "dbgStackFrameClass", "dbgStackFrameClass" )
        .def_readonly( "instructionOffset", &dbgStackFrameClass::InstructionOffset )
        .def_readonly( "returnOffset", &dbgStackFrameClass::ReturnOffset )
        .def_readonly( "frameOffset", &dbgStackFrameClass::FrameOffset )
        .def_readonly( "stackOffset", &dbgStackFrameClass::StackOffset )
        .def_readonly( "frameNumber", &dbgStackFrameClass::FrameNumber )
        .def( "__str__", &dbgStackFrameClass::print );
    boost::python::class_<dbgOut>( "windbgOut", "windbgOut" )
        .def( "write", &dbgOut::write );
    boost::python::class_<dbgIn>( "windbgIn", "windbgIn" )
        .def( "readline", &dbgIn::readline );                
    boost::python::class_<dbgBreakpointClass>( 
         "bp",
         "break point",
         boost::python::init<ULONG64,boost::python::object&>( boost::python::args("offset", "callback"), "__init__  dbgBreakpointClass" ) ) 
        .def( "set", &dbgBreakpointClass::set )
        .def( "remove", &dbgBreakpointClass::remove )
        .def( "__str__", &dbgBreakpointClass::print );

    // debug status
    _DEF_PY_CONST(DEBUG_STATUS_NO_CHANGE);
    _DEF_PY_CONST(DEBUG_STATUS_GO);
    _DEF_PY_CONST(DEBUG_STATUS_GO_HANDLED);
    _DEF_PY_CONST(DEBUG_STATUS_GO_NOT_HANDLED);
    _DEF_PY_CONST(DEBUG_STATUS_STEP_OVER);
    _DEF_PY_CONST(DEBUG_STATUS_STEP_INTO);
    _DEF_PY_CONST(DEBUG_STATUS_BREAK);
    _DEF_PY_CONST(DEBUG_STATUS_NO_DEBUGGEE);
    _DEF_PY_CONST(DEBUG_STATUS_STEP_BRANCH);
    _DEF_PY_CONST(DEBUG_STATUS_IGNORE_EVENT);
    _DEF_PY_CONST(DEBUG_STATUS_RESTART_REQUESTED);
    _DEF_PY_CONST(DEBUG_STATUS_REVERSE_GO);
    _DEF_PY_CONST(DEBUG_STATUS_REVERSE_STEP_BRANCH);
    _DEF_PY_CONST(DEBUG_STATUS_REVERSE_STEP_OVER);
    _DEF_PY_CONST(DEBUG_STATUS_REVERSE_STEP_INTO);

    // debug status additional mask
    _DEF_PY_CONST(DEBUG_STATUS_INSIDE_WAIT);
    _DEF_PY_CONST(DEBUG_STATUS_WAIT_TIMEOUT);

    // break point type
    _DEF_PY_CONST(DEBUG_BREAKPOINT_CODE);
    _DEF_PY_CONST(DEBUG_BREAKPOINT_DATA);
    _DEF_PY_CONST(DEBUG_BREAKPOINT_TIME);

    // break point flag
    _DEF_PY_CONST(DEBUG_BREAKPOINT_GO_ONLY);
    _DEF_PY_CONST(DEBUG_BREAKPOINT_DEFERRED);
    _DEF_PY_CONST(DEBUG_BREAKPOINT_ENABLED);
    _DEF_PY_CONST(DEBUG_BREAKPOINT_ADDER_ONLY);
    _DEF_PY_CONST(DEBUG_BREAKPOINT_ONE_SHOT);

    // break point access type
    _DEF_PY_CONST(DEBUG_BREAK_READ);
    _DEF_PY_CONST(DEBUG_BREAK_WRITE);
    _DEF_PY_CONST(DEBUG_BREAK_EXECUTE);
    _DEF_PY_CONST(DEBUG_BREAK_IO);

    // exception flags
    _DEF_PY_CONST(EXCEPTION_NONCONTINUABLE);

    // debug events
    _DEF_PY_CONST(DEBUG_EVENT_BREAKPOINT);
    _DEF_PY_CONST(DEBUG_EVENT_EXCEPTION);
    _DEF_PY_CONST(DEBUG_EVENT_CREATE_THREAD);
    _DEF_PY_CONST(DEBUG_EVENT_EXIT_THREAD);
    _DEF_PY_CONST(DEBUG_EVENT_CREATE_PROCESS);
    _DEF_PY_CONST(DEBUG_EVENT_EXIT_PROCESS);
    _DEF_PY_CONST(DEBUG_EVENT_LOAD_MODULE);
    _DEF_PY_CONST(DEBUG_EVENT_UNLOAD_MODULE);
    _DEF_PY_CONST(DEBUG_EVENT_SYSTEM_ERROR);
    _DEF_PY_CONST(DEBUG_EVENT_SESSION_STATUS);
    _DEF_PY_CONST(DEBUG_EVENT_CHANGE_DEBUGGEE_STATE);
    _DEF_PY_CONST(DEBUG_EVENT_CHANGE_ENGINE_STATE);
    _DEF_PY_CONST(DEBUG_EVENT_CHANGE_SYMBOL_STATE);

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
    
        boost::python::import( "pykd" );
        
        main = boost::python::import("__main__");
        
        // перенаправление стандартных потоков ВВ
        boost::python::object       sys = boost::python::import( "sys");
        
        dbgOut                      dout;
        sys.attr("stdout") = boost::python::object( dout );

        dbgIn                       din;
        sys.attr("stdin") = boost::python::object( din );
    }
    
    ~WindbgGlobalSession() {
        Py_Finalize();
    }
   
    boost::python::object           main;
    
    DbgEventCallbacksManager        callbackMgr;
    
    static volatile LONG            sessionCount;      
    
    static WindbgGlobalSession     *windbgGlobalSession;     
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
    masterClient->QueryInterface( __uuidof(IDebugClient), (void **)&client );
    masterClient->QueryInterface( __uuidof(IDebugClient4), (void **)&client4 );
    
    
    masterClient->QueryInterface( __uuidof(IDebugControl), (void **)&control );
    masterClient->QueryInterface( __uuidof(IDebugControl4), (void **)&control4 );
    
    masterClient->QueryInterface( __uuidof(IDebugRegisters), (void **)&registers );
    
    masterClient->QueryInterface( __uuidof(IDebugSymbols), (void ** )&symbols );
    masterClient->QueryInterface( __uuidof(IDebugSymbols2), (void ** )&symbols2 );    
    masterClient->QueryInterface( __uuidof(IDebugSymbols3), (void ** )&symbols3 );      
    
    masterClient->QueryInterface( __uuidof(IDebugDataSpaces), (void **)&dataSpaces );
    masterClient->QueryInterface( __uuidof(IDebugDataSpaces4), (void **)&dataSpaces4 );
    
    masterClient->QueryInterface( __uuidof(IDebugAdvanced2), (void **)&advanced2 );
    
    masterClient->QueryInterface( __uuidof(IDebugSystemObjects), (void**)&system );
    masterClient->QueryInterface( __uuidof(IDebugSystemObjects2), (void**)&system2 );
    
    m_previosExt = dbgExt;
    dbgExt = this;
}

DbgExt::~DbgExt()
{
    if ( client )
        client->Release();
        
    if ( client4 )
        client4->Release();
        
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
        
    dbgExt = m_previosExt;
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
            DWORD       oldCurDirLen = GetCurrentDirectoryA( 0, NULL );

	        std::vector<char> oldCurDirCstr(oldCurDirLen);
            
            GetCurrentDirectoryA( oldCurDirLen, &oldCurDirCstr[0] );
            
           // SetCurrentDirectoryA( filePath.c_str() );
            
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
                    PyObject *s = PyObject_Str(errvalue);
                    
                    dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "%s/n", PyString_AS_STRING( s ) );

                    Py_DECREF(s);
                }

                Py_XDECREF(errvalue);
                Py_XDECREF(errtype);
                Py_XDECREF(traceback);        
            }  
            
            SetCurrentDirectoryA( &oldCurDirCstr[0] );
        }
        else
        {
      		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "script file not found\n" );
        }           
    }
   
    catch(...)
    {           
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
    try {

        DbgExt      ext( client );

  
        if ( !std::string( args ).empty() )
        {
            try
            {
                boost::python::exec( args, WindbgGlobalSession::global(), WindbgGlobalSession::global() );
            }
            catch( boost::python::error_already_set const & )
            {
                // ошибка в скрипте
                PyObject    *errtype = NULL, *errvalue = NULL, *traceback = NULL;
                
                PyErr_Fetch( &errtype, &errvalue, &traceback );
                
                if(errvalue != NULL) 
                {
                    PyObject *s = PyObject_Str(errvalue);
                    
                    dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "%s\n", PyString_AS_STRING( s )  );
                    
                    Py_DECREF(s);
                }

                Py_XDECREF(errvalue);
                Py_XDECREF(errtype);
                Py_XDECREF(traceback);        
            }  
        }
        else
        {
            char        str[100];
            ULONG       inputSize;
            bool        stopInput = false;

            do {
            
                std::string     output;
                
                dbgExt->control->Output( DEBUG_OUTPUT_NORMAL, ">>>" );
                    
                do {    
                                
                    OutputReader        outputReader( dbgExt->client );                    
                    
                    HRESULT   hres = dbgExt->control->Input( str, sizeof(str), &inputSize );
                
                    if ( FAILED( hres ) || std::string( str ) == "" )
                    {
                       stopInput = true;
                       break;
                    }                       
                    
                } while( FALSE );                    
                
                if ( !stopInput )
                    try {
                        boost::python::exec( str, WindbgGlobalSession::global(), WindbgGlobalSession::global() );
                    }
                    catch( boost::python::error_already_set const & )
                    {
                        // ошибка в скрипте
                        PyObject    *errtype = NULL, *errvalue = NULL, *traceback = NULL;
                        
                        PyErr_Fetch( &errtype, &errvalue, &traceback );
                        
                        if(errvalue != NULL) 
                        {
                            PyObject *s = PyObject_Str(errvalue);
                            
                            dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "%s/n", PyString_AS_STRING( s ) );
                            
                            Py_DECREF(s);
                        }

                        Py_XDECREF(errvalue);
                        Py_XDECREF(errtype);
                        Py_XDECREF(traceback);        
                    }  
                    
            } while( !stopInput );                                
        }
    }
  
    catch(...)
    {           
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

