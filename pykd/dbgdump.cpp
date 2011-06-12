#include "stdafx.h"

#include "dbgext.h"
#include "dbgdump.h"
#include "dbgexcept.h"
#include "dbgeventcb.h"
#include "dbgsystem.h"
#include "dbgcmd.h"
#include "dbgclient.h"

/////////////////////////////////////////////////////////////////////////////////

static
bool  dbgStarted = false;


/////////////////////////////////////////////////////////////////////////////////

bool
dbgLoadDump( const std::wstring &fileName )
{
    HRESULT     hres;
    
    try {
    
        if ( dbgStarted || isWindbgExt() )
            return false;
        
        g_dbgClient.startEventsMgr();
       
        hres = dbgExt->client4->OpenDumpFileWide( fileName.c_str(), NULL );
       
        if ( FAILED( hres ) )
            throw DbgException( "IDebugClient4::OpenDumpFileWide failed" );
            
        hres = dbgExt->control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE);
        if ( FAILED( hres ) )
            throw DbgException( "IDebugControl::WaitForEvent failed" );
            
        dbgStarted = true;            

        return true;
    }
    catch( std::exception& )
    {
    }
    catch(...)
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////// 

bool
startProcess( const std::wstring  &processName )
{
    HRESULT     hres;

    try {
        
        if ( dbgStarted || isWindbgExt() )
            return false;        
        
        g_dbgClient.startEventsMgr();        

        ULONG       opt;
        hres = dbgExt->control->GetEngineOptions( &opt );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugControl::GetEngineOptions failed" );

        opt |= DEBUG_ENGOPT_INITIAL_BREAK;
        hres = dbgExt->control->SetEngineOptions( opt );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugControl::SetEngineOptions failed" );

        std::vector< std::wstring::value_type>      cmdLine( processName.size() + 1 );
        wcscpy_s( &cmdLine[0], cmdLine.size(), processName.c_str() );

        hres = dbgExt->client4->CreateProcessWide( 0, &cmdLine[0], DEBUG_PROCESS | DETACHED_PROCESS );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugClient4::CreateProcessWide failed" );

        hres = dbgExt->control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE);
        if ( FAILED( hres ) )
            throw DbgException( "IDebugControl::WaitForEvent failed" );
            
        dbgStarted = true;            

        return true;
    }
    catch( std::exception& e )
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