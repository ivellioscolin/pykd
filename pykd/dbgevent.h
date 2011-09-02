/////////////////////////////////////////////////////////////////////////////////
//  user-customizing debug event handler
/////////////////////////////////////////////////////////////////////////////////

#include "dbgeventcb.h"
#include "dbgmodule.h"
#include "pyaux.h"

/////////////////////////////////////////////////////////////////////////////////

class debugEvent : public DebugBaseEventCallbacks
{
public:

    debugEvent();

    virtual ~debugEvent();

    virtual ULONG onBreakpoint(boost::python::dict &/*bpParameters*/) = 0;

    virtual ULONG onException(boost::python::dict &/*exceptData*/) = 0;

    virtual ULONG onLoadModule(const dbgModuleClass &/* module */)  = 0;

    virtual ULONG onUnloadModule(const dbgModuleClass &/* module */)  = 0;
    
    virtual ULONG onChangeSessionStatus( ULONG status ) = 0;
    
    virtual ULONG onChangeDebugeeState() = 0;
    
private:

    STDMETHOD_(ULONG, AddRef)() { return 1; }
    STDMETHOD_(ULONG, Release)() { return 1; }

    STDMETHOD(GetInterestMask)(
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

private:

    IDebugClient       *m_debugClient;
};

/////////////////////////////////////////////////////////////////////////////////

class debugEventWrap : public boost::python::wrapper<debugEvent>, public debugEvent
{

public:

    ULONG onBreakpoint(boost::python::dict &bpParameters) {
        return handler<boost::python::dict &>("onBreakpoint", bpParameters);
    }

    ULONG onException(boost::python::dict &exceptData) {
        return handler<boost::python::dict &>("onException", exceptData);
    }

    ULONG onLoadModule(const dbgModuleClass &module) {
        return handler<const dbgModuleClass &>("onLoadModule", module );
    }

    ULONG onUnloadModule(const dbgModuleClass &module) {
        return handler<const dbgModuleClass &>("onUnloadModule", module );
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
        if (boost::python::override pythonHandler = get_override( handlerName ))
            return pythonHandler(arg1);

        return DEBUG_STATUS_NO_CHANGE;
    }

    ULONG handler( const char* handlerName )
    {
        if (boost::python::override pythonHandler = get_override( handlerName ))
            return pythonHandler();

        return DEBUG_STATUS_NO_CHANGE;
    }
};

/////////////////////////////////////////////////////////////////////////////////


//
//#include <set>
//
//#include <boost/interprocess/sync/interprocess_recursive_mutex.hpp>
//#include <boost/interprocess/sync/scoped_lock.hpp>
//
//class debugEvent
//{
//    debugEvent();
//    virtual ~debugEvent();
//
//    virtual ULONG onLoadModule(const dbgModuleClass &/* module */)
//    {
//        return DEBUG_STATUS_NO_CHANGE;
//    }
//    
//    virtual ULONG onUnloadModule(const dbgModuleClass &/* module */)
//    {
//        return DEBUG_STATUS_NO_CHANGE;
//    }
//    
//    virtual ULONG onChangeSessionState( ULONG state )
//    {
//        return DEBUG_STATUS_NO_CHANGE;
//    }
//    
//
//    // call from debug engine
//    static ULONG moduleLoaded(__in ULONG64 addr);
//    static ULONG moduleUnloaded(__in ULONG64 addr);
//    static ULONG sessionStatus(__in ULONG status );
//
//private:
//
//    typedef std::set<debugEvent *> modCallbacksColl;
//    static modCallbacksColl modCallbacks;    
//   
//
//    //typedef boost::interprocess::interprocess_recursive_mutex modCallbacksLock;
//    //static modCallbacksLock modCallbacksMtx;
//    //typedef boost::interprocess::scoped_lock<modCallbacksLock> modCallbacksScopedLock;
//};
//
//// python wrapper for debugEvent
//struct debugEventWrap : debugEvent, boost::python::wrapper<debugEvent>
//{
//    ULONG onLoadModule(const dbgModuleClass &module);
//    ULONG onLoadModuleDef(const dbgModuleClass &module) 
//    {
//        return debugEvent::onLoadModule(module);
//    }
//
//    ULONG onUnloadModule(const dbgModuleClass &module);
//    ULONG onUnloadModuleDef(const dbgModuleClass &module)
//    {
//        return debugEvent::onUnloadModule(module);
//    }
//};
//
///////////////////////////////////////////////////////////////////////////////////
