#pragma once

#include <string>

#include <boost/python.hpp>
#include <boost/python/object.hpp>

/////////////////////////////////////////////////////////////////////////////////

class typedVarClass {

public:

    typedVarClass()
    {}
    
    typedVarClass( ULONG64 addr, ULONG size ) :
        m_addr( addr ), 
        m_size( size )
    {}
    
    ULONG64
    getAddress() const {
        return m_addr;
    }
    
    ULONG
    size() const {
        return m_size;
    }
    
private:

    ULONG64     m_addr;
    
    ULONG       m_size;
        
};

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadTypedVar( const std::string &moduleName, const std::string &typeName, ULONG64 address );

boost::python::object
loadTypedVarList( ULONG64 address, const std::string &moduleName, const std::string &typeName, const std::string &listEntryName );

boost::python::object
containingRecord( ULONG64 address, const std::string &moduleName, const std::string &typeName, const std::string &fieldName );

ULONG
sizeofType( const std::string &moduleName, const std::string &typeName );


/////////////////////////////////////////////////////////////////////////////////