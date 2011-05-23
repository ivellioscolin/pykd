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

