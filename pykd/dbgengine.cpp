#include "stdafx.h"

#include "kdlib/dbgengine.h"
#include "kdlib/typeinfo.h"

#include "dbgengine.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

kdlib::ExecutionStatus targetGo()
{
    kdlib::ExecutionStatus  status;

    PyThreadState*    state = PyEval_SaveThread();

    status = kdlib::targetGo();

    PyEval_RestoreThread( state );

    return status;
}

///////////////////////////////////////////////////////////////////////////////

void targetBreak()
{
    PyThreadState*    state = PyEval_SaveThread();

    kdlib::targetBreak();

    PyEval_RestoreThread( state );
}

///////////////////////////////////////////////////////////////////////////////

kdlib::ExecutionStatus targetStep()
{
    kdlib::ExecutionStatus  status;

    PyThreadState*    state = PyEval_SaveThread();

    status =  kdlib::targetStep();

    PyEval_RestoreThread( state );

    return status;
}

///////////////////////////////////////////////////////////////////////////////

kdlib::ExecutionStatus targetStepIn()
{
    kdlib::ExecutionStatus  status;

    PyThreadState*    state = PyEval_SaveThread();

    status = kdlib::targetStepIn();

    PyEval_RestoreThread( state );

    return status;
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

python::tuple findSymbolAndDisp( ULONG64 offset )
{
    kdlib::MEMDISPLACEMENT  displacement = 0;
    std::wstring  symbolName = kdlib::findSymbol( offset, displacement );
    return python::make_tuple(symbolName,displacement);
}

///////////////////////////////////////////////////////////////////////////////

} //end namespace pykd
