#include "stdafx.h"

#include <boost/format.hpp>

#include "dbgbreak.h"
#include "dbgexcept.h"
#include "pyaux.h"

///////////////////////////////////////////////////////////////////////////////

dbgBreakpointClass::breakpointMap       dbgBreakpointClass::m_breakMap;

///////////////////////////////////////////////////////////////////////////////

HRESULT dbgBreakpointClass::onBreakpointEvnet( IDebugBreakpoint*  bp )
{
    PyThread_StateSave( dbgExt->getThreadState() ); 

    try {

        breakpointMap::iterator   it = m_breakMap.find( bp );
        if ( it != m_breakMap.end() )    
            return  boost::python::extract<HRESULT>( it->second->m_callback() );
            
    }
    catch(...)
    {}
    
    return DEBUG_STATUS_NO_CHANGE;               
}

///////////////////////////////////////////////////////////////////////////////

dbgBreakpointClass::dbgBreakpointClass( ULONG64 offset, boost::python::object  &callback )
{
    m_offset = offset;
    m_breakpoint = NULL;
    m_callback = callback;

    set();    
}
  
///////////////////////////////////////////////////////////////////////////////  
    
dbgBreakpointClass::~dbgBreakpointClass()
{
    remove();
}

///////////////////////////////////////////////////////////////////////////////

bool
dbgBreakpointClass::set()
{
    HRESULT             hres;
    
    try {
        
        if ( m_breakpoint )
            return true;            
    
        hres = dbgExt->control->AddBreakpoint( DEBUG_BREAKPOINT_CODE, DEBUG_ANY_ID, &m_breakpoint );
        if (  FAILED( hres ) )
            throw  DbgException( "IDebugControl::AddBreakpoint  failed" ); 
    
        hres = m_breakpoint->SetOffset( m_offset );
        if (  FAILED( hres ) )
            throw  DbgException( "IDebugBreakpoint::SetOffset  failed" );   
            
        hres = m_breakpoint->SetFlags( DEBUG_BREAKPOINT_ENABLED );
        if (  FAILED( hres ) )
            throw  DbgException( "IDebugBreakpoint::SetFlags  failed" );   
            
        m_breakMap.insert( std::make_pair( m_breakpoint, this ) );
            
        return true;                      
    } 
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	
	
	remove();
	
	return false;
}
    
///////////////////////////////////////////////////////////////////////////////    
    
void
dbgBreakpointClass::remove()
{
    if ( m_breakpoint )
    {
        dbgExt->control->RemoveBreakpoint( m_breakpoint );
        
        breakpointMap::iterator   bp = m_breakMap.find( m_breakpoint );
        if ( bp != m_breakMap.end() )
            m_breakMap.erase( bp );
        
        m_breakpoint = NULL;
    }
}

/////////////////////////////////////////////////////////////////////////////// 

std::string
dbgBreakpointClass::print() const
{
	HRESULT status = S_OK; 

	try
	{
		if (!m_breakpoint)
			return "not set";

		DEBUG_BREAKPOINT_PARAMETERS params;
		status = m_breakpoint->GetParameters(&params);
		if (FAILED(status))
			throw DbgException("IDebugBreakpoint::GetParameters failed");

		boost::format fmt("%1$2d %2%%3% %4%:*** ");
		fmt % params.Id 
		    % (params.Flags & DEBUG_BREAKPOINT_ENABLED ? 'e' : 'd') 
		    % 'u'
		    % params.CurrentPassCount;

		return fmt.str();
	}
	catch (std::exception & e)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch (...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}

	return "";
}

///////////////////////////////////////////////////////////////////////////////