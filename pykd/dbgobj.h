#pragma once

#include <dbgeng.h>
#include "dbgexcept.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

class DbgObject {

protected:

    DbgObject( IDebugClient5 *client ) {

        HRESULT    hres;
        hres = client->QueryInterface( __uuidof(IDebugClient5), (void **)&m_client );
        if ( FAILED( hres ) )
            throw DbgException("DebugCreate failed");

        hres = client->QueryInterface( __uuidof(IDebugControl4), (void**)&m_control );
        if ( FAILED( hres ) )
            throw DbgException("QueryInterface IDebugControl4  failed");    

        hres = client->QueryInterface( __uuidof(IDebugSymbols3), (void**)&m_symbols );
        if ( FAILED( hres ) )
            throw DbgException("QueryInterface IDebugSymbols3  failed");  
    }
    
    virtual ~DbgObject() {};

    CComPtr<IDebugClient5>      m_client;     
    CComPtr<IDebugControl4>     m_control;
    CComPtr<IDebugSymbols3>     m_symbols;
};



///////////////////////////////////////////////////////////////////////////////////

}; // end of namespace pykd