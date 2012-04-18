#include "stdafx.h"
#include "dbgexcept.h"
#include "diawrapper.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

python::handle<> exceptPyType<python::detail::not_specified>::pyExceptType;
python::handle<> exceptPyType<DbgException>::pyExceptType;
python::handle<> exceptPyType<MemoryException>::pyExceptType;
python::handle<> exceptPyType<WaitEventException>::pyExceptType;
python::handle<> exceptPyType<SymbolException>::pyExceptType;
python::handle<> exceptPyType<pyDia::Exception>::pyExceptType;
python::handle<> exceptPyType<TypeException>::pyExceptType;
python::handle<> exceptPyType<AddSyntheticSymbolException>::pyExceptType;
python::handle<> exceptPyType<ImplementException>::pyExceptType;

///////////////////////////////////////////////////////////////////////////////////
}; // end namespace pykd
