// 
// Breakpoints management
// 

#pragma once

////////////////////////////////////////////////////////////////////////////////

#include "pyaux.h"

namespace pykd {

ULONG setSoftwareBp(ULONG64 offset, python::object &callback = python::object() );

ULONG setHardwareBp(ULONG64 offset, ULONG size, ULONG accessType, python::object &callback = python::object() );

void removeBp( ULONG id );

void removeAllBp();


///////////////////////////////////////////////////////////////////////////////

}   // end pykd namespace


