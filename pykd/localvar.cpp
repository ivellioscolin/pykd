
#include <stdafx.h>

#include "localvar.h"
#include "module.h"
#include "symengine.h"
#include "dbgengine.h"
#include "typedvar.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

python::dict getLocals()
{
    return getLocalsByFrame( getCurrentStackFrame() );
}

///////////////////////////////////////////////////////////////////////////////

python::dict getLocalsByFrame( StackFrame &frame )
{
    python::dict  dct;
    return dct;
}

///////////////////////////////////////////////////////////////////////////////

} //end pykd namespace

