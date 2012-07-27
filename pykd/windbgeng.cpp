#include "stdafx.h"

#include "windbgeng.h"
#include "dbgexcept.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

DebugEngine     g_dbgEng;

///////////////////////////////////////////////////////////////////////////////////

ULONG startProcess( const std::wstring  &processName )
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

    hres = g_dbgEng->client->CreateProcessWide( 0, &cmdLine[0], DEBUG_PROCESS | DETACHED_PROCESS );
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

}

///////////////////////////////////////////////////////////////////////////////////

void debugGo()
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    hres = g_dbgEng->control->SetExecutionStatus( DEBUG_STATUS_GO );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::SetExecutionStatus failed" );

    ULONG    currentStatus;

    do {
        hres = g_dbgEng->control->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE);
        if ( FAILED( hres ) )
            throw DbgException( "IDebugControl::WaitForEvent failed" );

        hres = g_dbgEng->control->GetExecutionStatus( &currentStatus );

        if ( FAILED( hres ) )
            throw  DbgException( "IDebugControl::GetExecutionStatus  failed" ); 

    } while( currentStatus != DEBUG_STATUS_BREAK && currentStatus != DEBUG_STATUS_NO_DEBUGGEE );
}

///////////////////////////////////////////////////////////////////////////////////

ULONG64 findModuleBase( const std::string &moduleName )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;
    ULONG64     base;

    hres = g_dbgEng->symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &base );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleByModuleName  failed" ); 

    return base;
}

///////////////////////////////////////////////////////////////////////////////////

ULONG64 findModuleBase( ULONG64 offset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;
    ULONG64     base;
    ULONG       moduleIndex;

    hres = g_dbgEng->symbols->GetModuleByOffset( offset, 0, &moduleIndex, &base );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleByOffset failed" );

    return base;
}

///////////////////////////////////////////////////////////////////////////////////

std::string getModuleName( ULONG64 baseOffset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT  hres;

    char  moduleName[0x100];

    hres = g_dbgEng->symbols->GetModuleNameString( 
        DEBUG_MODNAME_MODULE,
        DEBUG_ANY_ID,
        baseOffset,
        moduleName,
        sizeof( moduleName ),
        NULL );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleNameString failed" );

    return std::string( moduleName );
}

///////////////////////////////////////////////////////////////////////////////////

ULONG64 addr64( ULONG64 addr )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    ULONG   processorMode;
    hres = g_dbgEng->control->GetActualProcessorType( &processorMode );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetEffectiveProcessorType  failed" );

    switch( processorMode )
    {
    case IMAGE_FILE_MACHINE_I386:
        if ( *( (ULONG*)&addr + 1 ) == 0 )
            return (ULONG64)(LONG)addr;

    case IMAGE_FILE_MACHINE_AMD64:
        break;

    default:
        throw DbgException( "Unknown processor type" );
        break;
    }

    return addr;
}

///////////////////////////////////////////////////////////////////////////////////

};


