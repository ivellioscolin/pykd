// 
// Debug engine: processes and treads
// 

///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dbgeng.h"

///////////////////////////////////////////////////////////////////////////////

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

ULONG startProcess( const std::wstring  &processName, bool debugChildren )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    ULONG       opt;
    hres = g_dbgEng->control->GetEngineOptions( &opt );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetEngineOptions failed" );

    opt |= DEBUG_ENGOPT_INITIAL_BREAK;
    hres = g_dbgEng->control->SetEngineOptions( opt );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::SetEngineOptions failed" );

    std::vector< std::wstring::value_type >      cmdLine( processName.size() + 1 );
    wcscpy_s( &cmdLine[0], cmdLine.size(), processName.c_str() );

    hres = g_dbgEng->client->CreateProcessWide( 
        0,
        &cmdLine[0],
        ( debugChildren ? DEBUG_PROCESS : DEBUG_ONLY_THIS_PROCESS) | DETACHED_PROCESS 
        );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient4::CreateProcessWide failed" );

    hres = g_dbgEng->control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE);
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::WaitForEvent failed" );

    ULONG processId = -1;
    hres = g_dbgEng->system->GetCurrentProcessId( &processId );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects::GetCurrentProcessId failed" );

    return processId;
}

///////////////////////////////////////////////////////////////////////////////////

ULONG attachProcess( ULONG pid )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    ULONG       opt;
    hres = g_dbgEng->control->GetEngineOptions( &opt );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetEngineOptions failed" );

    opt |= DEBUG_ENGOPT_INITIAL_BREAK;
    hres = g_dbgEng->control->SetEngineOptions( opt );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::SetEngineOptions failed" );

    hres = g_dbgEng->client->AttachProcess( 0, pid, 0 );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient::AttachProcess failed" );

    hres = g_dbgEng->control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE);
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::WaitForEvent failed" );

    ULONG processId = -1;
    hres = g_dbgEng->system->GetCurrentProcessId( &processId );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects::GetCurrentProcessId failed" );

    return processId;
}

///////////////////////////////////////////////////////////////////////////////////

void attachKernel( const std::string &connectOptions )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    ULONG       opt;
    HRESULT hres = g_dbgEng->control->GetEngineOptions( &opt );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetEngineOptions", hres );

    opt |= DEBUG_ENGOPT_INITIAL_BREAK;
    hres = g_dbgEng->control->SetEngineOptions( opt );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::SetEngineOptions", hres );

    hres = 
        g_dbgEng->client->AttachKernel(
            connectOptions.empty() ? DEBUG_ATTACH_LOCAL_KERNEL : DEBUG_ATTACH_KERNEL_CONNECTION,
            connectOptions.empty() ? NULL : connectOptions.c_str());
    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient::AttachKernel", hres );

    hres = g_dbgEng->control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE);
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::WaitForEvent", hres );
}

///////////////////////////////////////////////////////////////////////////////////

bool isLocalKernelDebuggerEnabled()
{
    HRESULT hres = g_dbgEng->client->IsKernelDebuggerEnabled();
    if ( ( hres != S_OK ) && ( hres != S_FALSE ) )
        throw DbgException( "IDebugClient::IsKernelDebuggerEnabled", hres );
    return hres == S_OK;
}

///////////////////////////////////////////////////////////////////////////////////

void detachProcess( ULONG processId )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;

    if ( processId != -1 )
    {
        hres = g_dbgEng->system->SetCurrentProcessId(processId);
        if ( FAILED(hres) )
            throw DbgException( "IDebugSystemObjects::SetCurrentProcessId failed" );
    }

    hres = g_dbgEng->client->DetachCurrentProcess();
    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient::DetachCurrentProcess failed" );
}

///////////////////////////////////////////////////////////////////////////////////

void terminateProcess( ULONG processId )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    if ( processId != -1 )
    {
        hres = g_dbgEng->system->SetCurrentProcessId(processId);
        if ( FAILED(hres) )
            throw DbgException( "IDebugSystemObjects::SetCurrentProcessId failed" );
    }

    hres = g_dbgEng->client->TerminateCurrentProcess();
    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient::TerminateCurrentProcess", hres );

    hres = g_dbgEng->client->DetachCurrentProcess();
    if ( FAILED( hres ) )
        throw DbgException( "IDebugClient::DetachCurrentProcess", hres );

}

///////////////////////////////////////////////////////////////////////////////

void detachAllProcesses()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    hres = g_dbgEng->client->DetachProcesses();

    if ( FAILED(hres) )
        throw DbgException( "IDebugClient::DetachProcesses", hres );
}

///////////////////////////////////////////////////////////////////////////////

void terminateAllProcesses()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;

    hres = g_dbgEng->client->TerminateProcesses();
    if ( FAILED(hres) )
        throw DbgException( "IDebugClient::TerminateProcesses", hres );

    hres = g_dbgEng->client->DetachProcesses();
    if ( FAILED(hres) )
        throw DbgException( "IDebugClient::DetachProcesses", hres );

}

///////////////////////////////////////////////////////////////////////////////

ULONG64
getCurrentProcess()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT         hres;  
    ULONG64         processAddr = 0;

    hres = g_dbgEng->system->GetImplicitProcessDataOffset( &processAddr );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects2::GetImplicitProcessDataOffset  failed" ); 

     return processAddr; 
}

///////////////////////////////////////////////////////////////////////////////

ULONG getCurrentProcessId()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT      hres;
    ULONG        pid;

    hres = g_dbgEng->system->GetCurrentProcessSystemId( &pid );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects2::GetCurrentProcessSystemId  failed" ); 
        
     return pid; 
}

///////////////////////////////////////////////////////////////////////////////

ULONG64 
getImplicitThread()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres; 
    ULONG64     threadOffset = -1;

    hres = g_dbgEng->system->GetImplicitThreadDataOffset( &threadOffset );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects2::GetImplicitThreadDataOffset  failed" ); 
        
    return threadOffset;
}

///////////////////////////////////////////////////////////////////////////////

ULONG getCurrentThreadId()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT      hres;
    ULONG        tid;

    hres = g_dbgEng->system->GetCurrentThreadSystemId( &tid );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects2::GetCurrentThreadSystemId  failed" ); 
        
     return tid; 
}

///////////////////////////////////////////////////////////////////////////////

void setCurrentProcess( ULONG64 processAddr )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    processAddr = addr64NoSafe(processAddr);
    hres = g_dbgEng->system->SetImplicitProcessDataOffset( processAddr );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects2::SetImplicitProcessDataOffset  failed" );
}

///////////////////////////////////////////////////////////////////////////////

void setImplicitThread( ULONG64 threadAddr )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;

    threadAddr = addr64NoSafe(threadAddr);
    hres = g_dbgEng->system->SetImplicitThreadDataOffset( threadAddr );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects2::SetImplicitThreadDataOffset  failed" );
}

///////////////////////////////////////////////////////////////////////////////

void getAllProcessThreads( std::vector<ULONG64> &threadsArray )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;
    ULONG       debugClass, debugQualifier;

    hres = g_dbgEng->control->GetDebuggeeType( &debugClass, &debugQualifier );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetDebuggeeType  failed" );   
         
    if  ( debugClass != DEBUG_CLASS_USER_WINDOWS )
        throw DbgException( "getAllProcessThreads routine only for usermode" );

    ULONG  threadsNumber = 0;

    hres = g_dbgEng->system->GetNumberThreads( &threadsNumber );
    if ( FAILED(hres) )
        throw DbgException( "IDebugSystemObjects::GetNumberThreads failed" );

    std::vector<ULONG>  ids(threadsNumber);

    hres = g_dbgEng->system->GetThreadIdsByIndex( 0, threadsNumber, &ids[0], NULL );
    if ( FAILED(hres) )
        throw DbgException( "IDebugSystemObjects::GetThreadIdsByIndex failed" );

    ULONG  currentThreadId;
    hres = g_dbgEng->system->GetCurrentThreadId( &currentThreadId );
    if ( FAILED(hres) )
        throw DbgException( "IDebugSystemObjects::GetCurrentThreadId failed" );

    threadsArray.resize( threadsNumber );
    for ( size_t i = 0; i < threadsNumber; ++i )
    {
        hres = g_dbgEng->system->SetCurrentThreadId( ids[i] );
        if ( FAILED(hres) )
        {   
            g_dbgEng->system->SetCurrentThreadId( currentThreadId );
            throw DbgException( "IDebugSystemObjects::SetCurrentThreadId failed" );
        }

        hres = g_dbgEng->system->GetCurrentThreadTeb( &threadsArray[i] );
        if ( FAILED(hres) )
        {   
            g_dbgEng->system->SetCurrentThreadId( currentThreadId );
            throw DbgException( "IDebugSystemObjects::GetCurrentThreadTeb failed" );
        }
    }

     g_dbgEng->system->SetCurrentThreadId( currentThreadId );
}

///////////////////////////////////////////////////////////////////////////////

std::string getCurrentProcessExecutableName()
{
    boost::scoped_array< CHAR > exeName( new CHAR[MAXWORD+1] );
    memset(&exeName[0], 0, MAXWORD+1);
    ULONG tmp;
    HRESULT hres = 
        g_dbgEng->system->GetCurrentProcessExecutableName(
            &exeName[0],
            MAXWORD,
            &tmp);
    if (S_OK != hres)
        throw DbgException("IDebugSystemObjects::GetCurrentProcessExecutableName", hres);

    return std::string(&exeName[0]);
}

///////////////////////////////////////////////////////////////////////////////

}   // namespace pykd

///////////////////////////////////////////////////////////////////////////////

