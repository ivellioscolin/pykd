#include "stdafx.h"

#include <vector>

#include <exception>
#include "dbgext.h"
#include "dbgexcept.h"
#include "dbgsystem.h"
#include "dbgcallback.h"

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

int
ptrSize()
{
    return is64bitSystem() ? 8 : 4;
}

///////////////////////////////////////////////////////////////////////////////////

std::string
dbgSymPath()
{
    HRESULT         hres;
    std::string     pathStr;
    
    try {
    
        ULONG    size;
        dbgExt->symbols->GetSymbolPath( NULL, 0, &size );
        
	std::vector<char> path(size);
        hres = dbgExt->symbols->GetSymbolPath( &path[0], size, NULL );
        if ( FAILED( hres ) )
             throw DbgException( "IDebugSymbols::GetSymbolPath  failed" ); 
        
        pathStr = &path[0];        
        
    }    
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	 
	
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
reloadModule( const char * moduleName  )
{
    HRESULT         hres;
     
    try {
    
        // подавить вывод сообщений об отсутствии символов
        OutputReader        outputReader( dbgExt->client );
 
        hres = dbgExt->symbols->Reload( moduleName );
        
        //if ( FAILED( hres ) )
        //    throw DbgException( "IDebugSymbol::Reload  failed" );
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

bool
isKernelDebugging()
{
    HRESULT         hres;
    bool            result = false;
     
    try {
    
        ULONG       debugClass, debugQualifier;
    
        hres = dbgExt->control->GetDebuggeeType( &debugClass, &debugQualifier );
    
        if ( FAILED( hres ) )
            throw DbgException( "IDebugControl::GetDebuggeeType  failed" );   
         
        result = debugClass == DEBUG_CLASS_KERNEL;
                       
    }
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	
	
	return result;
}

///////////////////////////////////////////////////////////////////////////////////

bool
isDumpAnalyzing()
{
    HRESULT         hres;
    bool            result = false;
     
    try {
    
        ULONG       debugClass, debugQualifier;
    
        hres = dbgExt->control->GetDebuggeeType( &debugClass, &debugQualifier );
    
        if ( FAILED( hres ) )
            throw DbgException( "IDebugControl::GetDebuggeeType  failed" );   
         
        result = debugQualifier >= DEBUG_DUMP_SMALL;
                       
    }
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	
	
	return result;
}

///////////////////////////////////////////////////////////////////////////////////
