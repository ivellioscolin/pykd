#include "stdafx.h"


#include <boost/python.hpp>
namespace python = boost::python;

#include "kdlib/kdlib.h"
#include "kdlib/windbg.h"

#include "windbgext.h"
#include "dbgexcept.h"
#include "pydbgio.h"

using namespace kdlib::windbg;

///////////////////////////////////////////////////////////////////////////////

KDLIB_WINDBG_EXTENSION_INIT( PykdExt );

bool PykdExt::isInit() {
    return WinDbgExt->isInit();
}

///////////////////////////////////////////////////////////////////////////////

extern "C" void initpykd();


void PykdExt::setUp() 
{
    WindbgExtension::setUp();

    PyImport_AppendInittab("pykd", initpykd ); 

    PyEval_InitThreads();

    Py_Initialize();

    python::object  main = boost::python::import("__main__");

    python::object  main_namespace = main.attr("__dict__");

    python::object  pykd = python::import( "pykd" );

   // делаем аналог from pykd import *
    python::dict     pykd_namespace( pykd.attr("__dict__") ); 

    python::list     iterkeys( pykd_namespace.iterkeys() );

    for (int i = 0; i < boost::python::len(iterkeys); i++)
    {
        std::string     key = boost::python::extract<std::string>(iterkeys[i]);

        main_namespace[ key ] = pykd_namespace[ key ];
    }

    python::object       sys = python::import("sys");

    sys.attr("stdout") = python::object( pykd::DbgOut() );
    sys.attr("stderr") = python::object( pykd::DbgOut() );
    sys.attr("stdin") = python::object( pykd::DbgIn() );

    python::list pathList(sys.attr("path"));

    python::ssize_t  n = python::len(pathList);

    for (python::ssize_t i = 0; i < n ; i++) 
        m_paths.push_back(boost::python::extract<std::string>(pathList[i]));

    m_pyState = PyEval_SaveThread();
}

///////////////////////////////////////////////////////////////////////////////

void PykdExt::tearDown()
{
    PyEval_RestoreThread( m_pyState );

    Py_Finalize();

    WindbgExtension::tearDown();
}

///////////////////////////////////////////////////////////////////////////////

KDLIB_EXT_COMMAND_METHOD_IMPL(PykdExt, py)
{
    ArgsList   args = getArgs();

    bool  global = false;
    bool  local = false;
    bool  clean = false;
 
    ArgsList::iterator  foundArg;

    if ( !args.empty() )
    {
        if ( args[0] ==  "-h" || args[0] == "--help" )
        {
            printUsage();
            return;
        }
        else
        if ( args[0] ==  "-g" || args[0] == "--global" )
        {
            global = true;
            args.erase( args.begin() );
        }
        else
        if ( args[0] ==  "-l" || args[0] == "--local" )
        {
            local = true;
            args.erase( args.begin() );
        }
    }

    std::string  scriptFileName;
    if ( args.size() > 0 )
    {
        scriptFileName  = getScriptFileName( args[0] );

        if ( scriptFileName.empty() )
        {
            kdlib::eprintln( L"script file not found" );
            return;
        }

        global = !(global || local ) ? false : global ; //set local by default
    }
    else
    {
        global = !(global || local ) ? true : global ; //set global by default
    }

    PyThreadState   *localState = NULL;
    PyThreadState   *globalState = NULL;

    PyEval_RestoreThread( m_pyState );

    if ( !global )
    {
        globalState =  PyThreadState_Swap(NULL);

        Py_NewInterpreter();

        localState = PyThreadState_Get();

        python::object       sys = python::import("sys");

        sys.attr("stdout") = python::object( pykd::DbgOut() );
        sys.attr("stderr") = python::object( pykd::DbgOut() );
        sys.attr("stdin") = python::object( pykd::DbgIn() );
    }

    if ( args.size() == 0 )
    {
        startConsole();
    }
    else
    {
        std::string  scriptFileName = getScriptFileName( args[0] );

        // устанавиливаем питоновские аргументы
        char  **pythonArgs = new char* [ args.size() ];

        pythonArgs[0] = const_cast<char*>(scriptFileName.c_str());

        for ( size_t  i = 1; i < args.size(); ++i )
            pythonArgs[i] = const_cast<char*>( args[i].c_str() );

        PySys_SetArgv( (int)args.size(), pythonArgs );

        delete[]  pythonArgs;

        // получаем достпу к глобальному мапу ( нужен для вызова exec_file )
        python::object  main =  python::import("__main__");

        python::object  global(main.attr("__dict__"));

        try {
            PykdInterruptWatch  interruptWatch;
            python::exec_file( scriptFileName.c_str(), global );
        }
        catch( python::error_already_set const & )
        {
            pykd::printException();
        }
    }

    if ( !global )
    {
        PyInterpreterState  *interpreter = localState->interp;

        while( interpreter->tstate_head != NULL )
        {
            PyThreadState   *threadState = (PyThreadState*)(interpreter->tstate_head);

            PyThreadState_Clear(threadState);

            PyThreadState_Swap( NULL );

            PyThreadState_Delete(threadState);
        }
    
        PyInterpreterState_Clear(interpreter);

        PyInterpreterState_Delete(interpreter);

        PyThreadState_Swap( globalState );
    }

    m_pyState = PyEval_SaveThread();

}

///////////////////////////////////////////////////////////////////////////////

void PykdExt::startConsole()
{

    // получаем достпу к глобальному мапу ( нужен для вызова exec_file )
    python::object       main =  python::import("__main__");

    python::object       global(main.attr("__dict__"));

    try {
        PykdInterruptWatch  interruptWatch;
        python::exec(  "__import__('code').InteractiveConsole(__import__('__main__').__dict__).interact()\n", global );
    }
    catch( python::error_already_set const & )
    {
        pykd::printException();
    }
}

///////////////////////////////////////////////////////////////////////////////

void PykdExt::printUsage()
{
    kdlib::dprintln( L"usage: !py [options] [file]" );
    kdlib::dprintln( L"Options:" );
    kdlib::dprintln( L"-g --global  : run code in the common namespace" );
    kdlib::dprintln( L"-l --local   : run code in the isolate namespace" );
}

///////////////////////////////////////////////////////////////////////////////

std::string PykdExt::getScriptFileName( const std::string &scriptName )
{
    std::string scriptFileName = findScript( scriptName );

    if ( scriptFileName.empty() )
    {
        std::string scriptNameLow;
        scriptNameLow.resize( scriptName.size() );
        std::transform(
            scriptName.begin(),
            scriptName.end(),
            scriptNameLow.begin(),
            ::tolower);
        if ( scriptNameLow.rfind(".py") != (scriptNameLow.length() - 3) )
            scriptFileName = findScript( scriptName + ".py" );
    }

    return scriptFileName;
}

///////////////////////////////////////////////////////////////////////////////

std::string PykdExt::findScript( const std::string &fullFileName )
{
    if ( GetFileAttributesA(fullFileName.c_str()) != INVALID_FILE_ATTRIBUTES )
        return fullFileName;

    std::vector<std::string>::const_iterator it = m_paths.begin();
    for ( ; it != m_paths.end(); ++it)
    {
        DWORD bufSize = SearchPathA(
            (*it).c_str(), 
            fullFileName.c_str(), 
            NULL, 
            0, 
            NULL, 
            NULL);          
                
        if (bufSize > 0)
        {
            bufSize += 1;
            std::vector<char> fullFileNameCStr(bufSize);
            char *partFileNameCStr = NULL;
            
            bufSize = SearchPathA(
                (*it).c_str(),
                fullFileName.c_str(),
                NULL,
                bufSize,
                &fullFileNameCStr[0],
                &partFileNameCStr);
            
            if (bufSize > 0)
            {
                return std::string(&fullFileNameCStr[0]);
            }
        }
    }

    return "";
}

///////////////////////////////////////////////////////////////////////////////

bool PykdInterruptWatch::onInterrupt()
{
    HANDLE  quitEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    PyGILState_STATE state = PyGILState_Ensure();
    Py_AddPendingCall(&quit, (void*)quitEvent);
    PyGILState_Release(state);
    WaitForSingleObject(quitEvent,INFINITE);
    CloseHandle(quitEvent);
    return true;
}

///////////////////////////////////////////////////////////////////////////////

int PykdInterruptWatch::quit(void *context)
{
    HANDLE   quitEvent = (HANDLE)context;
    kdlib::eprintln( L"User Interrupt: CTRL+BREAK");
    PyErr_SetString( PyExc_SystemExit, "CTRL+BREAK" );
    SetEvent(quitEvent);
    return -1;
}

///////////////////////////////////////////////////////////////////////////////

