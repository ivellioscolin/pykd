#include "stdafx.h"
#include "dbgexcept.h"
#include "diawrapper.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

PyObject   *ExceptionTranslator<DbgException>::exceptTypeObject = NULL;
PyObject   *ExceptionTranslator<MemoryException>::exceptTypeObject = NULL;
PyObject   *ExceptionTranslator<WaitEventException>::exceptTypeObject = NULL;
PyObject   *ExceptionTranslator<pyDia::Exception>::exceptTypeObject = NULL;
PyObject   *ExceptionTranslator<AddSyntheticSymbolException>::exceptTypeObject = NULL;

///////////////////////////////////////////////////////////////////////////////////

}; // end namespace pykd
