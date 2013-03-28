#pragma once

#include "dbgengine.h"
#include "dbgexcept.h"
#include "pyaux.h"
#include "eventhandler.h"

#include <dbgeng.h>
#include <dbghelp.h>

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

class DebugEngine : private DebugBaseEventCallbacks, private IDebugInputCallbacks
{
public:

    struct DbgEngBind {

        CComQIPtr<IDebugClient4>  client;
        CComQIPtr<IDebugControl4>  control;
        CComQIPtr<IDebugSystemObjects2>  system;
        CComQIPtr<IDebugSymbols3>  symbols;
        CComQIPtr<IDebugDataSpaces4>  dataspace;
        CComQIPtr<IDebugAdvanced2>  advanced;
        CComQIPtr<IDebugRegisters2>  registers;

        DbgEngBind( PDEBUG_CLIENT4 client_, PDEBUG_EVENT_CALLBACKS callback )
        {
            client = client_;
            control = client_;
            system = client_;
            symbols = client_;
            dataspace = client_;
            advanced = client_;
            registers = client_;

            client->SetEventCallbacks( callback );
        }

        PyThreadStateSaver     pystate;
    };

    // IUnknown impls
    // IUnknown.
    STDMETHOD(QueryInterface)(
        __in REFIID InterfaceId,
        __out PVOID* Interface
        )
    {
        *Interface = NULL;
        if (IsEqualIID(InterfaceId, __uuidof(IUnknown)) ||
            IsEqualIID(InterfaceId, __uuidof(IDebugEventCallbacks)) ||
            IsEqualIID(InterfaceId,__uuidof(IDebugInputCallbacks)) 
            )
        {
            *Interface = this;
            return S_OK;
        }
        else
        {
            return E_NOINTERFACE;
        }
    }


    STDMETHOD_(ULONG, AddRef)() { return 1; }
    STDMETHOD_(ULONG, Release)() { return 1; }

    // IDebugEventCallbacks impls
    STDMETHOD(GetInterestMask)(
        __out PULONG Mask 
        )
    {
        *Mask = DEBUG_EVENT_BREAKPOINT;
        *Mask |= DEBUG_EVENT_LOAD_MODULE;
        *Mask |= DEBUG_EVENT_UNLOAD_MODULE;
        *Mask |= DEBUG_EVENT_EXCEPTION;
        *Mask |= DEBUG_EVENT_CHANGE_ENGINE_STATE;
        return S_OK;
    }

    STDMETHOD(Breakpoint)(
        __in IDebugBreakpoint *bp
    );

    STDMETHOD(LoadModule)(
        __in ULONG64 ImageFileHandle,
        __in ULONG64 BaseOffset,
        __in ULONG ModuleSize,
        __in_opt PCSTR ModuleName,
        __in_opt PCSTR ImageName,
        __in ULONG CheckSum,
        __in ULONG TimeDateStamp);

    STDMETHOD(UnloadModule)(
        __in_opt PCSTR ImageBaseName,
        __in ULONG64 BaseOffset );

    STDMETHOD(Exception)(
        __in PEXCEPTION_RECORD64 Exception,
        __in  ULONG FirstChance );

    STDMETHOD(ChangeEngineState)(
        __in ULONG Flags,
        __in ULONG64 Argument );


    STDMETHOD(StartInput)(
        __in ULONG BufferSize );

    STDMETHOD(EndInput)();

    DbgEngBind* operator->();

    void registerCallbacks( const DEBUG_EVENT_CALLBACK *callbacks );
    void removeCallbacks( const DEBUG_EVENT_CALLBACK *callbacks );

    DebugEngine() :
        previousExecutionStatus( DebugStatusNoChange )
        {}

private:

    std::auto_ptr<DbgEngBind>    m_bind;

    struct DebugEventContext 
    {
        DEBUG_EVENT_CALLBACK*       callback;
        PyThreadStateSaver          pystate;

        DebugEventContext( const DEBUG_EVENT_CALLBACK* callback_, PyThreadStateSaver &pystate_ ) :
            callback( const_cast<DEBUG_EVENT_CALLBACK*>(callback_) ),
            pystate(pystate_)
            {}
    };

    typedef std::list<DebugEventContext>  HandlerList;

    boost::recursive_mutex       m_handlerLock;
    HandlerList  m_handlers;

    ULONG   previousExecutionStatus;
};

///////////////////////////////////////////////////////////////////////////////////

extern  DebugEngine  g_dbgEng;

/////////////////////////////////////////////////////////////////////////////////

};