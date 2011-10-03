#include "stdafx.h"

#include "dbgclient.h"
#include <vector>


namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

DebugClientPtr  g_dbgClient( DebugClient::createDbgClient() );

void loadDump( const std::wstring &fileName ) {
    g_dbgClient->loadDump( fileName );    
}

void startProcess( const std::wstring  &processName ) {
    g_dbgClient->startProcess( processName );    
}

void attachProcess( ULONG  processId ) {
    g_dbgClient->attachProcess( processId );
}

void attachKernel( const std::wstring  &param ) {
    g_dbgClient->attachKernel( param );
}


///////////////////////////////////////////////////////////////////////////////////

DebugClientPtr  DebugClient::setDbgClientCurrent( DebugClientPtr  newDbgClient ) {
    DebugClientPtr  oldClient = g_dbgClient;
    g_dbgClient = newDbgClient;
    return oldClient;
}

///////////////////////////////////////////////////////////////////////////////////

DebugClient::DebugClient()
{
    HRESULT    hres;
    hres = DebugCreate( __uuidof(IDebugClient5), (void **)&m_client );
    if ( FAILED( hres ) )
        throw DbgException("DebugCreate failed");

    hres = m_client->QueryInterface( __uuidof(IDebugControl4), (void**)&m_control );
    if ( FAILED( hres ) )
        throw DbgException("QueryInterface IDebugControl4  failed");    

    hres = m_client->QueryInterface( __uuidof(IDebugSymbols3), (void**)&m_symbols );
    if ( FAILED( hres ) )
        throw DbgException("QueryInterface IDebugSymbols3  failed");    
}

///////////////////////////////////////////////////////////////////////////////////

DebugClient::DebugClient( IDebugClient4 *client )
{
    HRESULT    hres;

    hres = client->QueryInterface( __uuidof(IDebugClient5), (void**)&m_client );
    if ( FAILED( hres ) )
        throw DbgException("QueryInterface IDebugControl4  failed");    

    hres = client->QueryInterface( __uuidof(IDebugControl4), (void**)&m_control );
    if ( FAILED( hres ) )
        throw DbgException("QueryInterface IDebugControl4  failed");    

    hres = client->QueryInterface( __uuidof(IDebugSymbols3), (void**)&m_symbols );
    if ( FAILED( hres ) )
        throw DbgException("QueryInterface IDebugSymbols3  failed");          
}

///////////////////////////////////////////////////////////////////////////////////

void DebugClient::loadDump( const std::wstring &fileName )
{
    HRESULT     hres;
     
    hres = m_client->OpenDumpFileWide( fileName.c_str(), NULL );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient4::OpenDumpFileWide failed" );
        
    hres = m_control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE);
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::WaitForEvent failed" );
 
}

///////////////////////////////////////////////////////////////////////////////////

void DebugClient::startProcess( const std::wstring  &processName )
{
    HRESULT     hres;
        
    ULONG       opt;
    hres = m_control->GetEngineOptions( &opt );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetEngineOptions failed" );

    opt |= DEBUG_ENGOPT_INITIAL_BREAK;
    hres = m_control->SetEngineOptions( opt );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::SetEngineOptions failed" );

    std::vector< std::wstring::value_type>      cmdLine( processName.size() + 1 );
    wcscpy_s( &cmdLine[0], cmdLine.size(), processName.c_str() );

    hres = m_client->CreateProcessWide( 0, &cmdLine[0], DEBUG_PROCESS | DETACHED_PROCESS );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient4::CreateProcessWide failed" );

    hres = m_control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE);
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::WaitForEvent failed" );
}

///////////////////////////////////////////////////////////////////////////////////

void DebugClient::attachProcess( ULONG  processId )
{
    HRESULT     hres;
    
    hres = m_client->AttachProcess( 0, processId, 0 );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient::AttachProcess failed" );
}

///////////////////////////////////////////////////////////////////////////////////

void DebugClient::attachKernel( const std::wstring  &param )
{
    HRESULT     hres;

    hres = m_client->AttachKernelWide( DEBUG_ATTACH_KERNEL_CONNECTION, param.c_str() );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient5::AttachKernelWide failed" );
}

///////////////////////////////////////////////////////////////////////////////////

}; // end of namespace pykd