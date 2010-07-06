#pragma once

#include <string>

#include <boost/python.hpp>
#include <boost/python/object.hpp>

/////////////////////////////////////////////////////////////////////////////////

class dbgModuleClass {

public:

    dbgModuleClass() :
        m_base( 0 ),
        m_end( 0 )     
    {}           

    dbgModuleClass( ULONG64 base, ULONG size ) :
        m_base( base ),
        m_end( base + size )
    {}    
    
    ULONG64
    getBegin() const {
        return m_base;
    }      
    
    ULONG64
    getEnd() const {
        return m_end;
    }
    
    bool
    contain( ULONG64  addr ) const {
        if ( *( (ULONG*)&addr + 1 ) == 0 )
            *( (ULONG*)&addr + 1 ) = 0xFFFFFFFF;
                          
        return m_base <= addr && addr <= m_end;
    }
    
    
private:

    ULONG64    m_base;
    ULONG64    m_end;        
};

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadModule( const std::string &moduleName );


boost::python::object
findModule( ULONG64 addr );

/////////////////////////////////////////////////////////////////////////////////