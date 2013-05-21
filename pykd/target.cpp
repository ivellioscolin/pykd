#include "stdafx.h"

#include "kdlib/dbgengine.h"

#include "target.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

void targetGo()
{
    PyThreadState*    state = PyEval_SaveThread();

    kdlib::targetGo();

    PyEval_RestoreThread( state );
}

///////////////////////////////////////////////////////////////////////////////

} //end namespace pykd
