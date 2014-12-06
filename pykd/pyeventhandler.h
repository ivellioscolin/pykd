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
    virtual void onCurrentThreadChange(kdlib::THREAD_DEBUG_ID  threadid);

private:

    PyThreadState*  m_pystate;
};

/////////////////////////////////////////////////////////////////////////////////


class Breakpoint : public python::wrapper<kdlib::BreakpointCallback>, public kdlib::BreakpointCallback
{

public:

    static Breakpoint* setSoftwareBreakpoint( kdlib::MEMOFFSET_64 offset, python::object  &callback = python::object() );

    static Breakpoint* setHardwareBreakpoint( kdlib::MEMOFFSET_64 offset, size_t size, kdlib::ACCESS_TYPE accessType, python::object  &callback= python::object() );

    static unsigned long getNumberBreakpoints() {
        AutoRestorePyState  pystate;
        return kdlib::getNumberBreakpoints();
    }

    static Breakpoint* getBreakpointByIndex(unsigned long index);

public:

    explicit Breakpoint(kdlib::BreakpointPtr bp);

    explicit Breakpoint(kdlib::MEMOFFSET_64 offset);

    Breakpoint(kdlib::MEMOFFSET_64 offset, python::object  &callback);

    Breakpoint(kdlib::MEMOFFSET_64 offset, size_t size, kdlib::ACCESS_TYPE accessType);

    Breakpoint(kdlib::MEMOFFSET_64 offset, size_t size, kdlib::ACCESS_TYPE accessType, python::object  &callback);

    ~Breakpoint() 
    {}

    virtual kdlib::DebugCallbackResult onHit();

    virtual void onRemove() {
        delete this;
    }

    kdlib::BREAKPOINT_ID getId() const
    {
        AutoRestorePyState  pystate;
        return m_breakpoint->getId();
    }


    kdlib::MEMOFFSET_64 getOffset() const
    {
        AutoRestorePyState  pystate;
        return m_breakpoint->getOffset();
    }

    void remove();

private:

    PyThreadState*  m_pystate;

    kdlib::BreakpointPtr  m_breakpoint;

    python::object  m_callback;
};









//
//class BreakpointBase 
//{
//public:
//    kdlib::DebugCallbackResult onHit() {
//        return kdlib::DebugCallbackBreak;
//    }
//};
//
//typedef kdlib::AutoBreakpoint<BreakpointBase>  Breakpoint;


//class BaseBreakpoint {
//public:
//    virtual kdlib::DebugCallbackResult onHit() = 0;
//};


//class Breakpoint 
//{
//public:
//
//    Breakpoint( kdlib::MEMOFFSET_64 offset ) {
//    }
//
//    //static kdlib::BreakpointPtr createSoftwareBreakpoint( kdlib::MEMOFFSET_64 offset ) {
//    //    return kdlib::BreakpointPtr( new kdlib::AutoBreakpoint<Breakpoint>(offset) );
//    //}
//
//    virtual kdlib::DebugCallbackResult onHit();
//
//    static kdlib::BREAKPOINT_ID getId(kdlib::BreakpointPtr& bp) {
//        AutoRestorePyState  pystate;
//        return bp->getId();
//    }
//
//    static kdlib::MEMOFFSET_64 getOffset(kdlib::BreakpointPtr& bp) {
//        AutoRestorePyState  pystate;
//        return bp->getOffset();
//    }
//
//    static void remove(kdlib::BreakpointPtr& bp) {
//        AutoRestorePyState  pystate;
//        return bp->remove();
//    }
//
//    Breakpoint() {
//        m_pystate = PyThreadState_Get();
//    }
//
//private:
//
//    PyThreadState*  m_pystate;
//};



///////////////////////////////////////////////////////////////////////////////

} // end namespace pykd
