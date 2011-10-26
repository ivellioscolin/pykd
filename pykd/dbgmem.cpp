#include "stdafx.h"

#include "dbgclient.h"
#include "dbgexcept.h"

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////////

template<typename T>
python::list
DebugClient::loadArray( ULONG64 offset, ULONG count, bool phyAddr )
{
    std::vector<T>   buffer(count);

    if (count)
        readMemory( offset, &buffer[0], count*sizeof(T), phyAddr );

    python::list        lst;

    for( ULONG  i = 0; i < count; ++i )
        lst.append( buffer[i] );

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

std::string DebugClient::loadChars( ULONG64 address, ULONG  number, bool phyAddr )
{
    std::vector<char>   buffer(number);

    ULONG  bufferSize = sizeof(std::vector<char>::value_type)*buffer.size();
    
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

    ULONG  bufferSize = sizeof(std::vector<wchar_t>::value_type)*buffer.size();
    
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


}; // end of pykd




























//#include <boost/scoped_array.hpp>
//
//#include "dbgext.h"
//#include "dbgexcept.h"
//#include "dbgmem.h"
//#include "dbgsystem.h"
//#include "dbgio.h"
//
//using namespace std;
//
/////////////////////////////////////////////////////////////////////////////////////
//
//void
//loadMemory( ULONG64 address, PVOID dest, ULONG length, BOOLEAN phyAddr  )
//{
//    address = addr64( address );
//    
//    HRESULT     hres;
//
//    if ( phyAddr == FALSE )
//    {
//        hres = dbgExt->dataSpaces->ReadVirtual( address, dest, length, NULL );
//    }        
//    else
//    {
//        hres = dbgExt->dataSpaces->ReadPhysical( address, dest, length, NULL );
//    }               
//    
//    if ( FAILED( hres ) )
//        throw MemoryException( address, !!phyAddr );
//}    
//
/////////////////////////////////////////////////////////////////////////////////////
//
//ULONG64
//addr64( ULONG64  addr)
//{
//    HRESULT     hres;
//
//    try {
//
//        ULONG   processorMode;
//        hres = dbgExt->control->GetActualProcessorType( &processorMode );
//        if ( FAILED( hres ) )
//            throw DbgException( "IDebugControl::GetEffectiveProcessorType  failed" );
//
//        switch( processorMode )
//        {
//        case IMAGE_FILE_MACHINE_I386:
//            if ( *( (ULONG*)&addr + 1 ) == 0 )
//                return (ULONG64)(LONG)addr;
//
//        case IMAGE_FILE_MACHINE_AMD64:
//            break;
//
//        default:
//            throw DbgException( "Unknown processor type" );
//            break;
//        }
//    }
//    catch( std::exception  &e )
//    {
//        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
//    }
//    catch(...)
//    {
//        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
//    }
//
//    return addr;
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//bool
//compareMemory( ULONG64 addr1, ULONG64 addr2, ULONG length, BOOLEAN phyAddr  )
//{
//    HRESULT     hres;
//    bool        result = false;
//
//    addr1 = addr64( addr1 );
//    addr2 = addr64( addr2 );
//
//    boost::scoped_array<char> m1(new char[length]);
//    boost::scoped_array<char> m2(new char[length]);
//   
//    try {
//    
//        if ( phyAddr == FALSE )
//        { 
//    
//            hres = dbgExt->dataSpaces->ReadVirtual( addr1, m1.get(), length, NULL );
//            if ( FAILED( hres ) )
//               throw DbgException( "IDebugDataSpace::ReadVirtual  failed" );
//           
//            hres = dbgExt->dataSpaces->ReadVirtual( addr2, m2.get(), length, NULL );
//            if ( FAILED( hres ) )
//                throw DbgException( "IDebugDataSpace::ReadVirtual  failed" );           
//        }
//        else
//        {
//            hres = dbgExt->dataSpaces->ReadPhysical( addr1, m1.get(), length, NULL );
//            if ( FAILED( hres ) )
//               throw DbgException( "IDebugDataSpace::ReadPhysical  failed" );
//           
//            hres = dbgExt->dataSpaces->ReadPhysical( addr2, m2.get(), length, NULL );
//            if ( FAILED( hres ) )
//                throw DbgException( "IDebugDataSpace::ReadPhysical  failed" );   
//        }                
//           
//        result = memcmp( m1.get(), m2.get(), length ) == 0;     
//        
//    } 
//	catch( std::exception  &e )
//	{
//		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
//	}
//	catch(...)
//	{
//		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
//	}	 
//	
//    return result;
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//boost::python::object
//loadPtrArray( ULONG64 address, ULONG  number )
//{
//    if ( is64bitSystem() )
//    {
//		boost::scoped_array<ULONG64> buffer(new ULONG64[number]);
//        
//        loadMemory( address, buffer.get(), number*sizeof(ULONG64) );
//
//        boost::python::list    lst;
//        
//        for ( ULONG  i = 0; i < number; ++i )
//             lst.append( buffer[i] );
//             
//        return   lst;
//    }
//    else
//    {
//		boost::scoped_array<ULONG> buffer(new ULONG[number]);
//        
//        loadMemory( address, buffer.get(), number*sizeof(ULONG) );
//        
//        boost::python::list    lst;
//        
//        for ( ULONG  i = 0; i < number; ++i )
//            lst.append( addr64( buffer[i] ) );
//                
//        return   lst;
//    }
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//boost::python::object
//loadChars( ULONG64 address, ULONG  number, BOOLEAN phyAddr  )
//{
//    std::vector<char>   buffer(number);
//
//    if (number)
//        loadMemory( address, &buffer[0], (ULONG)buffer.size(), phyAddr );
//
//    return boost::python::object(std::string( buffer.begin(), buffer.end() ) );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//boost::python::object
//loadWChars( ULONG64 address, ULONG  number, BOOLEAN phyAddr  )
//{
//    std::vector<wchar_t>   buffer(number);
//    
//    loadMemory( address, &buffer[0], (ULONG)buffer.size(), phyAddr );
//
//    return boost::python::object(std::wstring( buffer.begin(), buffer.end() ) );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//ULONG64
//loadPtrByPtr( ULONG64 address )
//{
//    ULONG64    value = 0;
//
//    if ( is64bitSystem() )
//        loadMemory( address, &value, sizeof(ULONG64) );
//    else
//    {
//        loadMemory( address, &value, sizeof(ULONG) );
//
//        value = addr64( value );
//    }
//    
//    return value;
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//ULONG64
//loadMWord( ULONG64 address )
//{
//    ULONG64    value = 0;
//
//    if ( is64bitSystem() )
//        loadMemory( address, &value, sizeof(ULONG64) );
//    else
//        loadMemory( address, &value, sizeof(ULONG) );
//    
//    return value;
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//LONG64
//loadSignMWord( ULONG64 address )
//{
//    LONG64    value = 0;
//
//    if ( is64bitSystem() )
//        loadMemory( address, &value, sizeof(LONG64) );
//    else
//        loadMemory( address, &value, sizeof(LONG) );
//    
//    return value;
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//template<>
//boost::python::object
//loadByPtr<char>( ULONG64 address )
//{
//    char   value;
//    
//    loadMemory( address, &value, sizeof(char) );
//    
//    return boost::python::object( (int)value );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//boost::python::object
//loadUnicodeStr( ULONG64 address )
//{
//    USHORT   length;
//    USHORT   maximumLength;
//    ULONG64  buffer = 0;
//    
//    do {
//    
//        loadMemory( address, &length, sizeof( length ) );
//            
//        if ( length == 0 )
//            break;            
//            
//        address += sizeof( length );
//            
//        loadMemory( address, &maximumLength, sizeof( maximumLength ) );
//            
//        address += sizeof( maximumLength );            
//        
//        if ( is64bitSystem() )
//        {
//            address += address % 8 ? ( 8 - address % 8 ) : 0 ;  // выравнивание на 8 байт
//        
//            loadMemory( address, &buffer, 8 );
//                
//            address += 8;                
//        }
//        else
//        {
//            address += address % 4 ? ( 4 - address % 4 ) : 0 ;  // выравнивание на 8 байт
//        
//            loadMemory( address, &buffer, 4 );
//                
//            buffer = addr64( buffer ); 
//            
//            address += 4;               
//        }    
//        
//	    std::vector<wchar_t> str(length / 2);
//        
//        loadMemory( buffer, &str[0], length );
//            
//        std::wstring    strValue(&str[0], length/2);
//        
//        return boost::python::object( strValue );
//                    
//    } while( FALSE );
//    
//    return boost::python::object( "" );
//    
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//boost::python::object
//loadAnsiStr( ULONG64 address )
//{
//    USHORT   length;
//    USHORT   maximumLength;
//    ULONG64  buffer = 0;
//    
//    do {
//    
//        loadMemory( address, &length, sizeof( length ) );
//            
//        if ( length == 0 )
//            break;            
//            
//        address += sizeof( length );
//            
//        loadMemory( address, &maximumLength, sizeof( maximumLength ) );
//           
//        address += sizeof( maximumLength );
//        
//        if ( is64bitSystem() )
//        {
//            address += address % 8 ? ( 8 - address % 8 ) : 0;  // выравнивание на 8 байт
//        
//            loadMemory( address, &buffer, 8 );
//                
//            address += 8;                
//        }
//        else
//        {
//            address += address % 4 ? ( 4 - address % 4 ) : 0;  // выравнивание на 8 байт
//        
//            loadMemory( address, &buffer, 4 );
//                
//            buffer = addr64( buffer ); 
//            
//            address += 4;               
//        }    
//        
//       
//	    std::vector<char> str(length);
//        
//        loadMemory( buffer, &str[0], length );
//
//        std::string     strVal ( &str[0], length );
//            
//        return boost::python::object( strVal );
//    
//    } while( FALSE );
//    
//    return boost::python::object( "" );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//void
//loadCStrToBuffer( ULONG64 address, PCHAR buffer, ULONG  bufferLen )
//{
//    address = addr64( address );
//        
//    HRESULT     hres = 
//        dbgExt->dataSpaces4->ReadMultiByteStringVirtual(
//            address,
//            bufferLen,
//            buffer,
//            bufferLen/sizeof(CHAR),
//            NULL );
//    
//    if ( FAILED( hres ) )
//        throw MemoryException( address );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//void
//loadWStrToBuffer( ULONG64 address, PWCHAR buffer, ULONG  bufferLen )
//{
//    address = addr64( address );
//        
//    HRESULT     hres = 
//        dbgExt->dataSpaces4->ReadUnicodeStringVirtualWide(
//            address,
//            bufferLen,
//            buffer,
//            bufferLen/sizeof(WCHAR),
//            NULL );
//    
//    if ( FAILED( hres ) )
//        throw MemoryException( address );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//boost::python::object
//loadCStr( ULONG64 address )
//{
//    const   size_t              maxLength = 0x1000;
// 
//    address = addr64( address );
//    
//    boost::scoped_array<char> buffer(new char[maxLength]);
//        
//    HRESULT     hres = 
//        dbgExt->dataSpaces4->ReadMultiByteStringVirtual(
//            address,
//            maxLength,
//            buffer.get(),
//            maxLength,
//            NULL );
//    
//    if ( FAILED( hres ) )
//        throw MemoryException( address );
//                           
//    return boost::python::object( std::string( buffer.get() ) );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//boost::python::object
//loadWStr( ULONG64 address )
//{
//    const   size_t              maxLength = 0x1000;
//
//    address = addr64( address );
//    
//    boost::scoped_array<wchar_t> buffer(new wchar_t[maxLength]);
//        
//    HRESULT     hres = 
//        dbgExt->dataSpaces4->ReadUnicodeStringVirtualWide(
//            address,
//            maxLength*sizeof(wchar_t),
//            buffer.get(),
//            maxLength,
//            NULL );
//        
//    if ( FAILED( hres ) )
//        throw MemoryException( address );
//                               
//    return  boost::python::object( std::wstring(buffer.get()) );
//}
//
//
/////////////////////////////////////////////////////////////////////////////////////
//
//boost::python::object
//loadLinkedList( ULONG64 address )
//{
//    ULONG64     entryAddress = 0;
//    
//    boost::python::list    objList;
//    
//    for( entryAddress = loadPtrByPtr( address ); entryAddress != address; entryAddress = loadPtrByPtr( entryAddress ) )
//        objList.append( entryAddress );
//    
//    return objList;
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//bool
//isOffsetValid( ULONG64  addr )
//{
//    HRESULT     hres;
//
//    try {
//
//        // нужно подавить возможный вывод в консоль об отсутствующей странице памяти
//        OutputReader outputReader( dbgExt->client );    
//
//        ULONG       offsetInfo;
//        
//        hres = 
//            dbgExt->dataSpaces4->GetOffsetInformation(
//                DEBUG_DATA_SPACE_VIRTUAL,
//                DEBUG_OFFSINFO_VIRTUAL_SOURCE,
//                addr,
//                &offsetInfo,
//                sizeof( offsetInfo ),
//                NULL );
//
//        if ( FAILED( hres ) )
//            throw DbgException( "IDebugDataSpace4::GetOffsetInformation  failed" );
//
//        return  offsetInfo != DEBUG_VSOURCE_INVALID;
//    
//    }
//    catch( std::exception  &e )
//    {
//        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
//    }
//    catch(...)
//    {
//        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
//    }
//
//    return false;
//}
//
/////////////////////////////////////////////////////////////////////////////////////