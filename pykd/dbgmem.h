#pragma once

#include <boost/python.hpp>
#include <boost/python/object.hpp>


/////////////////////////////////////////////////////////////////////////////////

bool
loadMemory( ULONG64 address, PVOID dest, ULONG length );

template<typename T>
boost::python::object
loadArray( ULONG64 address, ULONG  number )
{
    T   *buffer = new T[ number ];
    
    if ( loadMemory( address, buffer, number*sizeof(T) ) )
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

boost::python::object
loadPtrByPtr( ULONG64 address );

boost::python::object
loadPtrArray( ULONG64 address, ULONG  number );

boost::python::object
loadUnicodeStr( ULONG64 address );

boost::python::object
loadAnsiStr( ULONG64 address );

bool
compareMemory( ULONG64 addr1, ULONG64 addr2, ULONG length );

ULONG64
addr64( ULONG64  addr );

/////////////////////////////////////////////////////////////////////////////////