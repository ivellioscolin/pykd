#pragma once

#include <boost/python/tuple.hpp>
namespace python = boost::python;

#include "kdlib/dbgengine.h"
#include "kdlib/cpucontext.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

class CPUContextAdaptor
{
public:
    static python::object getRegisterByName( kdlib::CPUContext& cpu, const std::wstring &name );
    static python::object getRegisterByIndex( kdlib::CPUContext& cpu, size_t index );
};

///////////////////////////////////////////////////////////////////////////////

} //end namespace pykd
