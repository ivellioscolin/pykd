#pragma once

#include <boost/python.hpp>
#include <boost/python/object.hpp>


/////////////////////////////////////////////////////////////////////////////////

bool
loadMemory( ULONG64 address, PVOID dest, ULONG length, BOOLEAN phyAddr = FALSE );

ULONG64
loadPtrByPtr( ULONG64 address );

ULONG64
loadMWord( ULONG64 address );

LONG64
loadSignMWord( ULONG64 address );


template<typename T>
boost::python::object
loadArray( ULONG64 address, ULONG  number, BOOLEAN phyAddr = FALSE )
{
    T   *buffer = new T[ number ];
    
    if ( loadMemory( address, buffer, number*sizeof(T), phyAddr ) )
    {
        boost::python::list    lst;
    
        for ( ULONG  i = 0; i < number; ++i )
            lst.append( buffer[i] );
            //arr[i] = buffer[i];
            
        delete[]  buffer;            
        
        return   lst;
    }
   
    delete[]  buffer;
    
 	return boost::python::object();
}


template<typename T>
boost::python::object
loadByPtr( ULONG64 address )
{
    T   value;
    
    if ( loadMemory( address, &value, sizeof(T) ) )
    {
        return boost::python::object( value );
    }    
    
    return boost::python::object();
}

template<>
boost::python::object
loadByPtr<char>( ULONG64 address );

boost::python::object
loadPtrArray( ULONG64 address, ULONG  number );

boost::python::object
loadUnicodeStr( ULONG64 address );

boost::python::object
loadAnsiStr( ULONG64 address );

boost::python::object
loadCStr( ULONG64 address ); 

bool
loadCStrToBuffer( ULONG64 address, PCHAR buffer, ULONG  bufferLen );

boost::python::object
loadWStr( ULONG64 address ); 

bool
loadWStrToBuffer( ULONG64 address, PWCHAR buffer, ULONG  bufferLen );

bool
compareMemory( ULONG64 addr1, ULONG64 addr2, ULONG length, BOOLEAN phyAddr = FALSE );

ULONG64
addr64( ULONG64  addr );

boost::python::object
loadLinkedList( ULONG64 address );

bool
isOffsetValid( ULONG64  addr );

/////////////////////////////////////////////////////////////////////////////////