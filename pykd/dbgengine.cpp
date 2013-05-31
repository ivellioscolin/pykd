#include "stdafx.h"

#include "kdlib/dbgengine.h"

#include "dbgengine.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

void targetGo()
{
    PyThreadState*    state = PyEval_SaveThread();

    kdlib::targetGo();

    PyEval_RestoreThread( state );
}

///////////////////////////////////////////////////////////////////////////////

python::tuple getSourceLine( kdlib::MEMOFFSET_64 offset )
{
    std::wstring  fileName;
    unsigned long  lineno;
    long  displacement;

    kdlib::getSourceLine( fileName, lineno, displacement, offset );

    return python::make_tuple( fileName, lineno, displacement );
}

///////////////////////////////////////////////////////////////////////////////

} //end namespace pykd
