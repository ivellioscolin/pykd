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

    kdlib::DebugCallbackResult onBreakpoint( kdlib::BREAKPOINT_ID bpId ) override;
    kdlib::DebugCallbackResult onException( const kdlib::ExceptionInfo &exceptionInfo ) override;
    kdlib::DebugCallbackResult onModuleLoad( kdlib::MEMOFFSET_64 offset, const std::wstring &name ) override;
    kdlib::DebugCallbackResult onModuleUnload( kdlib::MEMOFFSET_64 offset, const std::wstring &name ) override;
    kdlib::DebugCallbackResult onThreadStart() override;
    kdlib::DebugCallbackResult onThreadStop() override;

    void onExecutionStatusChange(kdlib::ExecutionStatus executionStatus) override;
    void onCurrentThreadChange(kdlib::THREAD_DEBUG_ID  threadid) override;
    void onChangeLocalScope() override;
    void onChangeSymbolPaths() override;
    void onChangeBreakpoints() override;
    void onDebugOutput(const std::wstring& text, kdlib::OutputFlag) override;
    void onStartInput() override;
    void onStopInput() override;

private:

    PyThreadState*  m_pystate;
};

/////////////////////////////////////////////////////////////////////////////////


class Breakpoint : public python::wrapper<kdlib::BreakpointCallback>, public kdlib::BreakpointCallback
{

public:

    static Breakpoint* setSoftwareBreakpoint( kdlib::MEMOFFSET_64 offset, python::object  &callback = python::object() );

    static Breakpoint* setHardwareBreakpoint( kdlib::MEMOFFSET_64 offset, size_t size, kdlib::ACCESS_TYPE accessType, python::object  &callback= python::object() );

    static unsigned long getNumberBreakpoints();

    static Breakpoint* getBreakpointByIndex(unsigned long index);

    static void removeBreakpointByIndex(unsigned long index);

    static void removeAllBreakpoints();

public:

    explicit Breakpoint(kdlib::BreakpointPtr bp, bool weakbp = true);

    explicit Breakpoint(kdlib::MEMOFFSET_64 offset);

    Breakpoint(kdlib::MEMOFFSET_64 offset, python::object  &callback);

    Breakpoint(kdlib::MEMOFFSET_64 offset, size_t size, kdlib::ACCESS_TYPE accessType);

    Breakpoint(kdlib::MEMOFFSET_64 offset, size_t size, kdlib::ACCESS_TYPE accessType, python::object  &callback);

    ~Breakpoint();

    virtual kdlib::DebugCallbackResult onHit();

    virtual void onRemove()
    {
        m_breakpoint = 0;
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

    Breakpoint* detach();

private:

    PyThreadState*  m_pystate;

    kdlib::BreakpointPtr  m_breakpoint;

    python::object  m_callback;

    bool m_weakBp;
};


///////////////////////////////////////////////////////////////////////////////

} // end namespace pykd
