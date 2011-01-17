#pragma once

#include <string>
#include <map>

#include <boost/python.hpp>
#include <boost/python/object.hpp>

/////////////////////////////////////////////////////////////////////////////////

class dbgModuleClass {

public:

    dbgModuleClass() :
        m_base( 0 ),
        m_end( 0 )     
    {}           
    
    dbgModuleClass( const std::string &name, ULONG64 base, ULONG size ) :
        m_name( name ),
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
    
    std::string
    getName() const {
        return m_name;    
    }    
    
    void
    reloadSymbols();
    
    ULONG64
    getOffset( const std::string  &symName );
    
    
private:

    ULONG64         m_base;
    
    ULONG64         m_end;        
    
    std::string     m_name;
    
    typedef std::map<std::string, ULONG64>  OffsetMap;
    OffsetMap       m_offsets;
};

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
loadModule( const std::string &moduleName );


boost::python::object
findModule( ULONG64 addr );

/////////////////////////////////////////////////////////////////////////////////