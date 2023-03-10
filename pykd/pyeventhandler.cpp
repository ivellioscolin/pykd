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

            if (resObj.is_none())
            {
                result = kdlib::DebugCallbackProceed;
                break;
            }

            if (PyBool_Check(resObj.ptr()))
            {
                result = python::extract<bool>(resObj) ? kdlib::DebugCallbackBreak : kdlib::DebugCallbackProceed;
                break;
            }

            python::extract<int>  resInt(resObj);
            if (resInt.check())
            {
                result = resInt < kdlib::DebugCallbackMax ? kdlib::DebugCallbackResult(resInt()) : kdlib::DebugCallbackBreak;
                break;
            }

            result = kdlib::DebugCallbackBreak;


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

            if (resObj.is_none())
            {
                result = kdlib::DebugCallbackProceed;
                break;
            }

            if (PyBool_Check(resObj.ptr()))
            {
                result = python::extract<bool>(resObj) ? kdlib::DebugCallbackBreak : kdlib::DebugCallbackProceed;
                break;
            }

            python::extract<int>  resInt(resObj);
            if (resInt.check())
            {
                result = resInt < kdlib::DebugCallbackMax ? kdlib::DebugCallbackResult(resInt()) : kdlib::DebugCallbackBreak;
                break;
            }

            result = kdlib::DebugCallbackBreak;


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

            python::override pythonHandler = get_override( "onLoadModule" );
            if ( !pythonHandler )
            {
                result = kdlib::EventHandler::onModuleLoad( offset, name );
                break;
            }

            python::object  resObj = pythonHandler( offset, name );

            if (resObj.is_none())
            {
                result = kdlib::DebugCallbackProceed;
                break;
            }

            if (PyBool_Check(resObj.ptr()))
            {
                result = python::extract<bool>(resObj) ? kdlib::DebugCallbackBreak : kdlib::DebugCallbackProceed;
                break;
            }

            python::extract<int>  resInt(resObj);
            if (resInt.check())
            {
                result = resInt < kdlib::DebugCallbackMax ? kdlib::DebugCallbackResult(resInt()) : kdlib::DebugCallbackBreak;
                break;
            }

            result = kdlib::DebugCallbackBreak;


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

            python::override pythonHandler = get_override( "onUnloadModule" );
            if ( !pythonHandler )
            {
                result = kdlib::EventHandler::onModuleUnload( offset, name );
                break;
            }

            python::object  resObj = pythonHandler( offset, name );

            if (resObj.is_none())
            {
                result = kdlib::DebugCallbackProceed;
                break;
            }

            if (PyBool_Check(resObj.ptr()))
            {
                result = python::extract<bool>(resObj) ? kdlib::DebugCallbackBreak : kdlib::DebugCallbackProceed;
                break;
            }

            python::extract<int>  resInt(resObj);
            if (resInt.check())
            {
                result = resInt < kdlib::DebugCallbackMax ? kdlib::DebugCallbackResult(resInt()) : kdlib::DebugCallbackBreak;
                break;
            }

            result = kdlib::DebugCallbackBreak;


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

/////////////////////////////////////////////////////////////////////////////////

kdlib::DebugCallbackResult EventHandler::onThreadStart()
{
    kdlib::DebugCallbackResult  result = kdlib::DebugCallbackNoChange;

    PyEval_RestoreThread(m_pystate);

    try {

        do {

            python::override pythonHandler = get_override("onThreadStart");
            if (!pythonHandler)
            {
                result = kdlib::EventHandler::onThreadStart();
                break;
            }

            python::object  resObj = pythonHandler();

            if (resObj.is_none())
            {
                result = kdlib::DebugCallbackProceed;
                break;
            }

            if (PyBool_Check(resObj.ptr()))
            {
                result = python::extract<bool>(resObj) ? kdlib::DebugCallbackBreak : kdlib::DebugCallbackProceed;
                break;
            }

            python::extract<int>  resInt(resObj);
            if (resInt.check())
            {
                result = resInt < kdlib::DebugCallbackMax ? kdlib::DebugCallbackResult(resInt()) : kdlib::DebugCallbackBreak;
                break;
            }

            result = kdlib::DebugCallbackBreak;


        } while (FALSE);

    }
    catch (const python::error_already_set &)
    {
        printException();
        result = kdlib::DebugCallbackBreak;
    }

    m_pystate = PyEval_SaveThread();

    return result;
}

/////////////////////////////////////////////////////////////////////////////////

kdlib::DebugCallbackResult EventHandler::onThreadStop()
{
    kdlib::DebugCallbackResult  result = kdlib::DebugCallbackNoChange;

    PyEval_RestoreThread(m_pystate);

    try {

        do {

            python::override pythonHandler = get_override("onThreadStop");
            if (!pythonHandler)
            {
                result = kdlib::EventHandler::onThreadStop();
                break;
            }

            python::object  resObj = pythonHandler();

            if (resObj.is_none())
            {
                result = kdlib::DebugCallbackProceed;
                break;
            }

            if (PyBool_Check(resObj.ptr()))
            {
                result = python::extract<bool>(resObj) ? kdlib::DebugCallbackBreak : kdlib::DebugCallbackProceed;
                break;
            }

            python::extract<int>  resInt(resObj);
            if (resInt.check())
            {
                result = resInt < kdlib::DebugCallbackMax ? kdlib::DebugCallbackResult(resInt()) : kdlib::DebugCallbackBreak;
                break;
            }

            result = kdlib::DebugCallbackBreak;

        } while (FALSE);

    }
    catch (const python::error_already_set &)
    {
        printException();
        result = kdlib::DebugCallbackBreak;
    }

    m_pystate = PyEval_SaveThread();

    return result;
}

/////////////////////////////////////////////////////////////////////////////////

void EventHandler::onCurrentThreadChange(kdlib::THREAD_DEBUG_ID  threadid)
{
    PyEval_RestoreThread( m_pystate );

    try {

        python::override pythonHandler = get_override("onCurrentThreadChange");
        if ( pythonHandler )
        {
            pythonHandler(threadid);
        }
    }
    catch (const python::error_already_set &) 
    {
        printException();
    }

    m_pystate = PyEval_SaveThread();
}

/////////////////////////////////////////////////////////////////////////////////

void EventHandler::onChangeLocalScope()
{
    PyEval_RestoreThread( m_pystate );

    try {

        python::override pythonHandler = get_override("onChangeLocalScope");
        if ( pythonHandler )
        {
            pythonHandler();
        }
    }
    catch (const python::error_already_set &) 
    {
        printException();
    }

    m_pystate = PyEval_SaveThread();
}


/////////////////////////////////////////////////////////////////////////////////

void EventHandler::onChangeSymbolPaths()
{
    PyEval_RestoreThread( m_pystate );

    try {

        python::override pythonHandler = get_override("onChangeSymbolPaths");
        if ( pythonHandler )
        {
            pythonHandler();
        }
    }
    catch (const python::error_already_set &) 
    {
        printException();
    }

    m_pystate = PyEval_SaveThread();
}

/////////////////////////////////////////////////////////////////////////////////

void EventHandler::onChangeBreakpoints()
{
    PyEval_RestoreThread(m_pystate);

    try {

        python::override pythonHandler = get_override("onChangeBreakpoints");
        if (pythonHandler)
        {
            pythonHandler();
        }
    }
    catch (const python::error_already_set &)
    {
        printException();
    }

    m_pystate = PyEval_SaveThread();
}

/////////////////////////////////////////////////////////////////////////////////

void EventHandler::onDebugOutput(const std::wstring& text, kdlib::OutputFlag flag)
{
    PyEval_RestoreThread( m_pystate );

    try {

        python::override pythonHandler = get_override("onDebugOutput");
        if ( pythonHandler )
        {
            pythonHandler(text, flag);
        }
    }
    catch (const python::error_already_set &) 
    {
        printException();
    }

    m_pystate = PyEval_SaveThread();
}

/////////////////////////////////////////////////////////////////////////////////

void EventHandler::onStartInput()
{
    PyEval_RestoreThread(m_pystate);

    try {

        python::override pythonHandler = get_override("onStartInput");
        if (pythonHandler)
        {
            pythonHandler();
        }
    }
    catch (const python::error_already_set &)
    {
        printException();
    }

    m_pystate = PyEval_SaveThread();
}

/////////////////////////////////////////////////////////////////////////////////

void EventHandler::onStopInput()
{
    PyEval_RestoreThread(m_pystate);

    try {

        python::override pythonHandler = get_override("onStopInput");
        if (pythonHandler)
        {
            pythonHandler();
        }
    }
    catch (const python::error_already_set &)
    {
        printException();
    }

    m_pystate = PyEval_SaveThread();
}

/////////////////////////////////////////////////////////////////////////////////

Breakpoint::Breakpoint(kdlib::BreakpointPtr bp, bool weakbp)
{
    m_breakpoint = bp;
    m_weakBp = weakbp;
}

Breakpoint::Breakpoint(kdlib::MEMOFFSET_64 offset)
{
    AutoRestorePyState  pystate(&m_pystate);
    m_breakpoint = kdlib::softwareBreakPointSet(offset, this);
    m_weakBp = false;
}

Breakpoint::Breakpoint(kdlib::MEMOFFSET_64 offset, python::object  &callback)
{
    m_callback = callback;
    AutoRestorePyState  pystate(&m_pystate);
    m_breakpoint = kdlib::softwareBreakPointSet(offset, this);
    m_weakBp = false;
}

Breakpoint::Breakpoint(kdlib::MEMOFFSET_64 offset, size_t size, kdlib::ACCESS_TYPE accessType)
{
    AutoRestorePyState  pystate(&m_pystate);
    m_breakpoint = kdlib::hardwareBreakPointSet(offset, size, accessType, this);
    m_weakBp = false;
}

Breakpoint::Breakpoint(kdlib::MEMOFFSET_64 offset, size_t size, kdlib::ACCESS_TYPE accessType, python::object  &callback)
{
    m_callback = callback;
    AutoRestorePyState  pystate(&m_pystate);
    m_breakpoint = kdlib::hardwareBreakPointSet(offset, size, accessType, this);
    m_weakBp = false;
}

/////////////////////////////////////////////////////////////////////////////////

Breakpoint::~Breakpoint()
{
    AutoRestorePyState  pystate;
    if (!m_weakBp && m_breakpoint )
    {
        m_breakpoint->remove();
    }
}

/////////////////////////////////////////////////////////////////////////////////

kdlib::DebugCallbackResult Breakpoint::onHit()
{
    kdlib::DebugCallbackResult  result = kdlib::DebugCallbackNoChange;

    PyEval_RestoreThread( m_pystate );

    try {

        do {

            python::object  resObj;

            if ( m_callback )
            {
                resObj = m_callback();
            }
            else
            {
                python::override pythonHandler = get_override( "onHit" );
                if ( !pythonHandler )
                {
                    result = kdlib::DebugCallbackBreak;
                    break;
                }

                resObj = pythonHandler();
            }

            if (resObj.is_none())
            {
                result = kdlib::DebugCallbackProceed;
                break;
            }

            if (PyBool_Check(resObj.ptr()))
            {
                result = python::extract<bool>(resObj) ? kdlib::DebugCallbackBreak : kdlib::DebugCallbackProceed;
                break;
            }

            python::extract<int>  resInt(resObj);
            if (resInt.check())
            {
                result = resInt < kdlib::DebugCallbackMax ? kdlib::DebugCallbackResult(resInt()) : kdlib::DebugCallbackBreak;
                break;
            }

            result = kdlib::DebugCallbackBreak;

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

/////////////////////////////////////////////////////////////////////////////////

void Breakpoint::remove()
{
    AutoRestorePyState  pystate;
    if (m_breakpoint)
    {
        m_breakpoint->remove();
        m_breakpoint = 0;
        return;
    }

    throw kdlib::DbgException("Cannot remove breakpoint, it is detached");
}

/////////////////////////////////////////////////////////////////////////////////

Breakpoint* Breakpoint::detach()
{
    AutoRestorePyState  pystate;

    if (m_weakBp)
    {
        throw kdlib::DbgException("Cannot detach 'weak' breakpoint");
    }

    if (m_breakpoint)
    {
        if (m_callback)
        {
            throw kdlib::DbgException("Cannot detach breakpoint with callback");
        }

        auto  bp = m_breakpoint;
        m_breakpoint = 0;
        return new Breakpoint(bp);
    }

    throw kdlib::DbgException("Cannot remove breakpoint, it is detached");
}

/////////////////////////////////////////////////////////////////////////////////

Breakpoint* Breakpoint::setSoftwareBreakpoint( kdlib::MEMOFFSET_64 offset, python::object  &callback )
{
    if (!callback)
    {
        AutoRestorePyState  pystate;
        kdlib::BreakpointPtr  bp = kdlib::softwareBreakPointSet(offset, 0);
        return new Breakpoint(bp, false);
    }
    else
    {
        return new Breakpoint(offset, callback);
    }
}

/////////////////////////////////////////////////////////////////////////////////

Breakpoint* Breakpoint::setHardwareBreakpoint( kdlib::MEMOFFSET_64 offset, size_t size, kdlib::ACCESS_TYPE accessType, python::object  &callback )
{
    if (!callback)
    {
        AutoRestorePyState  pystate;
        kdlib::BreakpointPtr  bp = kdlib::hardwareBreakPointSet(offset, size, accessType, 0);
        return new Breakpoint(bp, false);
    }
    else
    {
        return new Breakpoint(offset, size, accessType, callback);
    }
}

/////////////////////////////////////////////////////////////////////////////////

unsigned long Breakpoint::getNumberBreakpoints()
{
    AutoRestorePyState  pystate;
    return kdlib::getNumberBreakpoints();
}

/////////////////////////////////////////////////////////////////////////////////

Breakpoint* Breakpoint::getBreakpointByIndex(unsigned long index)
{
    kdlib::BreakpointPtr  bp;

    {
        AutoRestorePyState  pystate;
        bp = kdlib::getBreakpointByIndex(index);
    }

    return new Breakpoint(bp);
}

/////////////////////////////////////////////////////////////////////////////////

void Breakpoint::removeBreakpointByIndex(unsigned long index)
{
    AutoRestorePyState  pystate;
    kdlib::BreakpointPtr  bp;
    bp = kdlib::getBreakpointByIndex(index);
    bp->remove();
}

/////////////////////////////////////////////////////////////////////////////////

void Breakpoint::removeAllBreakpoints()
{
    AutoRestorePyState  pystate;

    while (kdlib::getNumberBreakpoints() > 0)
    {
        kdlib::BreakpointPtr  bp = kdlib::getBreakpointByIndex(0);
        bp->remove();
    }
}

/////////////////////////////////////////////////////////////////////////////////


} // end namespace pykd
