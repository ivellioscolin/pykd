#include "stdafx.h"

#include <exception>
#include "dbgext.h"
#include "dbgexcept.h"
#include "dbgsystem.h"

///////////////////////////////////////////////////////////////////////////////////

bool
is64bitSystem()
{
    HRESULT     hres;
    
    try {
    
        hres = dbgExt->control->IsPointer64Bit();
        
        return hres == S_OK;    
        
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

///////////////////////////////////////////////////////////////////////////////////

std::string
dbgSymPath()
{
    HRESULT         hres;
    char            *path = NULL;
    std::string     pathStr;
    
    try {
    
        ULONG    size;
        dbgExt->symbols->GetSymbolPath( NULL, 0, &size );
        
        path = new char[ size ];
        hres = dbgExt->symbols->GetSymbolPath( path, size, NULL );
        if ( FAILED( hres ) )
             throw DbgException( "IDebugSymbols::GetSymbolPath  failed" ); 
        
        pathStr = path;        
        
    }    
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	 
	
    if ( path )
        delete[]  path;
        
    return pathStr;
}


///////////////////////////////////////////////////////////////////////////////////
