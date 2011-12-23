#pragma once

#include "intbase.h"
#include "dbgobj.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

python::object getRegByName( const std::wstring &regName );

python::object getRegByIndex( ULONG index );

ULONG64 loadMSR( ULONG  msr );

void setMSR( ULONG msr, ULONG64 value);

///////////////////////////////////////////////////////////////////////////////////

}; // end pykd namespace

