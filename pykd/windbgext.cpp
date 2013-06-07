#include "stdafx.h"


#include <boost/python.hpp>
namespace python = boost::python;

#include "kdlib/kdlib.h"
#include "kdlib/windbg.h"

#include "windbgext.h"
#include "dbgexcept.h"

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

void PykdExt::setUp() 
{
    WindbgExtension::setUp();

    PyImport_AppendInittab("pykd", initpykd ); 

    Py_Initialize();

    python::import( "pykd" );

    // перенаправление стандартных потоков ВВ
    python::object       sys = python::import("sys");

    sys.attr("stdout") = python::ptr( dbgout  );
    sys.attr("stderr") = python::ptr( dbgout );
    sys.attr("stdin") = python::ptr( dbgin );


    python::list pathList(sys.attr("path"));

    python::ssize_t  n = python::len(pathList);

    for (python::ssize_t i = 0; i < n ; i++) 
        m_paths.push_back(boost::python::extract<std::string>(pathList[i]));
}

///////////////////////////////////////////////////////////////////////////////

void PykdExt::tearDown()
{
    Py_Finalize();

    WindbgExtension::tearDown();
}

///////////////////////////////////////////////////////////////////////////////

KDLIB_EXT_COMMAND_METHOD_IMPL(PykdExt, py)
{
    const ArgsList&   args = getArgs();

    if ( args.size() == 0 )
    {
        startConsole();
        return;
    }

    std::string  scriptFileName = getScriptFileName( args[0] );

    if ( scriptFileName.empty() )
    {
        eprintln( L"script file not found" );
        return;
    }

    // устанавиливаем питоновские аргументы
    char  **pythonArgs = new char* [ args.size() ];

    for ( size_t  i = 0; i < args.size(); ++i )
        pythonArgs[i] = const_cast<char*>( args[i].c_str() );

    PySys_SetArgv( (int)args.size(), pythonArgs );

    delete[]  pythonArgs;

    // получаем достпу к глобальному мапу ( нужен для вызова exec_file )
    python::object  main =  python::import("__main__");

    python::object  global(main.attr("__dict__"));

    try {
        python::exec_file( scriptFileName.c_str(), global );
    }
    catch( python::error_already_set const & )
    {
        printException();
    }
}

///////////////////////////////////////////////////////////////////////////////

void PykdExt::startConsole()
{

    // получаем достпу к глобальному мапу ( нужен для вызова exec_file )
    python::object       main =  python::import("__main__");

    python::object       global(main.attr("__dict__"));

    try {
        python::exec(  "__import__('code').InteractiveConsole(__import__('__main__').__dict__).interact()\n", global );
    }
    catch( python::error_already_set const & )
    {
        printException();
    }
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
