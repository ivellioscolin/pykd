#include "stdafx.h"

#include "win/dbgeng.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

ULONG64 addr64NoSafe( ULONG64 addr )
{
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

ULONG64 addr64( ULONG64 addr )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    return addr64NoSafe( addr );
}

///////////////////////////////////////////////////////////////////////////////////

void readMemory( ULONG64 offset, PVOID buffer, ULONG length, bool phyAddr )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;

    if ( phyAddr == false )
    {
        offset = addr64NoSafe( offset );

        // workitem/10473 workaround
        ULONG64 nextAddress;
        hres = 
            g_dbgEng->dataspace->GetNextDifferentlyValidOffsetVirtual( offset, &nextAddress );

        DBG_UNREFERENCED_LOCAL_VARIABLE(nextAddress);

        hres = g_dbgEng->dataspace->ReadVirtual( offset, buffer, length, NULL );
    }
    else
    {
        hres = g_dbgEng->dataspace->ReadPhysical( offset, buffer, length, NULL );
    }

    if ( FAILED( hres ) )
        throw MemoryException( offset, phyAddr );
}

///////////////////////////////////////////////////////////////////////////////////

}; //namespace pykd