/////////////////////////////////////////////////////////////////////////////////
//  user-customizing debug event handler
/////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"

#include <dbgeng.h>
#include "dbgobj.h"
#include "module.h"
#include "dbgclient.h"

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////

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

    virtual ULONG onBreakpoint(const python::dict &/*bpParameters*/) = 0;

    virtual ULONG onException(const python::dict &/*exceptData*/) = 0;

    virtual ULONG onLoadModule(const Module &/* module */)  = 0;

    virtual ULONG onUnloadModule(const Module &/* module */)  = 0;
    
    virtual ULONG onChangeSessionStatus( ULONG status ) = 0;
    
    virtual ULONG onChangeDebugeeState() = 0;

protected:

    CComPtr<IDebugClient>       m_handlerClient;

    DebugClientPtr              m_parentClient;
};

/////////////////////////////////////////////////////////////////////////////////

class EventHandlerWrap : public boost::python::wrapper<EventHandler>, public EventHandler
{

public:

    EventHandlerWrap()
    {}

    EventHandlerWrap( DebugClientPtr  &client ) : EventHandler( client )
    {}

    ULONG onBreakpoint(const python::dict &bpParameters) {
        return handler<const python::dict&>("onBreakpoint", bpParameters);
    }

    ULONG onException(const python::dict &exceptData) {
        return handler<const python::dict&>("onException", exceptData);
    }

    ULONG onLoadModule(const Module &module) {
        return handler<const Module&>("onLoadModule", module );
    }

    ULONG onUnloadModule(const Module &module) {
        return handler<const Module&>("onUnloadModule", module );
    }

    ULONG onChangeSessionStatus( ULONG status ) {
        return handler( "onChangeSessionStatus", status );
    }

    ULONG onChangeDebugeeState() {
        return handler( "onChangeDebugeeState" );
    }

private:

    template<typename Arg1Type>
    ULONG handler( const char* handlerName, Arg1Type  arg1 )
    {
        if (python::override pythonHandler = get_override( handlerName ))
            return pythonHandler(arg1);

        return DEBUG_STATUS_NO_CHANGE;
    }

    ULONG handler( const char* handlerName )
    {
        if (python::override pythonHandler = get_override( handlerName ))
            return pythonHandler();

        return DEBUG_STATUS_NO_CHANGE;
    }
}; 

/////////////////////////////////////////////////////////////////////////////////

}; // end namespace pykd












//#include "dbgeventcb.h"
//#include "dbgmodule.h"
//#include "pyaux.h"
//
///////////////////////////////////////////////////////////////////////////////////
//
//class debugEvent : public DebugBaseEventCallbacks
//{
//public:
//
//    debugEvent();
//
//    virtual ~debugEvent();
//
//    virtual ULONG onBreakpoint(boost::python::dict &/*bpParameters*/) = 0;
//
//    virtual ULONG onException(boost::python::dict &/*exceptData*/) = 0;
//
//    virtual ULONG onLoadModule(const dbgModuleClass &/* module */)  = 0;
//
//    virtual ULONG onUnloadModule(const dbgModuleClass &/* module */)  = 0;
//    
//    virtual ULONG onChangeSessionStatus( ULONG status ) = 0;
//    
//    virtual ULONG onChangeDebugeeState() = 0;
//    
//private:
//
//    STDMETHOD_(ULONG, AddRef)() { return 1; }
//    STDMETHOD_(ULONG, Release)() { return 1; }
//
//    STDMETHOD(GetInterestMask)(
//        __out PULONG Mask
//    );
//
//    STDMETHOD(Breakpoint)(
//        __in PDEBUG_BREAKPOINT Bp
//    );
//
//
//    STDMETHOD(Exception)(
//        __in PEXCEPTION_RECORD64 Exception,
//        __in ULONG FirstChance
//    );
//
//    STDMETHOD(LoadModule)(
//        __in ULONG64 ImageFileHandle,
//        __in ULONG64 BaseOffset,
//        __in ULONG ModuleSize,
//        __in PCSTR ModuleName,
//        __in PCSTR ImageName,
//        __in ULONG CheckSum,
//        __in ULONG TimeDateStamp
//    );
//
//    STDMETHOD(UnloadModule)(
//        __in PCSTR ImageBaseName,
//        __in ULONG64 BaseOffset
//    );  
//
//    STDMETHOD(SessionStatus)(
//        __in ULONG  Status
//    );
//
//    STDMETHOD(ChangeDebuggeeState)(
//        __in ULONG  Flags,
//        __in ULONG64 Argument );
//
//private:
//
//    IDebugClient       *m_debugClient;
//};
//
///////////////////////////////////////////////////////////////////////////////////
//
//class debugEventWrap : public boost::python::wrapper<debugEvent>, public debugEvent
//{
//
//public:
//
//    ULONG onBreakpoint(boost::python::dict &bpParameters) {
//        return handler<boost::python::dict &>("onBreakpoint", bpParameters);
//    }
//
//    ULONG onException(boost::python::dict &exceptData) {
//        return handler<boost::python::dict &>("onException", exceptData);
//    }
//
//    ULONG onLoadModule(const dbgModuleClass &module) {
//        return handler<const dbgModuleClass &>("onLoadModule", module );
//    }
//
//    ULONG onUnloadModule(const dbgModuleClass &module) {
//        return handler<const dbgModuleClass &>("onUnloadModule", module );
//    }
//
//    ULONG onChangeSessionStatus( ULONG status ) {
//        return handler( "onChangeSessionStatus", status );
//    }
//
//    ULONG onChangeDebugeeState() {
//        return handler( "onChangeDebugeeState" );
//    }
//
//private:
//
//    template<typename Arg1Type>
//    ULONG handler( const char* handlerName, Arg1Type  arg1 )
//    {
//        if (boost::python::override pythonHandler = get_override( handlerName ))
//            return pythonHandler(arg1);
//
//        return DEBUG_STATUS_NO_CHANGE;
//    }
//
//    ULONG handler( const char* handlerName )
//    {
//        if (boost::python::override pythonHandler = get_override( handlerName ))
//            return pythonHandler();
//
//        return DEBUG_STATUS_NO_CHANGE;
//    }
//};
//
///////////////////////////////////////////////////////////////////////////////////
