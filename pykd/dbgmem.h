#pragma once

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

ULONG64
addr64( ULONG64  addr );

///////////////////////////////////////////////////////////////////////////////////

void
readMemory( ULONG64 address, PVOID buffer, ULONG length, BOOLEAN phyAddr = FALSE );

void
writeMemory( ULONG64 address, PVOID buffer, ULONG length, BOOLEAN phyAddr = FALSE );

///////////////////////////////////////////////////////////////////////////////////

std::string loadChars( ULONG64 address, ULONG  number, bool phyAddr = FALSE );

std::wstring loadWChars( ULONG64 address, ULONG  number, bool phyAddr = FALSE );

python::list loadBytes( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

python::list loadWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

python::list loadDWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

python::list loadQWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

///////////////////////////////////////////////////////////////////////////////////

};









//#include <boost/scoped_array.hpp>
//
///////////////////////////////////////////////////////////////////////////////////
//
//void
//loadMemory( ULONG64 address, PVOID dest, ULONG length, BOOLEAN phyAddr = FALSE );
//
//ULONG64
//loadPtrByPtr( ULONG64 address );
//
//ULONG64
//loadMWord( ULONG64 address );
//
//LONG64
//loadSignMWord( ULONG64 address );
//
//
//template<typename T>
//boost::python::object
//loadArray( ULONG64 address, ULONG  number, BOOLEAN phyAddr = FALSE )
//{
//    boost::scoped_array<T> buffer(new T[number]);
//    
//    loadMemory( address, buffer.get(), number*sizeof(T), phyAddr );
//
//    boost::python::list    lst;
//    
//    for ( ULONG  i = 0; i < number; ++i )
//        lst.append( buffer[i] );
//            
//    return   lst;
//}
//
//boost::python::object
//loadChars( ULONG64 address, ULONG  number, BOOLEAN phyAddr = FALSE );
//
//boost::python::object
//loadWChars( ULONG64 address, ULONG  number, BOOLEAN phyAddr = FALSE );
//
//template<typename T>
//boost::python::object
//loadByPtr( ULONG64 address )
//{
//    T   value;
//    
//    loadMemory( address, &value, sizeof(T) );
//    
//    return boost::python::object( value );
//}
//
//template<>
//boost::python::object
//loadByPtr<char>( ULONG64 address );
//
//boost::python::object
//loadPtrArray( ULONG64 address, ULONG  number );
//
//boost::python::object
//loadUnicodeStr( ULONG64 address );
//
//boost::python::object
//loadAnsiStr( ULONG64 address );
//
//boost::python::object
//loadCStr( ULONG64 address ); 
//
//void
//loadCStrToBuffer( ULONG64 address, PCHAR buffer, ULONG  bufferLen );
//
//boost::python::object
//loadWStr( ULONG64 address ); 
//
//void
//loadWStrToBuffer( ULONG64 address, PWCHAR buffer, ULONG  bufferLen );
//
//bool
//compareMemory( ULONG64 addr1, ULONG64 addr2, ULONG length, BOOLEAN phyAddr = FALSE );
//
//ULONG64
//addr64( ULONG64  addr );
//
//boost::python::object
//loadLinkedList( ULONG64 address );
//
//bool
//isOffsetValid( ULONG64  addr );
//
///////////////////////////////////////////////////////////////////////////////////