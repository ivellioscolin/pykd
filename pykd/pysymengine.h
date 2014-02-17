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

inline
std::wstring getSymbolPath() 
{
	AutoRestorePyState  pystate;
	return kdlib::getSymbolPath();
}

inline
void setSymbolPath(const std::wstring &symPath)
{
	AutoRestorePyState  pystate;
	kdlib::setSymbolPath(symPath);
}

inline
void appendSymbolPath(const std::wstring &symPath)
{
	AutoRestorePyState  pystate;
	kdlib::appendSymbolPath(symPath);
}

} // namespace pykd
