#pragma once

#include <boost/python/tuple.hpp>
namespace python = boost::python;

#include "kdlib/dbgengine.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

kdlib::ExecutionStatus targetGo();
void targetBreak();
kdlib::ExecutionStatus targetStep();
kdlib::ExecutionStatus targetStepIn();

python::tuple getSourceLine( kdlib::MEMOFFSET_64 offset = 0 );

///////////////////////////////////////////////////////////////////////////////

} //end namespace pykd