#pragma once

#include "kdlib/dbgengine.h"
#include "kdlib/eventhandler.h"

#include "boost/shared_ptr.hpp"
#include "boost/noncopyable.hpp"
#include "boost/python/object.hpp"
#include "boost/python/wrapper.hpp"

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

class EventHandler;
typedef boost::shared_ptr<EventHandler>  EventHandlerPtr;

class EventHandler : public python::wrapper<kdlib::EventHandler>, public kdlib::EventHandler
{
public:

    EventHandler();

    virtual kdlib::DebugCallbackResult onBreakpoint( kdlib::BREAKPOINT_ID bpId );
    virtual void onExecutionStatusChange( kdlib::ExecutionStatus executionStatus );
    virtual kdlib::DebugCallbackResult onException( const kdlib::ExceptionInfo &exceptionInfo );

private:

    PyThreadState*  m_pystate;
};

///////////////////////////////////////////////////////////////////////////////

} // end namespace pykd
