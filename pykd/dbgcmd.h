#pragma once

#include <string>
#include <map>

/////////////////////////////////////////////////////////////////////////////////

std::string
dbgCommand( const std::string &command );

template <ULONG status>
bool
setExecutionStatus()
{
    HRESULT     hres;
    
    try {
  
        hres = dbgExt->control->SetExecutionStatus( status );
        
        if ( FAILED( hres ) )
            return false;
            
        ULONG    currentStatus;                
          
        do {
        
            Py_BEGIN_ALLOW_THREADS
            
            hres = dbgExt->control->WaitForEvent( 0, INFINITE );
            
            Py_END_ALLOW_THREADS
        
            if ( FAILED( hres ) )
                throw  DbgException( "IDebugControl::SetExecutionStatus  failed" ); 
                            
            hres = dbgExt->control->GetExecutionStatus( &currentStatus );
                
            if ( FAILED( hres ) )
                throw  DbgException( "IDebugControl::GetExecutionStatus  failed" ); 
               
                
        } while( currentStatus != DEBUG_STATUS_BREAK && currentStatus != DEBUG_STATUS_NO_DEBUGGEE );
        
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
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////

class dbgExtensionClass {

public:

    dbgExtensionClass() :
        m_handle( NULL )
        {}
    
    dbgExtensionClass( const char* path );
    
    ~dbgExtensionClass();
    
    std::string
    call( const std::string &command, const std::string param );

    std::string
    print() const;
    
private:

    ULONG64         m_handle;  
	std::string     m_path;
};


/////////////////////////////////////////////////////////////////////////////////

ULONG64
evaluate( const std::string  &expression );
    
/////////////////////////////////////////////////////////////////////////////////