#include "stdafx.h"

#include "dbgext.h"
#include "dbgcmd.h"
#include "dbgexcept.h"
#include "dbgcallback.h"

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

dbgExtensionClass::dbgExtensionClass( const char* path )
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
