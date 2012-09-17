// 
// Breakpoints management
// 

#include "stdafx.h"
#include "bpoint.h"
#include "dbgengine.h"

///////////////////////////////////////////////////////////////////////////////

namespace pykd {

ULONG setSoftwareBp(ULONG64 offset, BpCallback &callback /*= BpCallback()*/)
{
    offset = addr64( offset );

    return breakPointSet( offset );
}

///////////////////////////////////////////////////////////////////////////////

ULONG setHardwareBp(ULONG64 offset, ULONG size, ULONG accessType, BpCallback &callback /*= BpCallback()*/ )
{
    offset = addr64( offset );

    return breakPointSet( offset, true, size, accessType );
}

///////////////////////////////////////////////////////////////////////////////

}   // end pykd namespace
