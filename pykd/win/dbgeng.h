#pragma once

#include "dbgengine.h"
#include "dbgexcept.h"
#include "pyaux.h"
#include "eventhandler.h"

#include <dbgeng.h>
#include <dbghelp.h>

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

class DebugEngine {

public:

    struct DbgEngBind {

        CComQIPtr<IDebugClient4>  client;
        CComQIPtr<IDebugControl4>  control;
        CComQIPtr<IDebugSystemObjects2>  system;
        CComQIPtr<IDebugSymbols3>  symbols;
        CComQIPtr<IDebugDataSpaces4>  dataspace;
        CComQIPtr<IDebugAdvanced2>  advanced;
        CComQIPtr<IDebugRegisters2>  registers;

        DbgEngBind( PDEBUG_CLIENT4 c )
        {
            client = c;
            control = c;
            system = c;
            symbols = c;
            dataspace = c;
            advanced = c;
            registers = c;
        }

        PyThreadStateSaver     pystate;
    };

    class DbgEventCallbacks : public DebugBaseEventCallbacks 
    {
        // IUnknown impls
        STDMETHOD_(ULONG, AddRef)() { return 1; }
        STDMETHOD_(ULONG, Release)() { return 1; }

        // IDebugEventCallbacks impls
        STDMETHOD(GetInterestMask)(
            __out PULONG Mask 
            )
        {
            *Mask = DEBUG_EVENT_BREAKPOINT;
            return S_OK;
        }

        STDMETHOD(Breakpoint)(
            __in IDebugBreakpoint *bp
        );
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

    void registerCallbacks( const DEBUG_EVENT_CALLBACK *callbacks );
    void removeCallbacks();
    const DEBUG_EVENT_CALLBACK* getCallbacks() const {
        return m_eventCallbacks;
    }

    DebugEngine() :
        m_callbacks()
    {
        g_eventHandler = new EventHandler();
    }

    ~DebugEngine()
    {
        delete g_eventHandler;
        g_eventHandler = NULL;
    }

private:

    std::auto_ptr<DbgEngBind>    m_bind;

    DbgEventCallbacks            m_callbacks;

    const DEBUG_EVENT_CALLBACK   *m_eventCallbacks;
};

///////////////////////////////////////////////////////////////////////////////////

extern  DebugEngine  g_dbgEng;

/////////////////////////////////////////////////////////////////////////////////

};