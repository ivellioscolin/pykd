#include "stdafx.h"

#include "kdlib\eventhandler.h"

#include "pyeventhandler.h"
#include "dbgexcept.h"

namespace pykd {

pykd::InternalEventHandler  *globalEventHandler = NULL;

///////////////////////////////////////////////////////////////////////////////////

InternalEventHandler::~InternalEventHandler()
{
    breakPointRemoveAll();
}

///////////////////////////////////////////////////////////////////////////////////

kdlib::BREAKPOINT_ID InternalEventHandler::setSoftwareBreakpoint( kdlib::MEMOFFSET_64 offset, python::object &callback )
{
    kdlib::BREAKPOINT_ID  id;
    kdlib::PROCESS_DEBUG_ID  processId;

    boost::recursive_mutex::scoped_lock  l(m_breakPointLock);

    do {
        AutoRestorePyState  pystate;
        id = kdlib::softwareBreakPointSet(offset);
        processId = kdlib::getCurrentProcessId();
    } while(false);

    m_breakPointMap[id] = Breakpoint(id, processId, callback);

    return id;
}

///////////////////////////////////////////////////////////////////////////////////

kdlib::BREAKPOINT_ID InternalEventHandler::setHardwareBreakpoint( kdlib::MEMOFFSET_64 offset, size_t size, kdlib::ACCESS_TYPE accessType, python::object &callback )
{
    kdlib::BREAKPOINT_ID  id;
    kdlib::PROCESS_DEBUG_ID  processId;

    boost::recursive_mutex::scoped_lock  l(m_breakPointLock);

    do {
        AutoRestorePyState  pystate;
        id = kdlib::hardwareBreakPointSet(offset,size,accessType);
        processId = kdlib::getCurrentProcessId();
    } while(false);

    m_breakPointMap[id] = Breakpoint(id, processId, callback);

    return id;
}

///////////////////////////////////////////////////////////////////////////////////

void InternalEventHandler::breakPointRemove(kdlib::BREAKPOINT_ID bpId)
{
    boost::recursive_mutex::scoped_lock  l(m_breakPointLock);
    
    BreakpointMap::iterator  it = m_breakPointMap.find(bpId);

    if ( it != m_breakPointMap.end() )
    {
        m_breakPointMap.erase(it);

        try {
            AutoRestorePyState  pystate;
            kdlib::breakPointRemove(bpId);
        } catch(kdlib::DbgException&)
        {}
    }
}

///////////////////////////////////////////////////////////////////////////////////

void InternalEventHandler::breakPointRemoveAll()
{
    boost::recursive_mutex::scoped_lock  l(m_breakPointLock);

    BreakpointMap::iterator  it;

    for ( it = m_breakPointMap.begin(); it != m_breakPointMap.end(); ++it )
    {
        try {
            AutoRestorePyState  pystate;
            kdlib::breakPointRemove(it->first);
        } catch(kdlib::DbgException&)
        {}
    }

    m_breakPointMap.clear();
}

///////////////////////////////////////////////////////////////////////////////////

kdlib::DebugCallbackResult InternalEventHandler::onBreakpoint( kdlib::BREAKPOINT_ID bpId )
{
    kdlib::DebugCallbackResult  result = kdlib::DebugCallbackNoChange;

    boost::recursive_mutex::scoped_lock  l(m_breakPointLock);
    
    BreakpointMap::iterator  it = m_breakPointMap.find(bpId);
    if ( it != m_breakPointMap.end() )
    {
        PyEval_RestoreThread( m_pystate );

        do {

            python::object &callback = it->second.getCallback();

            if ( !callback )
            {
                result = kdlib::DebugCallbackBreak;
                break;
            }

            try {

                python::object  resObj = callback( bpId );

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

            }
            catch (const python::error_already_set &) 
            {
                printException();
                result =  kdlib::DebugCallbackBreak;
            }

        } while( false);

        m_pystate = PyEval_SaveThread();
    }
    else
    {
        result =  kdlib::DebugCallbackNoChange;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////

kdlib::DebugCallbackResult InternalEventHandler::onProcessExit( kdlib::PROCESS_DEBUG_ID processId, kdlib::ProcessExitReason  reason, unsigned long exitCode )
{

    boost::recursive_mutex::scoped_lock  l(m_breakPointLock);

    BreakpointMap::iterator  it = m_breakPointMap.begin(); 

    while( it != m_breakPointMap.end() )
    {
        if ( it->second.getProcessId() == processId )
        {
            kdlib::breakPointRemove(it->first);

            PyEval_RestoreThread( m_pystate );

            m_breakPointMap.erase(it);
            
            m_pystate = PyEval_SaveThread();

            it = m_breakPointMap.begin(); 
        }
        else
        {
            ++it;
        }
    }

    return kdlib::DebugCallbackNoChange;
}

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


} // end namespace pykd
