////////////////////////////////////////////////////////////////////////////////
// User-customizing debug event handler
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"

#include "dbgobj.h"
#include "module.h"
#include "dbgclient.h"

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

class EventHandler : public DebugBaseEventCallbacks
{
public:

    EventHandler();

    EventHandler( DebugClientPtr  &client );

    virtual ~EventHandler();

protected:

    STDMETHOD_(ULONG, AddRef)() { return 1; }
    STDMETHOD_(ULONG, Release)() { return 1; }

    STDMETHOD( GetInterestMask ) (
       __out PULONG Mask
    );

    STDMETHOD(Breakpoint)(
        __in PDEBUG_BREAKPOINT Bp
    );

    STDMETHOD(Exception)(
        __in PEXCEPTION_RECORD64 Exception,
        __in ULONG FirstChance
    );

    STDMETHOD(LoadModule)(
        __in ULONG64 ImageFileHandle,
        __in ULONG64 BaseOffset,
        __in ULONG ModuleSize,
        __in PCSTR ModuleName,
        __in PCSTR ImageName,
        __in ULONG CheckSum,
        __in ULONG TimeDateStamp
    );

    STDMETHOD(UnloadModule)(
        __in PCSTR ImageBaseName,
        __in ULONG64 BaseOffset
    );  

    STDMETHOD(SessionStatus)(
        __in ULONG  Status
    );

    STDMETHOD(ChangeDebuggeeState)(
        __in ULONG  Flags,
        __in ULONG64 Argument );


protected:

    virtual ULONG onBreakpoint(ULONG Id) = 0;

    virtual ULONG onException(const python::dict &/*exceptData*/) = 0;

    virtual ULONG onLoadModule(const ModulePtr &/* module */)  = 0;

    virtual ULONG onUnloadModule(ULONG64 /*modBase*/)  = 0;
    
    virtual ULONG onChangeSessionStatus( ULONG status ) = 0;
    
    virtual ULONG onChangeDebugeeState() = 0;

protected:

    CComPtr<IDebugClient>       m_handlerClient;

    DebugClientPtr              m_parentClient;
};

////////////////////////////////////////////////////////////////////////////////

class EventHandlerWrap : public python::wrapper<EventHandler>, public EventHandler
{

public:

    EventHandlerWrap()
    {}

    EventHandlerWrap( DebugClientPtr  &client ) : EventHandler( client )
    {}

    ULONG onBreakpoint(ULONG Id) {
        return handler("onBreakpoint", Id);
    }

    ULONG onException(const python::dict &exceptData) {
        return handler<const python::dict&>("onException", exceptData);
    }

    ULONG onLoadModule(const ModulePtr &module) {
        return handler<const ModulePtr&>("onLoadModule", module );
    }

    ULONG onUnloadModule(ULONG64 modBase) {
        return handler<ULONG64>("onUnloadModule", modBase );
    }

    ULONG onChangeSessionStatus( ULONG status ) {
        return handler( "onChangeSessionStatus", status );
    }

    ULONG onChangeDebugeeState() {
        return handler( "onChangeDebugeeState" );
    }

    void onHandlerException();

private:

    template<typename Arg1Type>
    ULONG handler( const char* handlerName, Arg1Type  arg1 )
    {
        if (python::override pythonHandler = get_override( handlerName ))
        {
            try {
                return pythonHandler(arg1);
            }
            catch (const python::error_already_set &)  {
                onHandlerException();
            }
        }
        return DEBUG_STATUS_NO_CHANGE;
    }

    ULONG handler( const char* handlerName )
    {
        if (python::override pythonHandler = get_override( handlerName ))
        {
            try {
                return pythonHandler();
            }
            catch (const python::error_already_set &) {
                onHandlerException();
            }
        }

        return DEBUG_STATUS_NO_CHANGE;
    }
}; 

////////////////////////////////////////////////////////////////////////////////

}; // end namespace pykd
