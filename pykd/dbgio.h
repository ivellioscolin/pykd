#pragma once

#include "dbgobj.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

void dprint( const std::wstring &str, bool dml = false );

void dprintln( const std::wstring &str, bool dml = false );

void eprint( const std::wstring &str );

void eprintln( const std::wstring &str );

/////////////////////////////////////////////////////////////////////////////////

class DbgOut : private DbgObject {

public:

    DbgOut( IDebugClient5 *client ) : DbgObject( client )
    {}

    void
    write( const std::wstring  &str );    
};

/////////////////////////////////////////////////////////////////////////////////

class DbgIn : private DbgObject {

public:

    DbgIn( IDebugClient5 *client ) : DbgObject( client )
    {}

    std::string
    readline();
};

/////////////////////////////////////////////////////////////////////////////////

}; 