#include "stdafx.h"

#include "kdlib\eventhandler.h"

#include "eventhandler.h"
#include "dbgexcept.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

class SoftwareBreakpoint : public Breakpoint
{
public:

    SoftwareBreakpoint(  kdlib::MEMOFFSET_64 offset ) {
        m_id = kdlib::softwareBreakPointSet( offset );
    }

    ~SoftwareBreakpoint() {
        kdlib::breakPointRemove( m_id );
    }

private:

    kdlib::BREAKPOINT_ID  m_id;

};

///////////////////////////////////////////////////////////////////////////////

class SoftwareBreakpointWithCallback : public Breakpoint, private kdlib::EventHandler
{
public:

    SoftwareBreakpointWithCallback( kdlib::MEMOFFSET_64 offset, python::object &callback )
    {
        m_pystate = PyThreadState_Get();
        m_callback = callback;
        m_id = kdlib::softwareBreakPointSet( offset );
    }

    ~SoftwareBreakpointWithCallback()
    {
        try {
            kdlib::breakPointRemove( m_id );
        } catch( kdlib::DbgException& )
        { }
    }

private:

    virtual kdlib::DebugCallbackResult onBreakpoint( kdlib::BREAKPOINT_ID bpId ) 
    {

        kdlib::DebugCallbackResult  result;

        if ( bpId != m_id )
            return kdlib::DebugCallbackNoChange;

        PyEval_RestoreThread( m_pystate );

        try {

            do {

                python::object  resObj = m_callback( bpId );

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

    kdlib::BREAKPOINT_ID  m_id;
    python::object  m_callback;
    PyThreadState*  m_pystate;

};

///////////////////////////////////////////////////////////////////////////////

BreakpointPtr Breakpoint::setSoftwareBreakpoint( kdlib::MEMOFFSET_64 offset, python::object &callback )
{
    if ( callback )
    { 
        return BreakpointPtr( new SoftwareBreakpointWithCallback( offset, callback ) );
    }
    else
    {
        return BreakpointPtr( new SoftwareBreakpoint(offset) );
    }
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

} // end namespace pykd
