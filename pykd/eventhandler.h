#pragma once

#include "kdlib/dbgengine.h"

#include "boost/shared_ptr.hpp"
#include "boost/noncopyable.hpp"
#include "boost/python/object.hpp"
namespace python = boost::python;

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

class Breakpoint;
typedef boost::shared_ptr<Breakpoint>  BreakpointPtr;

class Breakpoint : public boost::noncopyable
{

public:

    static
    BreakpointPtr setSoftwareBreakpoint( kdlib::MEMOFFSET_64 offset, python::object &callback = python::object() );

protected:

    virtual ~Breakpoint() {}

};

///////////////////////////////////////////////////////////////////////////////

} // end namespace pykd
