#include "stdafx.h"
#include "dbgexcept.h"
#include "dbgengine.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

python::handle<> exceptPyType<python::detail::not_specified>::pyExceptType;
python::handle<> exceptPyType<DbgException>::pyExceptType;
python::handle<> exceptPyType<MemoryException>::pyExceptType;
python::handle<> exceptPyType<WaitEventException>::pyExceptType;
python::handle<> exceptPyType<SymbolException>::pyExceptType;
//python::handle<> exceptPyType<pyDia::Exception>::pyExceptType;
python::handle<> exceptPyType<TypeException>::pyExceptType;
python::handle<> exceptPyType<AddSyntheticSymbolException>::pyExceptType;
python::handle<> exceptPyType<WrongEventTypeException>::pyExceptType;
python::handle<> exceptPyType<ImplementException>::pyExceptType;

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

    eprintln( sstr.str() );
}


///////////////////////////////////////////////////////////////////////////////////

}; // end namespace pykd
