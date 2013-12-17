#pragma once

#include <kdlib/symengine.h>

#include "pythreadstate.h"

namespace pykd {

inline
void setSymSrvDir(const std::wstring &symSrvDirectory) 
{
	AutoRestorePyState  pystate;
	kdlib::setSymSrvDir(symSrvDirectory);
}


} // namespace pykd
