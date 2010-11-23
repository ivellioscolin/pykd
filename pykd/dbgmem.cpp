#include "stdafx.h"

#include "dbgext.h"
#include "dbgexcept.h"
#include "dbgmem.h"
#include "dbgsystem.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////

bool
loadMemory( ULONG64 address, PVOID dest, ULONG length, BOOLEAN phyAddr  )
{
    address = addr64( address );

    try {
    
        if ( phyAddr == FALSE )
        {
            HRESULT     hres = dbgExt->dataSpaces->ReadVirtual( address, dest, length, NULL );
            if ( FAILED( hres ) )
               throw DbgException( "IDebugDataSpace::ReadVirtual  failed" );
               
        }
        else
        {
             HRESULT     hres = dbgExt->dataSpaces->ReadPhysical( address, dest, length, NULL );
             if ( FAILED( hres ) )
                throw DbgException( "IDebugDataSpace::ReadPhysical  failed" ); 
        }               
           
        return true;           
        
    } 
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	            
	
	return false;
}    

///////////////////////////////////////////////////////////////////////////////////

ULONG64
addr64( ULONG64  addr )
{
    if ( *( (ULONG*)&addr + 1 ) == 0 )
        return (ULONG64)(LONG)addr;
        
    return addr;        
}

///////////////////////////////////////////////////////////////////////////////////

bool
compareMemory( ULONG64 addr1, ULONG64 addr2, ULONG length, BOOLEAN phyAddr  )
{
    HRESULT     hres;
    bool        result = false;

    addr1 = addr64( addr1 );
    addr2 = addr64( addr2 );

    char*  m1 = new char[length];    
    char*  m2 = new char[length];        
   
    try {
    
        if ( phyAddr == FALSE )
        { 
    
            hres = dbgExt->dataSpaces->ReadVirtual( addr1, m1, length, NULL );
            if ( FAILED( hres ) )
               throw DbgException( "IDebugDataSpace::ReadVirtual  failed" );
           
            hres = dbgExt->dataSpaces->ReadVirtual( addr2, m2, length, NULL );
            if ( FAILED( hres ) )
                throw DbgException( "IDebugDataSpace::ReadVirtual  failed" );           
        }
        else
        {
            hres = dbgExt->dataSpaces->ReadPhysical( addr1, m1, length, NULL );
            if ( FAILED( hres ) )
               throw DbgException( "IDebugDataSpace::ReadPhysical  failed" );
           
            hres = dbgExt->dataSpaces->ReadPhysical( addr2, m2, length, NULL );
            if ( FAILED( hres ) )
                throw DbgException( "IDebugDataSpace::ReadPhysical  failed" );   
        }                
           
        result = memcmp( m1, m2, length ) == 0;     
        
    } 
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	 
	
    delete[] m1;
    delete[] m2;
    
    return result;
}

///////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadPtrArray( ULONG64 address, ULONG  number )
{
    if ( is64bitSystem() )
    {
        ULONG64   *buffer = new ULONG64[ number ];
        
        if ( loadMemory( address, buffer, number*sizeof(ULONG64) ) )
        {
            boost::python::dict    arr;
        
            for ( ULONG  i = 0; i < number; ++i )
                arr[i] = buffer[i];
                
            delete[]  buffer;            
            
            return   arr;
        }
       
        delete[]  buffer;
        
 	    return boost::python::object();    
    }
    else
    {
        ULONG   *buffer = new ULONG[ number ];
        
        if ( loadMemory( address, buffer, number*sizeof(ULONG) ) )
        {
            boost::python::dict    arr;
        
            for ( ULONG  i = 0; i < number; ++i )
                arr[i] = addr64( buffer[i] );
                
            delete[]  buffer;            
            
            return   arr;
        }
       
        delete[]  buffer;
        
 	    return boost::python::object();       
    }
}

///////////////////////////////////////////////////////////////////////////////////

ULONG64
loadPtrByPtr( ULONG64 address )
{
    ULONG64    value = 0;

    if ( is64bitSystem() )
        loadMemory( address, &value, sizeof(ULONG64) );
    else
    {
        if ( loadMemory( address, &value, sizeof(ULONG) ) )
              value = addr64( value );
    }
    
    return value;
}

///////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadUnicodeStr( ULONG64 address )
{
    USHORT   length;
    USHORT   maximumLength;
    ULONG64  buffer = 0;
    wchar_t  *str = NULL;
    
    do {
    
        if ( !loadMemory( address, &length, sizeof( length ) ) )
            break;
            
        if ( length == 0 )
            break;            
            
        address += sizeof( length );
            
        if ( !loadMemory( address, &maximumLength, sizeof( maximumLength ) ) )          
            break;
            
        address += sizeof( maximumLength );            
        
        if ( is64bitSystem() )
        {
            address += address % 8 ? ( 8 - address % 8 ) : 0 ;  // выравнивание на 8 байт
        
            if ( !loadMemory( address, &buffer, 8 ) )
                break;
                
            address += 8;                
        }
        else
        {
            address += address % 4 ? ( 4 - address % 4 ) : 0 ;  // выравнивание на 8 байт
        
            if ( !loadMemory( address, &buffer, 4 ) )
                break;        
                
            buffer = addr64( buffer ); 
            
            address += 4;               
        }    
        
        wchar_t  *str  = new wchar_t[ length/2 ];
        
        if ( !loadMemory( buffer, str, length ) )
            break;
            
        std::wstring    strValue(str, length/2);
        
        delete[] str; 
            
        return boost::python::object( strValue );
                    
    } while( FALSE );
    
    if ( str )
        delete[] str;
        
    return boost::python::object( "" );
    
}

///////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadAnsiStr( ULONG64 address )
{
    USHORT   length;
    USHORT   maximumLength;
    ULONG64  buffer = 0;
    char     *str = NULL;
    
    do {
    
        if ( !loadMemory( address, &length, sizeof( length ) ) )
            break;
            
        if ( length == 0 )
            break;            
            
        address += sizeof( length );
            
        if ( !loadMemory( address, &maximumLength, sizeof( maximumLength ) ) )          
            break;
           
        address += sizeof( maximumLength );
        
        if ( is64bitSystem() )
        {
            address += address % 8 ? ( 8 - address % 8 ) : 0;  // выравнивание на 8 байт
        
            if ( !loadMemory( address, &buffer, 8 ) )
                break;
                
            address += 8;                
        }
        else
        {
            address += address % 4 ? ( 4 - address % 4 ) : 0;  // выравнивание на 8 байт
        
            if ( !loadMemory( address, &buffer, 4 ) )
                break;        
                
            buffer = addr64( buffer ); 
            
            address += 4;               
        }    
        
       
        str = new char [ length ];        
        
        if ( !loadMemory( buffer, str, length ) )
            break;

        std::string     strVal ( str, length );
            
        delete[] str;
        
        return boost::python::object( strVal );
    
    } while( FALSE );
    
    if ( str )
        delete[] str;
    
    return boost::python::object( "" );
}

///////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadCStr( ULONG64 address )
{
    const   size_t              maxLength = 0x1000;
    boost::python::object       strObj( std::string("") );

    address = addr64( address );
    
    char*   buffer = new char[maxLength];

    try {
        
        HRESULT     hres = 
            dbgExt->dataSpaces4->ReadMultiByteStringVirtual(
                address,
                maxLength,
                buffer,
                maxLength,
                NULL );
        
        if ( FAILED( hres ) )
            throw DbgException( "IDebugDataSpace4::ReadMultiByteStringVirtual  failed" );
                               
        strObj = boost::python::object( std::string( buffer ) );
    
    } 
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	            
	
	delete[] buffer;
	
	return strObj;
}

///////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadWStr( ULONG64 address )
{
    const   size_t              maxLength = 0x1000;
    boost::python::object       strObj( std::wstring(L"") );

    address = addr64( address );
    
    wchar_t*   buffer = new wchar_t[maxLength];

    try {
        
        HRESULT     hres = 
            dbgExt->dataSpaces4->ReadUnicodeStringVirtualWide(
                address,
                maxLength*sizeof(wchar_t),
                buffer,
                maxLength,
                NULL );
        
        if ( FAILED( hres ) )
            throw DbgException( "IDebugDataSpace4::ReadUnicodeStringVirtualWide  failed" );
                               
        strObj = boost::python::object( std::wstring(buffer) );
    
    } 
	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}	            
	
	delete[] buffer;
	
	return strObj;
}

///////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadLinkedList( ULONG64 address )
{
    ULONG64     entryAddress = 0;
    
    boost::python::list    objList;
    
    for( entryAddress = loadPtrByPtr( address ); entryAddress != address; entryAddress = loadPtrByPtr( entryAddress ) )
        objList.append( entryAddress );
    
    return objList;
}

///////////////////////////////////////////////////////////////////////////////////