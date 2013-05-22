#include "stdafx.h"

#include "kdlib/dbgio.h"

#include "dbgexcept.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

python::handle<> exceptPyType<python::detail::not_specified>::pyExceptType;
python::handle<> exceptPyType<kdlib::DbgException>::pyExceptType;
python::handle<> exceptPyType<kdlib::MemoryException>::pyExceptType;
python::handle<> exceptPyType<kdlib::SymbolException>::pyExceptType;
python::handle<> exceptPyType<kdlib::TypeException>::pyExceptType;

///////////////////////////////////////////////////////////////////////////////////

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

    kdlib::eprintln( sstr.str() );
}


///////////////////////////////////////////////////////////////////////////////////

}; // end namespace pykd
