
#include "stdafx.h"

//#include <boost/tokenizer.hpp>
//
//#include "windbg.h"
//#include "dbgclient.h"
//#include "dbgpath.h"

//using namespace pykd;

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

HRESULT
CALLBACK
DebugExtensionInitialize(
    OUT PULONG  Version,
    OUT PULONG  Flags )
{
    *Version = DEBUG_EXTENSION_VERSION( 1, 0 );
    *Flags = 0;

//    WindbgGlobalSession::StartWindbgSession();

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////

VOID
CALLBACK
DebugExtensionUninitialize()
{
//    WindbgGlobalSession::StopWindbgSession();
}

////////////////////////////////////////////////////////////////////////////////

HRESULT 
CALLBACK
py( PDEBUG_CLIENT4 client, PCSTR args )
{
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////

HRESULT 
CALLBACK
pycmd( PDEBUG_CLIENT4 client, PCSTR args )
{
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////









//////////////////////////////////////////////////////////////////////////////////
//
//extern "C" void initpykd();
//
//////////////////////////////////////////////////////////////////////////////////
//
//WindbgGlobalSession::WindbgGlobalSession() {
//
//    PyImport_AppendInittab("pykd", initpykd ); 
//
//    PyEval_InitThreads();
//
//    Py_Initialize();
//
//    main = boost::python::import("__main__");
//
//    python::object   main_namespace = main.attr("__dict__");
//
//    // делаем аналог from pykd import *
//    python::object   pykd = boost::python::import( "pykd" );
//
//    python::dict     pykd_namespace( pykd.attr("__dict__") ); 
//
//    python::list     iterkeys( pykd_namespace.iterkeys() );
//
//    for (int i = 0; i < boost::python::len(iterkeys); i++)
//    {
//        std::string     key = boost::python::extract<std::string>(iterkeys[i]);
//
//        main_namespace[ key ] = pykd_namespace[ key ];
//    }
//
//    pyState = PyEval_SaveThread();
//}
//
//
//volatile LONG            WindbgGlobalSession::sessionCount = 0;
//
//WindbgGlobalSession     *WindbgGlobalSession::windbgGlobalSession = NULL; 
//
///////////////////////////////////////////////////////////////////////////////////
//
//HRESULT
//CALLBACK
//DebugExtensionInitialize(
//    OUT PULONG  Version,
//    OUT PULONG  Flags )
//{
//    *Version = DEBUG_EXTENSION_VERSION( 1, 0 );
//    *Flags = 0;
//
//    WindbgGlobalSession::StartWindbgSession();
//
//    return S_OK;
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//VOID
//CALLBACK
//DebugExtensionUninitialize()
//{
//    WindbgGlobalSession::StopWindbgSession();
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//HRESULT 
//CALLBACK
//py( PDEBUG_CLIENT4 client, PCSTR args )
//{
//    DebugClientPtr      dbgClient = DebugClient::createDbgClient( client );
//    DebugClientPtr      oldClient = DebugClient::setDbgClientCurrent( dbgClient );
//
//    WindbgGlobalSession::RestorePyState();
//
//    PyThreadState   *globalInterpreter = PyThreadState_Swap( NULL );
//    PyThreadState   *localInterpreter = Py_NewInterpreter();
//
//    try {
//
//        // получаем достпу к глобальному мапу ( нужен для вызова exec_file )
//        python::object       main =  python::import("__main__");
//
//        python::object       global(main.attr("__dict__"));
//
//        // настраиваем ввод/вывод ( чтобы в скрипте можно было писать print )
//
//        python::object       sys = python::import("sys");
//
//        sys.attr("stdout") = python::object( dbgClient->dout() );
//        sys.attr("stderr") = python::object( dbgClient->dout() );
//        sys.attr("stdin") = python::object( dbgClient->din() );
//
//        // импортируем модуль обработки исключений ( нужен для вывода traceback а )
//        python::object       tracebackModule = python::import("traceback");
//
//        // разбор параметров
//        typedef  boost::escaped_list_separator<char>    char_separator_t;
//        typedef  boost::tokenizer< char_separator_t >   char_tokenizer_t;  
//
//        std::string                 argsStr( args );
//
//        char_tokenizer_t            token( argsStr , char_separator_t( "", " \t", "\"" ) );
//        std::vector<std::string>    argsList;
//
//        for ( char_tokenizer_t::iterator   it = token.begin(); it != token.end(); ++it )
//        {
//            if ( *it != "" )
//                argsList.push_back( *it );
//        }            
//            
//        if ( argsList.size() == 0 )
//            return S_OK;      
//            
//        char    **pythonArgs = new char* [ argsList.size() ];
//     
//        for ( size_t  i = 0; i < argsList.size(); ++i )
//            pythonArgs[i] = const_cast<char*>( argsList[i].c_str() );
//
//        PySys_SetArgv( (int)argsList.size(), pythonArgs );
//
//        delete[]  pythonArgs;
//
//       // найти путь к файлу
//        std::string     fullScriptName;
//        DbgPythonPath   dbgPythonPath;
//        
//        if ( !dbgPythonPath.getFullFileName( argsList[0], fullScriptName ) )
//        {
//            dbgClient->eprintln( L"script file not found" );
//        }
//        else
//        try {
//
//            python::object       result;
//
//            result =  python::exec_file( fullScriptName.c_str(), global, global );
//        }
//        catch( boost::python::error_already_set const & )
//        {
//            // ошибка в скрипте
//            PyObject    *errtype = NULL, *errvalue = NULL, *traceback = NULL;
//
//            PyErr_Fetch( &errtype, &errvalue, &traceback );
//
//            PyErr_NormalizeException( &errtype, &errvalue, &traceback );
//
//            std::wstringstream       sstr;
//
//            python::object   lst = 
//                python::object( tracebackModule.attr("format_exception" ) )( 
//                    python::handle<>( errtype ),
//                    python::handle<>( python::allow_null( errvalue ) ),
//                    python::handle<>( python::allow_null( traceback ) ) );
//
//            sstr << std::endl << std::endl;
//
//            for ( long i = 0; i < python::len(lst); ++i )
//                sstr << std::wstring( python::extract<std::wstring>(lst[i]) ) << std::endl;
//
//            dbgClient->eprintln( sstr.str() );
//        }
//
//    }
//    catch(...)
//    {
//        dbgClient->eprintln( L"unexpected error" );
//    }
//
//    Py_EndInterpreter( localInterpreter ); 
//    PyThreadState_Swap( globalInterpreter );
//
//    WindbgGlobalSession::SavePyState();
//
//    DebugClient::setDbgClientCurrent( oldClient );
//
//    return S_OK;
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//HRESULT 
//CALLBACK
//pycmd( PDEBUG_CLIENT4 client, PCSTR args )
//{
//    if ( g_dbgClient->client() != client )
//    {
//        DebugClientPtr      dbgClient = DebugClient::createDbgClient( client );
//        DebugClient::setDbgClientCurrent( dbgClient );
//    }
//
//    WindbgGlobalSession::RestorePyState();
//    
//    ULONG    mask = 0;
//    client->GetOutputMask( &mask );
//
//    try {
//
//        // перенаправление стандартных потоков ВВ
//        python::object       sys = python::import("sys");
//       
//        sys.attr("stdout") = python::object( DbgOut( client ) );
//        sys.attr("stderr") = python::object( DbgOut( client ) );
//        sys.attr("stdin") = python::object( DbgIn( client ) );
//
//        client->SetOutputMask( DEBUG_OUTPUT_NORMAL );
//
//        PyRun_String(
//            "__import__('code').InteractiveConsole(__import__('__main__').__dict__).interact()", 
//            Py_file_input,
//            WindbgGlobalSession::global().ptr(),
//            WindbgGlobalSession::global().ptr()
//            );
//
//        // выход из интерпретатора происходит через исключение raise SystemExit(code)
//        // которое потом может помешать исполнению callback ов
//        PyErr_Clear();
//    }
//    catch(...)
//    {      
//        //dbgClient->eprintln( L"unexpected error" );
//    }    
//
//    client->SetOutputMask( mask );
//
//    WindbgGlobalSession::SavePyState();
//
//    return S_OK;
//}
//
//////////////////////////////////////////////////////////////////////////////////
