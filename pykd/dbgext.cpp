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
    boost::python::def( "go", &setExecutionStatus<DEBUG_STATUS_GO> );
    boost::python::def( "trace", &setExecutionStatus<DEBUG_STATUS_STEP_INTO> );
    boost::python::def( "step", &setExecutionStatus<DEBUG_STATUS_STEP_OVER> );   
    boost::python::def( "expr", &evaluate ); 
    boost::python::def( "isWindbgExt", &isWindbgExt );
    boost::python::def( "isSessionStart", &isWindbgExt );
    boost::python::def( "symbolsPath", &dbgSymPath );
    boost::python::def( "dprint", &DbgPrint::dprint, dprint( boost::python::args( "str", "dml" ), ""  ) );
    boost::python::def( "dprintln", &DbgPrint::dprintln, dprintln( boost::python::args( "str", "dml" ), ""  ) );
    boost::python::def( "loadDump", &dbgLoadDump );
    boost::python::def( "startProcess", &startProcess );
    boost::python::def( "dbgCommand", &dbgCommand );
    boost::python::def( "isValid", &isOffsetValid );
    boost::python::def( "is64bitSystem", &is64bitSystem );
    boost::python::def( "isKernelDebugging", &isKernelDebugging );
    boost::python::def( "ptrSize", ptrSize );
    boost::python::def( "reg", &loadRegister );
    boost::python::def( "typedVar", &loadTypedVar );
    boost::python::def( "typedVarList", &loadTypedVarList );
    boost::python::def( "typedVarArray", &loadTypedVarArray );
    boost::python::def( "containingRecord", &containingRecord );
    boost::python::def( "getTypeClass", &getTypeClass );
    boost::python::def( "sizeof", &sizeofType );
    boost::python::def( "loadModule", &loadModule );
    boost::python::def( "findSymbol", &findSymbolForAddress );
    boost::python::def( "getOffset", &findAddressForSymbol );
    boost::python::def( "findModule", &findModule );
    boost::python::def( "addr64", &addr64 );
    boost::python::def( "loadBytes", &loadArray<unsigned char>, loadBytes( boost::python::args( "address", "number",  "phyAddr"  ), "" ) );
    boost::python::def( "loadWords", &loadArray<unsigned short>, loadWords( boost::python::args( "address", "number",  "phyAddr"  ), "" ) );
    boost::python::def( "loadDWords", &loadArray<unsigned long>, loadDWords( boost::python::args( "address", "number",  "phyAddr"  ), "" ) );
    boost::python::def( "loadQWords", &loadArray<unsigned __int64>, loadQWords( boost::python::args( "address", "number",  "phyAddr"  ), "" ) );
    boost::python::def( "loadSignBytes", &loadArray<char>, loadSignBytes( boost::python::args( "address", "number",  "phyAddr"  ), "" ) );
    boost::python::def( "loadSignWords", &loadArray<short>, loadSignWords( boost::python::args( "address", "number",  "phyAddr"  ), "" ) );
    boost::python::def( "loadSignDWords", &loadArray<long>, loadSignDWords( boost::python::args( "address", "number",  "phyAddr"  ), "" ) );
    boost::python::def( "loadSignQWords", &loadArray<__int64>, loadSignQWords( boost::python::args( "address", "number",  "phyAddr"  ), "" ) );
    boost::python::def( "loadPtrs", &loadPtrArray );
    boost::python::def( "loadUnicodeString", &loadUnicodeStr );
    boost::python::def( "loadAnsiString", &loadAnsiStr );   
    boost::python::def( "loadCStr", &loadCStr );
    boost::python::def( "loadWStr", &loadWStr );
    boost::python::def( "loadLinkedList", &loadLinkedList ); 
    boost::python::def( "ptrByte", &loadByPtr<unsigned char> );
    boost::python::def( "ptrSignByte", &loadByPtr<char> );
    boost::python::def( "ptrWord", &loadByPtr<unsigned short> );
    boost::python::def( "ptrSignWord", &loadByPtr<short> );
    boost::python::def( "ptrDWord", &loadByPtr<unsigned long> );
    boost::python::def( "ptrSignDWord", &loadByPtr<long> );
    boost::python::def( "ptrQWord", &loadByPtr<unsigned __int64> );
    boost::python::def( "ptrSignQWord", &loadByPtr<__int64> );
    boost::python::def( "ptrPtr", &loadPtrByPtr );   
    boost::python::def( "ptrMWord", &loadMWord );
    boost::python::def( "ptrSignMWord", &loadSignMWord );
    boost::python::def( "compareMemory", &compareMemory, compareMemoryOver( boost::python::args( "addr1", "addr2", "length", "phyAddr" ), "" ) );
    boost::python::def( "getCurrentStack", &getCurrentStack );
    boost::python::def( "locals", &getLocals );
    boost::python::def( "reloadModule", &reloadModule );
    boost::python::def( "getPdbFile", &getPdbFile );
    boost::python::def( "getImplicitThread", &getImplicitThread );
    boost::python::def( "setImplicitThread", &setImplicitThread );
    boost::python::def( "getThreadList", &getThreadList );
    boost::python::def( "getCurrentProcess", &getCurrentProcess );
    boost::python::def( "setCurrentProcess", &setCurrentProcess );
    boost::python::def( "getProcessorMode", &getProcessorMode );
    boost::python::def( "setProcessorMode", &setProcessorMode );
    boost::python::def( "addSynSymbol", &addSyntheticSymbol );
    boost::python::def( "delAllSynSymbols", &delAllSyntheticSymbols);
    boost::python::def( "delSynSymbol", &delSyntheticSymbol );
    boost::python::def( "delSynSymbolsMask", &delSyntheticSymbolsMask);

    boost::python::class_<typeClass, boost::shared_ptr<typeClass> >( "typeClass" )
        .def("sizeof", &typeClass::size )
        .def("offset", &typeClass::getOffset )
        .def("__str__", &typeClass::print);
    boost::python::class_<typedVarClass, boost::python::bases<typeClass>, boost::shared_ptr<typedVarClass> >( "typedVarClass" )
        .def("getAddress", &typedVarClass::getAddress );
    boost::python::class_<dbgModuleClass>( "dbgModuleClass" )
        .def("begin", &dbgModuleClass::getBegin )
        .def("end", &dbgModuleClass::getEnd )
        .def("size", &dbgModuleClass::getSize )
        .def("name", &dbgModuleClass::getName )
        .def("contain", &dbgModuleClass::contain )
        .def("image", &dbgModuleClass::getImageSymbolName )
        .def("pdb", &dbgModuleClass::getPdbName )
        .def("checksum", &dbgModuleClass::getCheckSum )
        .def("timestamp", &dbgModuleClass::getTimeStamp )
        .def("addSynSymbol", &dbgModuleClass::addSyntheticSymbol )
        .def("delAllSynSymbols", &dbgModuleClass::delAllSyntheticSymbols )
        .def("delSynSymbol", &dbgModuleClass::delSyntheticSymbol )
        .def("delSynSymbolsMask", &dbgModuleClass::delSyntheticSymbolsMask )
        .def("__getattr__", &dbgModuleClass::getOffset )
        .def("__str__", &dbgModuleClass::print );
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

