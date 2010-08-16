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

std::string
getPdbFile( ULONG64  moduleBase )
{
    HRESULT             hres;

    try {
    

        IMAGEHLP_MODULEW64      imageHelpInfo = { 0 };

        hres = 
            dbgExt->advanced2->GetSymbolInformation(
                DEBUG_SYMINFO_IMAGEHLP_MODULEW64,
                moduleBase,
                0,
                &imageHelpInfo,
                sizeof( imageHelpInfo ),
                NULL,
                NULL,
                0,
                NULL );
                
        char  fileName[ 256 ];                
        WideCharToMultiByte( CP_ACP, 0, imageHelpInfo.LoadedPdbName, 256, fileName, 256, NULL, NULL );
         
        return std::string( fileName );
        
    }    
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	 
        
    return  std::string();
}

///////////////////////////////////////////////////////////////////////////////////

void
reloadSymbols( const char * moduleName  )
{
    HRESULT         hres;
     
    try {
 
        std::string   reloadParam( "/f " );
        reloadParam += moduleName;

        hres = dbgExt->symbols->Reload( reloadParam.c_str() );
        
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSymbol::Reload  failed" );      
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

///////////////////////////////////////////////////////////////////////////////////

