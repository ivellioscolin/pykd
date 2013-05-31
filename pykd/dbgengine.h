#pragma once

#include <boost/python/tuple.hpp>
namespace python = boost::python;

#include "kdlib/dbgengine.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

void targetGo();

python::tuple getSourceLine( kdlib::MEMOFFSET_64 offset = 0 );

///////////////////////////////////////////////////////////////////////////////

} //end namespace pykd