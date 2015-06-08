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

inline
std::wstring getSrcPath()
{
    AutoRestorePyState  pystate;
    return kdlib::getSrcPath();
}

inline
void setSrcPath(const std::wstring &srcPath)
{
    AutoRestorePyState  pystate;
    kdlib::setSrcPath(srcPath);
}

inline
void appendSrcPath(const std::wstring &srcPath)
{
    AutoRestorePyState  pystate;
    kdlib::appendSrcPath(srcPath);
}

} // namespace pykd
