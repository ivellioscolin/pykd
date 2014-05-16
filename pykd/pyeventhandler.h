#pragma once

#include <map>

#include <boost/thread/recursive_mutex.hpp>

#include "kdlib/dbgengine.h"
#include "kdlib/eventhandler.h"
#include "kdlib/breakpoint.h"

#include "boost/shared_ptr.hpp"
#include "boost/noncopyable.hpp"
#include "boost/python/object.hpp"
#include "boost/python/wrapper.hpp"

#include "pythreadstate.h"

namespace python = boost::python;

namespace pykd {

///////////////////////////////////////////////////////////////////////////////


class EventHandler : public python::wrapper<kdlib::EventHandler>, public kdlib::EventHandler
{
public:

    EventHandler();

    virtual kdlib::DebugCallbackResult onBreakpoint( kdlib::BREAKPOINT_ID bpId );
    virtual void onExecutionStatusChange( kdlib::ExecutionStatus executionStatus );
    virtual kdlib::DebugCallbackResult onException( const kdlib::ExceptionInfo &exceptionInfo );
    virtual kdlib::DebugCallbackResult onModuleLoad( kdlib::MEMOFFSET_64 offset, const std::wstring &name );
    virtual kdlib::DebugCallbackResult onModuleUnload( kdlib::MEMOFFSET_64 offset, const std::wstring &name );

private:

    PyThreadState*  m_pystate;
};

/////////////////////////////////////////////////////////////////////////////////

kdlib::BREAKPOINT_ID setSoftwareBreakpoint( kdlib::MEMOFFSET_64 offset, python::object  &callback = python::object() );
kdlib::BREAKPOINT_ID setHardwareBreakpoint( kdlib::MEMOFFSET_64 offset, size_t size, kdlib::ACCESS_TYPE accessType,python::object  &callback = python::object()) ;
void breakPointRemove( kdlib::BREAKPOINT_ID id );


class Breakpoint : public python::wrapper<kdlib::BaseBreakpoint>, public kdlib::BaseBreakpoint {

public:

    Breakpoint( kdlib::MEMOFFSET_64 offset );
    Breakpoint( kdlib::MEMOFFSET_64 offset, size_t size, kdlib::ACCESS_TYPE accessType );

    virtual kdlib::DebugCallbackResult onHit();

private:

    PyThreadState*  m_pystate;

};

///////////////////////////////////////////////////////////////////////////////

} // end namespace pykd
