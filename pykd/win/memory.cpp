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

bool isVaValid( ULONG64 addr )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT     hres;
    ULONG       offsetInfo;
    
    hres = 
        g_dbgEng->dataspace->GetOffsetInformation(
            DEBUG_DATA_SPACE_VIRTUAL,
            DEBUG_OFFSINFO_VIRTUAL_SOURCE,
            addr64NoSafe( addr ),
            &offsetInfo,
            sizeof( offsetInfo ),
            NULL );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugDataSpace4::GetOffsetInformation  failed" );

    return  offsetInfo != DEBUG_VSOURCE_INVALID;
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT readMemoryImpl(ULONG64 offset, PVOID buffer, ULONG length, ULONG *readed, bool phyAddr)
{
    HRESULT hres;
    if ( phyAddr == false )
    {
        offset = addr64NoSafe( offset );

        // workitem/10473 workaround
        ULONG64 nextAddress;
        hres = g_dbgEng->dataspace->GetNextDifferentlyValidOffsetVirtual( offset, &nextAddress );

        DBG_UNREFERENCED_LOCAL_VARIABLE(nextAddress);

        hres = g_dbgEng->dataspace->ReadVirtual( offset, buffer, length, readed );
    }
    else
    {
        hres = g_dbgEng->dataspace->ReadPhysical( offset, buffer, length, readed );
    }

    return hres;
}

///////////////////////////////////////////////////////////////////////////////////

void readMemory( ULONG64 offset, PVOID buffer, ULONG length, bool phyAddr )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    HRESULT hres = readMemoryImpl(offset, buffer, length, NULL, phyAddr);

    if ( FAILED( hres ) )
        throw MemoryException( offset, phyAddr );
}

///////////////////////////////////////////////////////////////////////////////////

std::string loadCStr( ULONG64 offset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    const size_t    maxLength = 0x10000;

    offset = addr64NoSafe( offset );

    ULONG   strLength = 0;

    HRESULT     hres = 
        g_dbgEng->dataspace->ReadMultiByteStringVirtual(
            offset,
            maxLength,
            NULL,
            0,
            &strLength );

    if ( FAILED( hres ) )
        throw MemoryException( offset );

    std::vector<char>  buffer(strLength);

    hres = 
        g_dbgEng->dataspace->ReadMultiByteStringVirtual(
            offset,
            strLength,
            &buffer[0],
            strLength,
            NULL );

    if ( FAILED( hres ) )
        throw MemoryException( offset );
                           
    return std::string( &buffer[0] );
}

///////////////////////////////////////////////////////////////////////////////////

std::wstring loadWStr( ULONG64 offset )
{
    PyThread_StateRestore pyThreadRestore( g_dbgEng->pystate );

    const size_t    maxLength = 0x10000;
 
    offset = addr64NoSafe( offset );

    ULONG   strLength = 0;

    HRESULT     hres = 
        g_dbgEng->dataspace->ReadUnicodeStringVirtualWide(
            offset,
            maxLength,
            NULL,
            0,
            &strLength );

    std::vector<wchar_t>  buffer(strLength);
        
    hres = 
        g_dbgEng->dataspace->ReadUnicodeStringVirtualWide(
            offset,
            strLength,
            &buffer[0],
            strLength,
            NULL );
    
    if ( FAILED( hres ) )
        throw MemoryException( offset );
                           
    return std::wstring( &buffer[0] );
}

///////////////////////////////////////////////////////////////////////////////////

}; //namespace pykd