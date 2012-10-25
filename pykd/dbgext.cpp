
#include "stdafx.h"

#include <dbgeng.h>

#include "win/dbgio.h"
#include "win/dbgeng.h"
#include "win/dbgpath.h"
#include "win/windbg.h"

using namespace pykd;

////////////////////////////////////////////////////////////////////////////////

extern "C" void initpykd();

////////////////////////////////////////////////////////////////////////////////

WindbgGlobalSession::WindbgGlobalSession()
{
    PyImport_AppendInittab("pykd", initpykd ); 

    PyEval_InitThreads();

    Py_Initialize();

    main = boost::python::import("__main__");

    python::object   main_namespace = main.attr("__dict__");

    python::object   pykd = boost::python::import( "pykd" );

    main_namespace["globalEventHandler"] = EventHandlerPtr( new EventHandlerImpl() );

   // делаем аналог from pykd import *
    python::dict     pykd_namespace( pykd.attr("__dict__") ); 

    python::list     iterkeys( pykd_namespace.iterkeys() );

    for (int i = 0; i < boost::python::len(iterkeys); i++)
    {
        std::string     key = boost::python::extract<std::string>(iterkeys[i]);

        main_namespace[ key ] = pykd_namespace[ key ];
    }

    // перенаправление стандартных потоков ВВ
    python::object       sys = python::import("sys");

    sys.attr("stdout") = python::object( DbgOut() );
    sys.attr("stderr") = python::object( DbgErr() );
    sys.attr("stdin") = python::object( DbgIn() );

    pyState = PyEval_SaveThread();
}
volatile LONG            WindbgGlobalSession::sessionCount = 0;

WindbgGlobalSession     *WindbgGlobalSession::windbgGlobalSession = NULL; 

////////////////////////////////////////////////////////////////////////////////

class InterruptWatch
{
public:
    InterruptWatch( PDEBUG_CLIENT4 client, python::object& global ) 
    {
        m_debugControl = client;

        m_global = global;

        m_stopEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

        m_thread = 
            CreateThread(
                NULL,
                0,
                threadRoutine,
                this,
                0,
                NULL );
    }

    ~InterruptWatch()
    {
        SetEvent( m_stopEvent );

        WaitForSingleObject( m_thread, INFINITE );

        CloseHandle( m_stopEvent );

        CloseHandle( m_thread );
    }

private:


    static int quit(void *) {
        PyErr_SetString( PyExc_SystemExit, "" );
        return -1;
    }

    DWORD workRoutine(){

        while( WAIT_TIMEOUT == WaitForSingleObject( m_stopEvent, 250 ) )
        {
            HRESULT  hres =  m_debugControl->GetInterrupt();
            if ( hres == S_OK )
            {
                PyGILState_STATE state = PyGILState_Ensure();
                Py_AddPendingCall(&quit, NULL);
                PyGILState_Release(state);

                break;
            }
        }

        return 0;
    }

    static DWORD WINAPI threadRoutine(LPVOID lpParameter) {
        return  static_cast<InterruptWatch*>(lpParameter)->workRoutine();
    }

    HANDLE  m_thread;

    HANDLE  m_stopEvent;

    python::object m_global;

    CComQIPtr<IDebugControl> m_debugControl;
};

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
    WindbgGlobalSession::RestorePyState();

    PyThreadState   *globalInterpreter = PyThreadState_Swap( NULL );
    PyThreadState   *localInterpreter = Py_NewInterpreter();

    try {

        // получаем достпу к глобальному мапу ( нужен для вызова exec_file )
        python::object       main =  python::import("__main__");

        python::object       global(main.attr("__dict__"));

        InterruptWatch   interruptWatch( client, global );

        // настраиваем ввод/вывод ( чтобы в скрипте можно было писать print )

        python::object       sys = python::import("sys");

        sys.attr("stdout") = python::object( DbgOut() );
        sys.attr("stderr") = python::object( DbgErr() );
        sys.attr("stdin") = python::object( DbgIn() );

        python::object   pykd = python::import( "pykd" );

        global["globalEventHandler"] = EventHandlerPtr( new EventHandlerImpl() );

        // импортируем модуль обработки исключений ( нужен для вывода traceback а )
        python::object       tracebackModule = python::import("traceback");

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
            eprintln( L"script file not found" );
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

            std::wstringstream       sstr;

            python::object   lst = 
                python::object( tracebackModule.attr("format_exception" ) )( 
                    python::handle<>( errtype ),
                    python::handle<>( python::allow_null( errvalue ) ),
                    python::handle<>( python::allow_null( traceback ) ) );

            sstr << std::endl << std::endl;

            for ( long i = 0; i < python::len(lst); ++i )
                sstr << std::wstring( python::extract<std::wstring>(lst[i]) ) << std::endl;

            eprintln( sstr.str() );
        }
    }
    catch(...)
    {
        eprintln( L"unexpected error" );
    }

    Py_EndInterpreter( localInterpreter ); 
    PyThreadState_Swap( globalInterpreter );

    WindbgGlobalSession::SavePyState();

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////

HRESULT 
CALLBACK
pycmd( PDEBUG_CLIENT4 client, PCSTR args )
{
    WindbgGlobalSession::RestorePyState();

    ULONG    mask = 0;
    client->GetOutputMask( &mask );
    client->SetOutputMask( mask & ~DEBUG_OUTPUT_PROMPT ); // убрать эхо ввода

    InterruptWatch   interruptWatch( client, WindbgGlobalSession::global() );

    try {

        python::exec( 
            "try:\n"
            "  __import__('code').InteractiveConsole(__import__('__main__').__dict__).interact()\n"
            "except SystemExit:\n"
            "  print 'Ctrl+Break'\n",
            WindbgGlobalSession::global(),
            WindbgGlobalSession::global()
            );
    }
    catch(...)
    {
        eprintln( L"unexpected error" );
    }

    // выход из интерпретатора происходит через исключение raise SystemExit(code)
    // которое потом может помешать исполнению callback ов
    PyErr_Clear();

    client->SetOutputMask( mask );

    WindbgGlobalSession::SavePyState();

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
