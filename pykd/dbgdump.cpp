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

void
dbgLoadDump( const std::wstring &fileName )
{
    HRESULT     hres;
    
    if ( dbgStarted || isWindbgExt() )
        throw DbgException( "debugger is alread attached" );
    
    g_dbgClient.startEventsMgr();
   
    hres = dbgExt->client4->OpenDumpFileWide( fileName.c_str(), NULL );
   
    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient4::OpenDumpFileWide failed" );
        
    hres = dbgExt->control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE);
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::WaitForEvent failed" );
        
    dbgStarted = true;            
}

///////////////////////////////////////////////////////////////////////////////// 

void
startProcess( const std::wstring  &processName )
{
    HRESULT     hres;
        
    if ( dbgStarted || isWindbgExt() )
        throw DbgException( "debugger is alread attached" );
    
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
}

///////////////////////////////////////////////////////////////////////////////// 

void
attachProcess( ULONG  processId )
{
    HRESULT     hres;
    
    hres = dbgExt->client->AttachProcess( 0, processId, 0 );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient::AttachProcess failed" );
}

///////////////////////////////////////////////////////////////////////////////// 

    