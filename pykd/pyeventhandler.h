#pragma once

#include <map>

#include <boost/thread/recursive_mutex.hpp>

#include "kdlib/dbgengine.h"
#include "kdlib/eventhandler.h"

#include "boost/shared_ptr.hpp"
#include "boost/noncopyable.hpp"
#include "boost/python/object.hpp"
#include "boost/python/wrapper.hpp"

#include "pythreadstate.h"

namespace python = boost::python;

namespace pykd {

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
    virtual kdlib::DebugCallbackResult onModuleLoad( kdlib::MEMOFFSET_64 offset, const std::wstring &name );
    virtual kdlib::DebugCallbackResult onModuleUnload( kdlib::MEMOFFSET_64 offset, const std::wstring &name );

private:

    PyThreadState*  m_pystate;
};

///////////////////////////////////////////////////////////////////////////////

class Breakpoint
{
public:

    Breakpoint()
    {}

    Breakpoint(
        kdlib::BREAKPOINT_ID&  bpId,
        kdlib::PROCESS_DEBUG_ID&  targetId,
        python::object&  callback
        ) : m_bpId(bpId), m_processId(targetId), m_callback(callback)
    {}

    kdlib::BREAKPOINT_ID  getId() const {
        return m_bpId;
    }

    kdlib::PROCESS_DEBUG_ID getProcessId() const {
        return m_processId;
    }

    python::object& getCallback() {
        return m_callback;
    }

private:

    kdlib::BREAKPOINT_ID  m_bpId;
    kdlib::PROCESS_DEBUG_ID  m_processId;
    python::object  m_callback;
};

class InternalEventHandler: public kdlib::EventHandler
{
public:
    
    InternalEventHandler() {
        m_pystate = PyThreadState_Get();
    }

    ~InternalEventHandler();

    kdlib::BREAKPOINT_ID setSoftwareBreakpoint( kdlib::MEMOFFSET_64 offset, python::object &callback = python::object() );

    kdlib::BREAKPOINT_ID setHardwareBreakpoint( kdlib::MEMOFFSET_64 offset, size_t size = 0, kdlib::ACCESS_TYPE accessType = 0, python::object &callback = python::object() );

    void breakPointRemove(kdlib::BREAKPOINT_ID bpId);

    void breakPointRemoveAll();

private:

    virtual kdlib::DebugCallbackResult onBreakpoint( kdlib::BREAKPOINT_ID bpId );

    virtual kdlib::DebugCallbackResult onProcessExit( kdlib::PROCESS_DEBUG_ID processid, kdlib::ProcessExitReason  reason, unsigned long exitCode );

private:

    PyThreadState*  m_pystate;

    typedef std::map<kdlib::BREAKPOINT_ID, Breakpoint>  BreakpointMap;
    boost::recursive_mutex  m_breakPointLock;
    BreakpointMap  m_breakPointMap;

};

extern pykd::InternalEventHandler  *globalEventHandler;

/////////////////////////////////////////////////////////////////////////////////

inline kdlib::BREAKPOINT_ID setSoftwareBreakpoint( kdlib::MEMOFFSET_64 offset, python::object &callback = python::object() ) 
{
    return globalEventHandler->setSoftwareBreakpoint(offset, callback);
}

inline kdlib::BREAKPOINT_ID setHardwareBreakpoint( kdlib::MEMOFFSET_64 offset, size_t size = 0, kdlib::ACCESS_TYPE accessType = 0, python::object &callback = python::object() ) 
{
    return globalEventHandler->setHardwareBreakpoint( offset, size, accessType, callback);
}

inline void breakPointRemove( kdlib::BREAKPOINT_ID id )
{
    globalEventHandler->breakPointRemove(id);
}

inline void breakPointRemoveAll()
{
    globalEventHandler->breakPointRemoveAll();
}

///////////////////////////////////////////////////////////////////////////////

} // end namespace pykd
