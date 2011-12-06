#include "stdafx.h"
#include <vector>

#include "dbgclient.h"


namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

DebugClientPtr  g_dbgClient( DebugClient::createDbgClient() );

///////////////////////////////////////////////////////////////////////////////////

DebugClient::DebugClient( IDebugClient4 *client )
    : DbgObject( client )
    , m_symSymbols( new SyntheticSymbols(*m_symbols, *this) )
    , m_internalDbgEventHandler(client, m_symSymbols)
{
}

///////////////////////////////////////////////////////////////////////////////////

DebugClientPtr DebugClient::createDbgClient() {

    HRESULT                  hres;
    CComPtr<IDebugClient4>   client = NULL;

    hres = DebugCreate( __uuidof(IDebugClient4), (void **)&client );
    if ( FAILED( hres ) )
        throw DbgException("DebugCreate failed");

    return  createDbgClient( client );
}

///////////////////////////////////////////////////////////////////////////////////

DebugClientPtr DebugClient::createDbgClient( IDebugClient4 *client ) {

    HRESULT                 hres;
    CComPtr<IDebugClient>   newClient = NULL;

    hres = client->CreateClient( &newClient );
    if ( FAILED( hres ) )
        throw DbgException("DebugCreate failed");

    CComQIPtr<IDebugClient4>  client4=  newClient;

    return DebugClientPtr( new DebugClient(client4) );
}

///////////////////////////////////////////////////////////////////////////////////

DebugClientPtr  DebugClient::setDbgClientCurrent( DebugClientPtr  newDbgClient ) {
    DebugClientPtr  oldClient = g_dbgClient;
    g_dbgClient = newDbgClient;
    return oldClient;
}

///////////////////////////////////////////////////////////////////////////////////

python::tuple DebugClient::getDebuggeeType()
{
    HRESULT         hres;
    ULONG           debugClass, debugQualifier;
    
    hres = m_control->GetDebuggeeType( &debugClass, &debugQualifier );
    
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetDebuggeeType  failed" );   

    return python::make_tuple( debugClass, debugQualifier );
}

python::tuple getDebuggeeType()
{
    return g_dbgClient->getDebuggeeType();
}

///////////////////////////////////////////////////////////////////////////////////

ULONG DebugClient::getExecutionStatus()
{
    ULONG       currentStatus;
    HRESULT     hres;

    hres = m_control->GetExecutionStatus( &currentStatus );

    if ( FAILED( hres ) )
        throw  DbgException( "IDebugControl::GetExecutionStatus  failed" ); 

    return currentStatus;
}

ULONG getExecutionStatus()
{
    return  g_dbgClient->getExecutionStatus();
}

///////////////////////////////////////////////////////////////////////////////////

bool DebugClient::is64bitSystem()
{
    HRESULT     hres;
    
    hres = m_control->IsPointer64Bit();
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::IsPointer64Bit failed" );
        
    return hres == S_OK;    
}

bool
is64bitSystem()
{
    return g_dbgClient->is64bitSystem();
}


///////////////////////////////////////////////////////////////////////////////////

bool DebugClient::isDumpAnalyzing()
{
    HRESULT         hres;
    ULONG           debugClass, debugQualifier;
    
    hres = m_control->GetDebuggeeType( &debugClass, &debugQualifier );
    
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetDebuggeeType  failed" );   
         
    return debugQualifier >= DEBUG_DUMP_SMALL;
}

bool isDumpAnalyzing() 
{
    return g_dbgClient->isDumpAnalyzing();
}

///////////////////////////////////////////////////////////////////////////////////

bool DebugClient::isKernelDebugging()
{
    HRESULT     hres;
    ULONG       debugClass, debugQualifier;
    
    hres = m_control->GetDebuggeeType( &debugClass, &debugQualifier );
    
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetDebuggeeType  failed" );   
         
    return debugClass == DEBUG_CLASS_KERNEL;
}

bool isKernelDebugging() 
{
    return g_dbgClient->isKernelDebugging();
}

//////////////////////////////////////////////////////////////////////////////////

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

void loadDump( const std::wstring &fileName ) {
    g_dbgClient->loadDump( fileName );    
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

void startProcess( const std::wstring  &processName ) {
    g_dbgClient->startProcess( processName );    
}

///////////////////////////////////////////////////////////////////////////////////

void DebugClient::attachProcess( ULONG  processId )
{
    HRESULT     hres;
    
    hres = m_client->AttachProcess( 0, processId, 0 );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient::AttachProcess failed" );
}

void attachProcess( ULONG  processId ) {
    g_dbgClient->attachProcess( processId );
}

///////////////////////////////////////////////////////////////////////////////////

void DebugClient::attachKernel( const std::wstring  &param )
{
    HRESULT     hres;

    hres = m_client5->AttachKernelWide( DEBUG_ATTACH_KERNEL_CONNECTION, param.c_str() );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient5::AttachKernelWide failed" );
}

void attachKernel( const std::wstring  &param ) {
    g_dbgClient->attachKernel( param );
}

///////////////////////////////////////////////////////////////////////////////////

std::string DebugClient::findSymbol( ULONG64 offset ) 
{
    HRESULT     hres;

    offset = addr64( offset );

    char        symbolName[0x100];
    ULONG64     displace = 0;
    hres = m_symbols->GetNameByOffset( offset, symbolName, sizeof(symbolName), NULL, &displace );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetNameByOffset  failed" );

    std::stringstream      ss;
    displace == 0 ?  ss << symbolName : ss << symbolName << '+' << std::hex << displace;

    return ss.str();
}

std::string findSymbol( ULONG64 offset ) 
{
    return g_dbgClient->findSymbol( offset );
}

///////////////////////////////////////////////////////////////////////////////////

void DebugClient::setExecutionStatus( ULONG status )
{
    HRESULT     hres;

    hres = m_control->SetExecutionStatus( status );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::SetExecutionStatus failed" );

}

void setExecutionStatus( ULONG status )
{
    g_dbgClient->setExecutionStatus( status );
}

///////////////////////////////////////////////////////////////////////////////////

void DebugClient::waitForEvent()
{
    HRESULT     hres;

    do {
        PyThread_StateRestore pyThreadRestore( m_pyThreadState );
        hres = m_control->WaitForEvent( 0, INFINITE );

    } while( false );

    if ( FAILED( hres ) )
    {
        if (E_UNEXPECTED == hres)
            throw WaitEventException();

        throw  DbgException( "IDebugControl::WaitForEvent  failed" );
    }
}

void waitForEvent()
{
    g_dbgClient->waitForEvent();
}

///////////////////////////////////////////////////////////////////////////////////

ULONG DebugClient::ptrSize()
{
    HRESULT     hres;

    hres = m_control->IsPointer64Bit();

    if ( FAILED( hres ) )
        throw  DbgException( "IDebugControl::IsPointer64Bit  failed" );
    
    return S_OK == hres ? 8 : 4;
}

///////////////////////////////////////////////////////////////////////////////////

ULONG ptrSize()
{ 
    return g_dbgClient->ptrSize();
}

///////////////////////////////////////////////////////////////////////////////////

}; // end of namespace pykd
