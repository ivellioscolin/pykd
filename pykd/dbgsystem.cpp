#include "stdafx.h"

#include "dbgext.h"
#include "dbgexcept.h"
#include "dbgsystem.h"
#include "dbgio.h"

///////////////////////////////////////////////////////////////////////////////////

bool
is64bitSystem()
{
    HRESULT     hres;
    
    hres = dbgExt->control->IsPointer64Bit();
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::IsPointer64Bit failed" );
        
    return hres == S_OK;    
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
    
    ULONG    size;
    dbgExt->symbols->GetSymbolPath( NULL, 0, &size );

        
	std::vector<char> path(size);
    hres = dbgExt->symbols->GetSymbolPath( &path[0], size, NULL );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbols::GetSymbolPath  failed" ); 
        
    return std::string(&path[0]);
}


///////////////////////////////////////////////////////////////////////////////////

std::string
getPdbFile( ULONG64  moduleBase )
{
    HRESULT             hres;



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

    if ( FAILED( hres ) )
        throw DbgException( "IDebugAdvanced2::GetSymbolInformation  failed" );

    char  fileName[ 256 ];                
    WideCharToMultiByte( CP_ACP, 0, imageHelpInfo.LoadedPdbName, 256, fileName, 256, NULL, NULL );
        
    return std::string( fileName );      
}

///////////////////////////////////////////////////////////////////////////////////

void
reloadModule( const char * moduleName  )
{
    // подавить вывод сообщений об отсутствии символов
    OutputReader        outputReader( dbgExt->client );
 
    dbgExt->symbols->Reload( moduleName );
}

///////////////////////////////////////////////////////////////////////////////////

bool
isKernelDebugging()
{
    HRESULT     hres;
    ULONG       debugClass, debugQualifier;
    
    hres = dbgExt->control->GetDebuggeeType( &debugClass, &debugQualifier );
    
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetDebuggeeType  failed" );   
         
    return debugClass == DEBUG_CLASS_KERNEL;
}

///////////////////////////////////////////////////////////////////////////////////

bool
isDumpAnalyzing()
{
    HRESULT         hres;
    ULONG           debugClass, debugQualifier;
    
    hres = dbgExt->control->GetDebuggeeType( &debugClass, &debugQualifier );
    
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetDebuggeeType  failed" );   
         
    return debugQualifier >= DEBUG_DUMP_SMALL;
}

///////////////////////////////////////////////////////////////////////////////////
