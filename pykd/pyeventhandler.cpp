#include "stdafx.h"

#include "kdlib\eventhandler.h"

#include "pyeventhandler.h"
#include "dbgexcept.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

EventHandler::EventHandler()
{
    m_pystate = PyThreadState_Get();
}

///////////////////////////////////////////////////////////////////////////////

kdlib::DebugCallbackResult EventHandler::onBreakpoint( kdlib::BREAKPOINT_ID bpId )
{
    kdlib::DebugCallbackResult  result = kdlib::DebugCallbackNoChange;

    PyEval_RestoreThread( m_pystate );

    try {

        do {

            python::override pythonHandler = get_override( "onBreakpoint" );
            if ( !pythonHandler )
            {
                result = kdlib::EventHandler::onBreakpoint( bpId );
                break;
            }

            python::object  resObj = pythonHandler( bpId );

            if ( resObj.is_none() )
            {
                result = kdlib::DebugCallbackNoChange;
                break;
            }

            int retVal = python::extract<int>( resObj );

            if ( retVal >= kdlib::DebugCallbackMax )
            {
                result = kdlib::DebugCallbackBreak;
                break;
            }
                
            result = kdlib::DebugCallbackResult(retVal);

        } while( FALSE );

    }
    catch (const python::error_already_set &) 
    {
        printException();
        result =  kdlib::DebugCallbackBreak;
    }

    m_pystate = PyEval_SaveThread();

    return result;
}

///////////////////////////////////////////////////////////////////////////////

void EventHandler::onExecutionStatusChange( kdlib::ExecutionStatus executionStatus )
{
    PyEval_RestoreThread( m_pystate );

    try {

        python::override pythonHandler = get_override( "onExecutionStatusChange" );
        if ( pythonHandler )
        {
            pythonHandler( executionStatus );
        }
    }
    catch (const python::error_already_set &) 
    {
        printException();
    }

    m_pystate = PyEval_SaveThread();
}

///////////////////////////////////////////////////////////////////////////////

kdlib::DebugCallbackResult EventHandler::onException( const kdlib::ExceptionInfo &exceptionInfo )
{
    kdlib::DebugCallbackResult  result = kdlib::DebugCallbackNoChange;

    PyEval_RestoreThread( m_pystate );

    try {

        do {

            python::override pythonHandler = get_override( "onException" );
            if ( !pythonHandler )
            {
                result = kdlib::EventHandler::onException( exceptionInfo );
                break;
            }

            python::object  resObj = pythonHandler( exceptionInfo );

            if ( resObj.is_none() )
            {
                result = kdlib::DebugCallbackNoChange;
                break;
            }

            int retVal = python::extract<int>( resObj );

            if ( retVal >= kdlib::DebugCallbackMax )
            {
                result = kdlib::DebugCallbackBreak;
                break;
            }
                
            result = kdlib::DebugCallbackResult(retVal);

        } while( FALSE );

    }
    catch (const python::error_already_set &) 
    {
        printException();
        result =  kdlib::DebugCallbackBreak;
    }

    m_pystate = PyEval_SaveThread();

    return result;
}

///////////////////////////////////////////////////////////////////////////////

kdlib::DebugCallbackResult  EventHandler::onModuleLoad( kdlib::MEMOFFSET_64 offset, const std::wstring &name )
{
    kdlib::DebugCallbackResult  result = kdlib::DebugCallbackNoChange;

    PyEval_RestoreThread( m_pystate );

    try {

        do {

            python::override pythonHandler = get_override( "onModuleLoad" );
            if ( !pythonHandler )
            {
                result = kdlib::EventHandler::onModuleLoad( offset, name );
                break;
            }

            python::object  resObj = pythonHandler( offset, name );

            if ( resObj.is_none() )
            {
                result = kdlib::DebugCallbackNoChange;
                break;
            }

            int retVal = python::extract<int>( resObj );

            if ( retVal >= kdlib::DebugCallbackMax )
            {
                result = kdlib::DebugCallbackBreak;
                break;
            }
                
            result = kdlib::DebugCallbackResult(retVal);

        } while( FALSE );

    }
    catch (const python::error_already_set &) 
    {
        printException();
        result =  kdlib::DebugCallbackBreak;
    }

    m_pystate = PyEval_SaveThread();

    return result;
}

///////////////////////////////////////////////////////////////////////////////

kdlib::DebugCallbackResult  EventHandler::onModuleUnload( kdlib::MEMOFFSET_64 offset, const std::wstring &name )
{
    kdlib::DebugCallbackResult  result = kdlib::DebugCallbackNoChange;

    PyEval_RestoreThread( m_pystate );

    try {

        do {

            python::override pythonHandler = get_override( "onModuleUnload" );
            if ( !pythonHandler )
            {
                result = kdlib::EventHandler::onModuleUnload( offset, name );
                break;
            }

            python::object  resObj = pythonHandler( offset, name );

            if ( resObj.is_none() )
            {
                result = kdlib::DebugCallbackNoChange;
                break;
            }

            int retVal = python::extract<int>( resObj );

            if ( retVal >= kdlib::DebugCallbackMax )
            {
                result = kdlib::DebugCallbackBreak;
                break;
            }
                
            result = kdlib::DebugCallbackResult(retVal);

        } while( FALSE );

    }
    catch (const python::error_already_set &) 
    {
        printException();
        result =  kdlib::DebugCallbackBreak;
    }

    m_pystate = PyEval_SaveThread();

    return result;
}

///////////////////////////////////////////////////////////////////////////////

class CallbackBreakpoint : public kdlib::BaseBreakpoint
{

public:

    static kdlib::BREAKPOINT_ID setSoftwareBreakpoint( kdlib::MEMOFFSET_64 offset, python::object  &callback);
    static kdlib::BREAKPOINT_ID setHardwareBreakpoint( kdlib::MEMOFFSET_64 offset, size_t size, kdlib::ACCESS_TYPE accessType, python::object  &callback );
    static void removeBreakpoint( kdlib::BREAKPOINT_ID  id );

private:

    virtual kdlib::DebugCallbackResult  onHit();

    python::object  m_callback;
    PyThreadState*  m_pystate;

    typedef std::map<kdlib::BREAKPOINT_ID, CallbackBreakpoint*>  BreakpointMap;
    static boost::recursive_mutex  m_breakpointLock;
    static BreakpointMap  m_breakpointMap;
};

CallbackBreakpoint::BreakpointMap  CallbackBreakpoint::m_breakpointMap;
boost::recursive_mutex CallbackBreakpoint::m_breakpointLock;

///////////////////////////////////////////////////////////////////////////////

kdlib::BREAKPOINT_ID CallbackBreakpoint::setSoftwareBreakpoint( kdlib::MEMOFFSET_64 offset, python::object  &callback)
{
    boost::recursive_mutex::scoped_lock l(m_breakpointLock);

    CallbackBreakpoint  *bp = new CallbackBreakpoint();
    bp->m_pystate = PyThreadState_Get();
    bp->m_callback = callback;
    bp->set(offset);

    m_breakpointMap[bp->m_id] = bp;

    return bp->m_id;
}

///////////////////////////////////////////////////////////////////////////////

kdlib::BREAKPOINT_ID CallbackBreakpoint::setHardwareBreakpoint( kdlib::MEMOFFSET_64 offset, size_t size, kdlib::ACCESS_TYPE accessType, python::object  &callback  )
{
    boost::recursive_mutex::scoped_lock l(m_breakpointLock);

    CallbackBreakpoint  *bp = new CallbackBreakpoint();
    bp->m_pystate = PyThreadState_Get();
    bp->m_callback = callback;
    bp->set(offset,size,accessType);

    m_breakpointMap[bp->m_id] = bp;

    return bp->m_id;
}

///////////////////////////////////////////////////////////////////////////////

void CallbackBreakpoint::removeBreakpoint( kdlib::BREAKPOINT_ID  id )
{
    boost::recursive_mutex::scoped_lock l(m_breakpointLock);

    BreakpointMap::iterator  it = m_breakpointMap.find(id);
    if ( it != m_breakpointMap.end() )
        m_breakpointMap.erase(it);

    kdlib::breakPointRemove(id);
}

///////////////////////////////////////////////////////////////////////////////

kdlib::DebugCallbackResult  CallbackBreakpoint::onHit()
{
    kdlib::DebugCallbackResult  result = kdlib::DebugCallbackNoChange;

    PyEval_RestoreThread( m_pystate );

    try {

        do {

            if ( !m_callback )
            {
                result = kdlib::DebugCallbackBreak;
                break;
            }

            python::object  resObj = m_callback();

            if ( resObj.is_none() )
            {
                result = kdlib::DebugCallbackNoChange;
                break;
            }

            int retVal = python::extract<int>( resObj );

            if ( retVal >= kdlib::DebugCallbackMax )
            {
                result = kdlib::DebugCallbackBreak;
                break;
            }
                
            result = kdlib::DebugCallbackResult(retVal);

        } while( FALSE );

    }
    catch (const python::error_already_set &) 
    {
        printException();
        result =  kdlib::DebugCallbackBreak;
    }

    m_pystate = PyEval_SaveThread();

    return result;
}

///////////////////////////////////////////////////////////////////////////////

kdlib::BREAKPOINT_ID setSoftwareBreakpoint( kdlib::MEMOFFSET_64 offset, python::object  &callback)
{
    return CallbackBreakpoint::setSoftwareBreakpoint(offset,callback);
}

///////////////////////////////////////////////////////////////////////////////

kdlib::BREAKPOINT_ID setHardwareBreakpoint( kdlib::MEMOFFSET_64 offset, size_t size, kdlib::ACCESS_TYPE accessType,python::object  &callback) 
{
    return CallbackBreakpoint::setHardwareBreakpoint(offset, size, accessType,callback);
}

///////////////////////////////////////////////////////////////////////////////

void breakPointRemove( kdlib::BREAKPOINT_ID id )
{
    CallbackBreakpoint::removeBreakpoint(id);
}

///////////////////////////////////////////////////////////////////////////////


} // end namespace pykd
