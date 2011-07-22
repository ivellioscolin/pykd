///////////////////////////////////////////////////////////////////////////////////
//// Load/Unload module events
///////////////////////////////////////////////////////////////////////////////////
//
#include "stdafx.h"
#include "dbgevent.h"
#include "dbgio.h"
#include "dbgexcept.h"

///////////////////////////////////////////////////////////////////////////////////

debugEvent::debugEvent()
{
     HRESULT    hres;
     
     hres = dbgExt->client->CreateClient( &m_debugClient );
     if ( FAILED( hres ) )
        throw DbgException( "IDebugClient::CreateClient" );    
     
     hres = m_debugClient->SetEventCallbacks(this);
     if (FAILED(hres))
        throw DbgException( "IDebugClient::SetEventCallbacks" );      
}

///////////////////////////////////////////////////////////////////////////////////

debugEvent::~debugEvent()
{
    m_debugClient->SetEventCallbacks( NULL );
    
    m_debugClient->Release();
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT debugEvent::GetInterestMask(
    __out PULONG Mask
)    
{
    *Mask = 0;

    *Mask |= DEBUG_EVENT_LOAD_MODULE;
    *Mask |= DEBUG_EVENT_UNLOAD_MODULE;  
    *Mask |= DEBUG_EVENT_SESSION_STATUS;
        
    return S_OK;     
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT debugEvent::LoadModule(
    __in ULONG64 ImageFileHandle,
    __in ULONG64 BaseOffset,
    __in ULONG ModuleSize,
    __in PCSTR ModuleName,
    __in PCSTR ImageName,
    __in ULONG CheckSum,
    __in ULONG TimeDateStamp
)
{
    std::auto_ptr<OutputReader> silentMode( new OutputReader(dbgExt->client) );

    ULONG64         moduleBase;
    ULONG           moduleSize;
    std::string     moduleName;       
    
    queryModuleParams(BaseOffset, moduleName, moduleBase, moduleSize);
    dbgModuleClass module(moduleName, moduleBase, moduleSize);
    silentMode.reset(); 
    
    return onLoadModule( module );
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT debugEvent::UnloadModule(
    __in PCSTR ImageBaseName,
    __in ULONG64 BaseOffset
)
{
    std::auto_ptr<OutputReader> silentMode( new OutputReader(dbgExt->client) );

    ULONG64         moduleBase;
    ULONG           moduleSize;
    std::string     moduleName;       
    
    queryModuleParams(BaseOffset, moduleName, moduleBase, moduleSize);
    dbgModuleClass module(moduleName, moduleBase, moduleSize);
    silentMode.reset(); 
    
    return onUnloadModule( module );
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT debugEvent::SessionStatus(
        __in ULONG  Status
)
{
    return onChangeSessionStatus( Status );
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT debugEvent::ChangeDebuggeeState(
        __in ULONG  Flags,
        __in ULONG64 Argument
)
{
    return onChangeDebugeeState();
}

///////////////////////////////////////////////////////////////////////////////////
        






//
//    STDMETHOD(GetInterestMask)(
//        __out PULONG Mask
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
//public:   
//    
//    ULONG onLoadModule(const dbgModuleClass &module);
//
//    ULONG onUnloadModule(const dbgModuleClass &module);
//





//#include <memory>
//#include <dbgeng.h>
//
//#include "dbgmodule.h"
//#include "dbgio.h"
//#include "dbgevent.h"
//
///////////////////////////////////////////////////////////////////////////////////
//
//debugEvent::modCallbacksColl debugEvent::modCallbacks;
//debugEvent::modCallbacksLock debugEvent::modCallbacksMtx;
//
///////////////////////////////////////////////////////////////////////////////////
//
//debugEvent::debugEvent()
//{
//    modCallbacksScopedLock lock(modCallbacksMtx);
//    modCallbacks.insert(this);
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//debugEvent::~debugEvent()
//{
//    modCallbacksScopedLock lock(modCallbacksMtx);
//    modCallbacks.erase(this);
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//ULONG debugEvent::moduleLoaded(__in ULONG64 addr)
//{
//    modCallbacksScopedLock lock(modCallbacksMtx);
//    if (modCallbacks.empty())
//        return DEBUG_STATUS_NO_CHANGE;
//
//    ULONG64 moduleBase;
//    ULONG moduleSize;
//    std::string moduleName;
//
//    std::auto_ptr<OutputReader> silentMode( new OutputReader(dbgExt->client) );
//    queryModuleParams(addr, moduleName, moduleBase, moduleSize);
//    dbgModuleClass module(moduleName, moduleBase, moduleSize);
//    silentMode.reset();
//
//    modCallbacksColl::iterator itCallback = modCallbacks.begin();
//    while (itCallback != modCallbacks.end())
//    {
//        const ULONG retValue = (*itCallback)->onLoadModule(module);
//        if (DEBUG_STATUS_NO_CHANGE != retValue)
//            return retValue;
//
//        ++itCallback;
//    }
//    return DEBUG_STATUS_NO_CHANGE;
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//ULONG debugEvent::moduleUnloaded(__in ULONG64 addr)
//{
//    modCallbacksScopedLock lock(modCallbacksMtx);
//    if (modCallbacks.empty())
//        return DEBUG_STATUS_NO_CHANGE;
//
//    ULONG64 moduleBase;
//    ULONG moduleSize;
//    std::string moduleName;
//
//    std::auto_ptr<OutputReader> silentMode( new OutputReader(dbgExt->client) );
//    queryModuleParams(addr, moduleName, moduleBase, moduleSize);
//    dbgModuleClass module(moduleName, moduleBase, moduleSize);
//    silentMode.reset();
//
//    modCallbacksColl::iterator itCallback = modCallbacks.begin();
//    while (itCallback != modCallbacks.end())
//    {
//        const ULONG retValue = (*itCallback)->onUnloadModule(module);
//        if (DEBUG_STATUS_NO_CHANGE != retValue)
//            return retValue;
//
//        ++itCallback;
//    }
//    return DEBUG_STATUS_NO_CHANGE;
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//ULONG debugEvent::sessionStatus(__in ULONG status )
//{
//    
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//ULONG debugEventWrap::onLoadModule(const dbgModuleClass &module)
//{
//    if (boost::python::override override = get_override("onLoadModule"))
//        return override(module);
//
//    return debugEvent::onLoadModule(module);
//}
//
///////////////////////////////////////////////////////////////////////////////////
//
//ULONG debugEventWrap::onUnloadModule(const dbgModuleClass &module)
//{
//    if (boost::python::override override = get_override("onUnloadModule"))
//        return override(module);
//
//    return debugEvent::onUnloadModule(module);
//}
//
///////////////////////////////////////////////////////////////////////////////////
