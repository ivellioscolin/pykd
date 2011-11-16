#include "stdafx.h"

#include "dbgclient.h"
#include "dbgexcept.h"

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct PyListType
{
    typedef T       ElementType;
};

template<>
struct PyListType<char>
{
    typedef int     ElementType;
};

template<typename T>
python::list
DebugClient::loadArray( ULONG64 offset, ULONG count, bool phyAddr )
{
    std::vector<T>   buffer(count);

    if (count)
        readMemory( offset, &buffer[0], count*sizeof(T), phyAddr );

    python::list        lst;

    for( ULONG  i = 0; i < count; ++i )
        lst.append( static_cast<PyListType<T>::ElementType> (buffer[i]) );

    return lst;
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG64
DebugClient::addr64( ULONG64  addr)
{
    HRESULT     hres;

    ULONG   processorMode;
    hres = m_control->GetActualProcessorType( &processorMode );
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

ULONG64
addr64( ULONG64  addr)
{
    return g_dbgClient->addr64( addr );
}

/////////////////////////////////////////////////////////////////////////////////////

void DebugClient::readMemory( ULONG64 address, PVOID buffer, ULONG length, bool phyAddr )
{
    HRESULT     hres;

    if ( phyAddr == false )
    {
        hres = m_dataSpaces->ReadVirtual( address, buffer, length, NULL );
    }        
    else
    {
        hres = m_dataSpaces->ReadPhysical( address, buffer, length, NULL );
    }               
    
    if ( FAILED( hres ) )
        throw MemoryException( address, phyAddr );
}

void readMemory( ULONG64 address, PVOID buffer, ULONG length, bool phyAddr )
{
    return g_dbgClient->readMemory( address, buffer, length, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

bool DebugClient::compareMemory( ULONG64 addr1, ULONG64 addr2, ULONG length, bool phyAddr )
{
    bool        result = false;

    addr1 = addr64( addr1 );
    addr2 = addr64( addr2 );

    std::vector<char>   m1(length);
    std::vector<char>   m2(length);

    readMemory( addr1, &m1[0], length, phyAddr );
    readMemory( addr2, &m2[0], length, phyAddr );

    return std::equal( m1.begin(), m1.end(), m2.begin() );
}

bool compareMemory( ULONG64 addr1, ULONG64 addr2, ULONG length, bool phyAddr )
{
    return g_dbgClient->compareMemory( addr1, addr2, length, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

std::string DebugClient::loadChars( ULONG64 address, ULONG  number, bool phyAddr )
{
    std::vector<char>   buffer(number);

    ULONG  bufferSize = (ULONG)( sizeof(std::vector<char>::value_type)*buffer.size() );
    
    if (number)
        readMemory( address, &buffer[0], bufferSize, phyAddr );

    return std::string( buffer.begin(), buffer.end() );
}

std::string loadChars( ULONG64 address, ULONG  number, bool phyAddr )
{
    return g_dbgClient->loadChars( address, number, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

std::wstring DebugClient::loadWChars( ULONG64 address, ULONG  number, bool phyAddr )
{
    std::vector<wchar_t>   buffer(number);

    ULONG  bufferSize = (ULONG)( sizeof(std::vector<wchar_t>::value_type)*buffer.size() );
    
    if (number)
        readMemory( address, &buffer[0], bufferSize, phyAddr );

    return std::wstring( buffer.begin(), buffer.end() );
}

std::wstring loadWChars( ULONG64 address, ULONG  number, bool phyAddr )
{
    return g_dbgClient->loadWChars( address, number, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

python::list DebugClient::loadBytes( ULONG64 offset, ULONG count, bool phyAddr )
{
    return loadArray<unsigned char>( offset, count, phyAddr );
}

python::list loadBytes( ULONG64 offset, ULONG count, bool phyAddr )
{
    return g_dbgClient->loadBytes( offset, count, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

python::list DebugClient::loadWords( ULONG64 offset, ULONG count, bool phyAddr )
{
    return loadArray<unsigned short>( offset, count, phyAddr );
}

python::list loadWords( ULONG64 offset, ULONG count, bool phyAddr )
{
    return g_dbgClient->loadWords( offset, count, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

python::list DebugClient::loadDWords( ULONG64 offset, ULONG count, bool phyAddr )
{
    return loadArray<unsigned long>( offset, count, phyAddr );
}

python::list loadDWords( ULONG64 offset, ULONG count, bool phyAddr )
{
    return g_dbgClient->loadDWords( offset, count, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

python::list DebugClient::loadQWords( ULONG64 offset, ULONG count, bool phyAddr )
{
    return loadArray<unsigned __int64>( offset, count, phyAddr );
}

python::list loadQWords( ULONG64 offset, ULONG count, bool phyAddr )
{
    return g_dbgClient->loadQWords( offset, count, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

python::list DebugClient::loadSignBytes( ULONG64 offset, ULONG count, bool phyAddr )
{
    return loadArray<char>( offset, count, phyAddr );
}

python::list loadSignBytes( ULONG64 offset, ULONG count, bool phyAddr )
{
    return g_dbgClient->loadSignBytes( offset, count, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

python::list DebugClient::loadSignWords( ULONG64 offset, ULONG count, bool phyAddr )
{
    return loadArray<short>( offset, count, phyAddr );
}

python::list loadSignWords( ULONG64 offset, ULONG count, bool phyAddr )
{
    return g_dbgClient->loadSignWords( offset, count, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

python::list DebugClient::loadSignDWords( ULONG64 offset, ULONG count, bool phyAddr )
{
    return loadArray<long>( offset, count, phyAddr );
}

python::list loadSignDWords( ULONG64 offset, ULONG count, bool phyAddr )
{
    return g_dbgClient->loadSignDWords( offset, count, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

python::list DebugClient::loadSignQWords( ULONG64 offset, ULONG count, bool phyAddr )
{
    return loadArray<__int64>( offset, count, phyAddr );
}

python::list loadSignQWords( ULONG64 offset, ULONG count, bool phyAddr )
{
    return g_dbgClient->loadSignQWords( offset, count, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG64 DebugClient::ptrByte( ULONG64 offset )
{
    unsigned char     val = 0;
    
    readMemory( offset, &val, sizeof(val), false );

    return val;
}

ULONG64 ptrByte( ULONG64 offset )
{
    return g_dbgClient->ptrByte( offset );
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG64 DebugClient::ptrWord( ULONG64 offset )
{
    unsigned short     val = 0;
    
    readMemory( offset, &val, sizeof(val), false );

    return val;
}

ULONG64 ptrWord( ULONG64 offset )
{
    return g_dbgClient->ptrWord( offset );
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG64 DebugClient::ptrDWord( ULONG64 offset )
{
    unsigned long     val = 0;
    
    readMemory( offset, &val, sizeof(val), false );

    return val;
}

ULONG64 ptrDWord( ULONG64 offset )
{
    return g_dbgClient->ptrDWord( offset );
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG64 DebugClient::ptrQWord( ULONG64 offset )
{
    unsigned __int64     val = 0;
    
    readMemory( offset, &val, sizeof(val), false );

    return val;
}

ULONG64 ptrQWord( ULONG64 offset )
{
    return g_dbgClient->ptrQWord( offset );
}

/////////////////////////////////////////////////////////////////////////////////////

ULONG64 DebugClient::ptrMWord( ULONG64 offset )
{
    return  ptrSize() == 8 ? ptrQWord( offset ) : ptrDWord(offset);
}

ULONG64 ptrMWord( ULONG64 offset )
{
    return g_dbgClient->ptrMWord( offset );
}

ULONG64 DebugClient::ptrPtr( ULONG64 offset )
{
    return ptrMWord( offset );
}

ULONG64 ptrPtr( ULONG64 offset )
{
    return  g_dbgClient->ptrMWord( offset );
}

/////////////////////////////////////////////////////////////////////////////////////

LONG64 DebugClient::ptrSignByte( ULONG64 offset )
{
    char     val = 0;
    
    readMemory( offset, &val, sizeof(val), false );

    return val;
}

LONG64 ptrSignByte( ULONG64 offset )
{
    return g_dbgClient->ptrSignByte( offset );
}

/////////////////////////////////////////////////////////////////////////////////////

LONG64 DebugClient::ptrSignWord( ULONG64 offset )
{
    short     val = 0;
    
    readMemory( offset, &val, sizeof(val), false );

    return val;
}

LONG64 ptrSignWord( ULONG64 offset )
{
    return g_dbgClient->ptrSignWord( offset );
}

/////////////////////////////////////////////////////////////////////////////////////

LONG64 DebugClient::ptrSignDWord( ULONG64 offset )
{
    long     val = 0;
    
    readMemory( offset, &val, sizeof(val), false );

    return val;
}

LONG64 ptrSignDWord( ULONG64 offset )
{
    return g_dbgClient->ptrSignDWord( offset );
}

/////////////////////////////////////////////////////////////////////////////////////

LONG64 DebugClient::ptrSignQWord( ULONG64 offset )
{
    __int64     val = 0;
    
    readMemory( offset, &val, sizeof(val), false );

    return val;
}

LONG64 ptrSignQWord( ULONG64 offset )
{
    return g_dbgClient->ptrSignQWord( offset );
}

/////////////////////////////////////////////////////////////////////////////////////

LONG64 DebugClient::ptrSignMWord( ULONG64 offset )
{
    return  ptrSize() == 8 ? ptrSignQWord( offset ) : ptrSignDWord(offset);
}

LONG64 ptrSignMWord( ULONG64 offset )
{
    return g_dbgClient->ptrSignMWord( offset );
}

/////////////////////////////////////////////////////////////////////////////////////

}; // end of pykd

