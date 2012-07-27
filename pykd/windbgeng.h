#pragma once

#include "dbgengine.h"
#include "dbgexcept.h"
#include "pyaux.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

class DebugEngine {

public:

    struct DbgEngBind {

        CComQIPtr<IDebugClient4>  client;
        CComQIPtr<IDebugControl4>  control;
        CComQIPtr<IDebugSystemObjects2>  system;
        CComQIPtr<IDebugSymbols3>  symbols;

        DbgEngBind( PDEBUG_CLIENT4 c )
        {
            client = c;
            control = c;
            system = c;
            symbols = c;
        }

        PyThreadStateSaver     pystate;
    };

    DbgEngBind*
    operator->() 
    {
        if ( m_bind.get() != NULL )
            return m_bind.get();

        CComPtr<IDebugClient4>   client = NULL;

        HRESULT  hres = DebugCreate( __uuidof(IDebugClient4), (void **)&client );
        if ( FAILED( hres ) )
            throw DbgException("DebugCreate failed");

        m_bind.reset(new DbgEngBind(client) );

        return m_bind.get();
    }

private:

    std::auto_ptr<DbgEngBind>    m_bind;

};

///////////////////////////////////////////////////////////////////////////////////

extern  DebugEngine     g_dbgEng;

///////////////////////////////////////////////////////////////////////////////////

};