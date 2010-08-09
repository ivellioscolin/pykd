#pragma once

#include <string>

#include <boost/python.hpp>
#include <boost/python/object.hpp>

/////////////////////////////////////////////////////////////////////////////////

std::string
dbgCommand( const std::string &command );

/////////////////////////////////////////////////////////////////////////////////

class dbgExtensionClass {

public:

    dbgExtensionClass() :
        m_handle( NULL )
        {}
    
    dbgExtensionClass( const char* path );
    
    ~dbgExtensionClass();
    
    std::string
    call( const std::string &command, const std::string param );
    
private:

    ULONG64         m_handle;    
};

/////////////////////////////////////////////////////////////////////////////////

