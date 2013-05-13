#include "stdafx.h"

#include "kdlib/kdlib.h"
#include "kdlib/windbg.h"

using namespace kdlib;
using namespace kdlib::windbg;

///////////////////////////////////////////////////////////////////////////////

extern "C" void initpykd();

class PykdExt : public WindbgExtension
{
public:
    KDLIB_EXT_COMMAND_METHOD(py);

private:

    void startConsole();

    virtual void setUp() 
    {
        WindbgExtension::setUp();

        PyImport_AppendInittab("pykd", initpykd ); 

        Py_Initialize();

        boost::python::import( "pykd" );

        // перенаправление стандартных потоков ВВ
        python::object       sys = python::import("sys");

        sys.attr("stdout") = python::ptr( dbgout  );
        sys.attr("stderr") = python::ptr( dbgout );
        sys.attr("stdin") = python::ptr( dbgin );
    }

    virtual void tearDown()
    {
        Py_Finalize();

        WindbgExtension::tearDown();
    }

};

KDLIB_WINDBG_EXTENSION_INIT( PykdExt );

///////////////////////////////////////////////////////////////////////////////

KDLIB_EXT_COMMAND_METHOD_IMPL(PykdExt, py)
{
    const ArgsList&   args = getArgs();

    if ( args.size() == 0 )
    {
        startConsole();
        return;
    }
}


///////////////////////////////////////////////////////////////////////////////

void printException()
{
    // ошибка в скрипте
    PyObject  *errtype = NULL, *errvalue = NULL, *traceback = NULL;

    PyErr_Fetch( &errtype, &errvalue, &traceback );

    PyErr_NormalizeException( &errtype, &errvalue, &traceback );

    if ( errtype == PyExc_SystemExit )
        return;

    python::object  tracebackModule = python::import("traceback");

    std::wstringstream  sstr;

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

///////////////////////////////////////////////////////////////////////////////

void PykdExt::startConsole()
{
    try {

        // получаем достпу к глобальному мапу ( нужен для вызова exec_file )
        python::object       main =  python::import("__main__");

        python::object       global(main.attr("__dict__"));

        python::exec(  "__import__('code').InteractiveConsole(__import__('__main__').__dict__).interact()\n", global );
    }
    catch( python::error_already_set const & )
    {
        printException();
    }
}

///////////////////////////////////////////////////////////////////////////////
