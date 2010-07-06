#pragma once

#include <string>

#include <boost/python.hpp>
#include <boost/python/object.hpp>

/////////////////////////////////////////////////////////////////////////////////

class typedVarClass {

public:

    typedVarClass()
    {}
    
    typedVarClass( ULONG64 addr ) :
        m_addr( addr )
    {}
    
    ULONG64
    getAddress() const {
        return m_addr;
    }
    
private:

    ULONG64     m_addr;
        
};

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadTypedVar( const std::string &moduleName, const std::string &typeName, ULONG64 address );

boost::python::object
containingRecord( ULONG64 address, const std::string &moduleName, const std::string &typeName, const std::string &fieldName );


/////////////////////////////////////////////////////////////////////////////////