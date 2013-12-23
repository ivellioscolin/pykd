#include "stdafx.h"


#include <boost/python.hpp>
namespace python = boost::python;

#include "kdlib/kdlib.h"
#include "kdlib/windbg.h"

#include "windbgext.h"
#include "dbgexcept.h"
#include "pydbgio.h"

using namespace kdlib;
using namespace kdlib::windbg;
using namespace pykd;

///////////////////////////////////////////////////////////////////////////////

KDLIB_WINDBG_EXTENSION_INIT( PykdExt );

bool PykdExt::isInit() {
    return WinDbgExt->isInit();
}

///////////////////////////////////////////////////////////////////////////////

extern "C" void initpykd();

pykd::DbgOut   pykdOut;
pykd::DbgOut   pykdErr;
pykd::DbgIn    pykdIn;


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

    // перенаправление стандартных потоков ВВ
    kdlib::dbgout =&pykdOut;
    kdlib::dbgerr = &pykdErr;
    kdlib::dbgin = &pykdIn;

    python::object       sys = python::import("sys");

    sys.attr("stdout") = python::object( &pykdOut );
    sys.attr("stderr") = python::object( &pykdErr );
    sys.attr("stdin") = python::object( &pykdIn );

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

class InterprterVirt
{
public:
    InterprterVirt()
    {
        PyThreadState   *threadState = PyGILState_GetThisThreadState();

        modules = threadState->interp->modules;
        modules_reloading = threadState->interp->modules_reloading;
        sysdict = threadState->interp->sysdict;
        builtins = threadState->interp->builtins;
        codec_search_path = threadState->interp->codec_search_path;
        codec_search_cache = threadState->interp->codec_search_cache;
        codec_error_registry = threadState->interp->codec_error_registry;

        newThread = NULL;
    }

    ~InterprterVirt()
    {
        PyThreadState   *threadState =PyGILState_GetThisThreadState();

        if ( threadState->interp->modules != modules )
        {
            Py_DECREF(threadState->interp->modules);
            threadState->interp->modules = modules;
        }

        if ( threadState->interp->modules_reloading != modules_reloading )
        {
            Py_DECREF(threadState->interp->modules_reloading);
            threadState->interp->modules_reloading = modules_reloading;
        }

        if ( threadState->interp->sysdict != sysdict )
        {
            Py_DECREF(threadState->interp->sysdict);
            threadState->interp->sysdict = sysdict;
        }

        if ( threadState->interp->builtins != builtins )
        {
            Py_DECREF(threadState->interp->builtins);
            threadState->interp->builtins = builtins;
        }

        if ( threadState->interp->codec_search_path != codec_search_path )
        {
            Py_DECREF(threadState->interp->codec_search_path);
            threadState->interp->codec_search_path = codec_search_path;
        }

        if ( threadState->interp->codec_search_cache != codec_search_cache )
        {
            Py_DECREF(threadState->interp->codec_search_cache);
            threadState->interp->codec_search_cache = codec_search_cache;
        }

        if ( threadState->interp->codec_error_registry != codec_error_registry )
        {
            Py_DECREF(threadState->interp->codec_error_registry);
            threadState->interp->codec_search_cache = codec_error_registry;
        }

        if (newThread)
        {
            PyThreadState*  current = PyThreadState_Get();
            PyThreadState_Swap(newThread);
            Py_EndInterpreter(newThread);
            PyThreadState_Swap(current);
        }

    }
    
    void fork()
    {
        PyThreadState  *threadState =PyGILState_GetThisThreadState();
        newThread = Py_NewInterpreter();

        threadState->interp->modules = newThread->interp->modules;
        Py_INCREF(threadState->interp->modules);

        threadState->interp->modules_reloading = newThread->interp->modules_reloading;
        Py_INCREF(threadState->interp->modules_reloading);

        threadState->interp->sysdict = newThread->interp->sysdict;
        Py_INCREF(threadState->interp->sysdict);

        threadState->interp->builtins = newThread->interp->builtins;
        Py_INCREF(threadState->interp->builtins);

        threadState->interp->codec_search_path = newThread->interp->codec_search_path;
        Py_INCREF(threadState->interp->codec_search_path);

        threadState->interp->codec_search_cache = newThread->interp->codec_search_cache;
        Py_INCREF(threadState->interp->codec_search_cache);

        threadState->interp->codec_error_registry = newThread->interp->codec_error_registry;
        Py_INCREF(threadState->interp->codec_error_registry);

        PyThreadState_Swap(threadState);
    }
   

private:

    PyObject *modules;
    PyObject *sysdict;
    PyObject *builtins;
    PyObject *modules_reloading;
    PyObject *codec_search_path;
    PyObject *codec_search_cache;
    PyObject *codec_error_registry;

    PyThreadState  *newThread;
};


KDLIB_EXT_COMMAND_METHOD_IMPL(PykdExt, py)
{
    ArgsList   args = getArgs();

    bool  global = false;
    bool  local = false;
    bool  clean = false;
 
    ArgsList::iterator  foundArg;

    foundArg = std::find( args.begin(), args.end(), "-h" );
    if ( foundArg !=  args.end() )
    {
        printUsage();
        return;
    }

    foundArg = std::find( args.begin(), args.end(), "--help" );
    if ( foundArg !=  args.end() )
    {
        printUsage();
        return;
    }

    foundArg = std::find( args.begin(), args.end(), "-g" );
    if ( foundArg != args.end() )
    {
        global = true;
        args.erase( foundArg );
    }

    foundArg = std::find( args.begin(), args.end(), "--global" );
    if ( foundArg != args.end() )
    {
        global = true;
        args.erase( foundArg );
    }

    foundArg = std::find( args.begin(), args.end(), "-l" );
    if ( foundArg != args.end() )
    {
        local = true;
        args.erase( foundArg );
    }

    foundArg = std::find( args.begin(), args.end(), "--local" );
    if ( foundArg != args.end() )
    {
        local = true;
        args.erase( foundArg );
    }

    if ( global && local )
    {
       eprintln( L"-g(--global) and -l(--local) cannot be set together" );
       return;
    }

    std::string  scriptFileName;
    if ( args.size() > 0 )
    {
       scriptFileName  = getScriptFileName( args[0] );

        if ( scriptFileName.empty() )
        {
            eprintln( L"script file not found" );
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

    do {

        InterprterVirt   interpretrVirt;

        if ( !global )
        {
            interpretrVirt.fork();

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
                printException();
            }
        }

    } while( false);

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
        printException();
    }
}

///////////////////////////////////////////////////////////////////////////////

void PykdExt::printUsage()
{
    dprintln( L"usage: !py [options] [file]" );
    dprintln( L"Options:" );
    dprintln( L"-g --global  : run code in the common namespace" );
    dprintln( L"-l --local   : run code in the isolate namespace" );
}

///////////////////////////////////////////////////////////////////////////////

std::string PykdExt::getScriptFileName( const std::string &scriptName )
{
    bool fileHasPyExt = false;

    if ( scriptName.length() > 3 )
        fileHasPyExt = scriptName.rfind(".py") == scriptName.length() - 3;
    
    std::string  fullFileName = scriptName;

    if (!fileHasPyExt)
        fullFileName += ".py";
    
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
                fullFileName = std::string(&fullFileNameCStr[0]);
                return fullFileName;
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

