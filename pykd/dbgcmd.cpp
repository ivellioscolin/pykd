#include "stdafx.h"

#include <boost/format.hpp>

#include "dbgext.h"
#include "dbgcmd.h"
#include "dbgexcept.h"
#include "dbgcallback.h"
#include "dbgsystem.h"

///////////////////////////////////////////////////////////////////////////////

std::string
dbgCommand( const std::string &command )
{
    HRESULT     hres;

    try {
    
        OutputReader        outReader(  dbgExt->client );
   
        hres = dbgExt->control->Execute( DEBUG_OUTCTL_THIS_CLIENT, command.c_str(), 0 );
        if ( FAILED( hres ) )
            throw  DbgException( "IDebugControl::Execute  failed" ); 
            
        return std::string( outReader.Line() );
    }    
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	
	
	return "error"; 
}	

///////////////////////////////////////////////////////////////////////////////

dbgExtensionClass::dbgExtensionClass( const char* path ) : m_path(path)
{
    HRESULT     hres;

    try {
        
        hres = dbgExt->control->AddExtension( path, 0, &m_handle );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugControl::AddExtension failed" );
        
    }
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	    
}

///////////////////////////////////////////////////////////////////////////////
    
dbgExtensionClass::~dbgExtensionClass()
{
    if ( m_handle )
        dbgExt->control->RemoveExtension( m_handle );
}

///////////////////////////////////////////////////////////////////////////////

std::string
dbgExtensionClass::call( const std::string &command, const std::string params )
{
    HRESULT     hres;

    try {
    
        OutputReader        outReader(  dbgExt->client );
   
        hres = dbgExt->control->CallExtension( m_handle, command.c_str(), params.c_str() );
        if ( FAILED( hres ) )
            throw  DbgException( "IDebugControl::CallExtension  failed" ); 
            
        return std::string( outReader.Line() );
    }    
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	
	
	return "error"; 
}

///////////////////////////////////////////////////////////////////////////////

std::string
dbgExtensionClass::print() const
{
	HRESULT status = S_OK;

	try
	{
		return m_handle ? m_path : "";
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

dbgBreakpointClass::breakpointMap       dbgBreakpointClass::m_breakMap;

///////////////////////////////////////////////////////////////////////////////

HRESULT dbgBreakpointClass::onBreakpointEvnet( IDebugBreakpoint*  bp )
{
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

ULONG64
evaluate( const std::string  &expression )
{
    HRESULT             hres;
    ULONG64             value = 0;
    
    try {
    
        DEBUG_VALUE  debugValue = {};
        ULONG        remainderIndex = 0;
        
        if ( is64bitSystem() )
        {
            hres = dbgExt->control->Evaluate( 
                expression.c_str(), 
                DEBUG_VALUE_INT64,
                &debugValue,
                &remainderIndex );
                
            if (  FAILED( hres ) )
                throw  DbgException( "IDebugControl::Evaluate  failed" );             
                
            if ( remainderIndex == expression.length() )
                value = debugValue.I64;
        }
        else
        {
            hres = dbgExt->control->Evaluate( 
                expression.c_str(), 
                DEBUG_VALUE_INT32,
                &debugValue,
                &remainderIndex );
                
            if (  FAILED( hres ) )
                throw  DbgException( "IDebugControl::Evaluate  failed" );             
                
            if ( remainderIndex == expression.length() )
                value = debugValue.I32;
        }      
    } 
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	
	
	return value;
}

/////////////////////////////////////////////////////////////////////////////// 

