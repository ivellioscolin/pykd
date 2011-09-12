
#include "stdafx.h"
//#include "dbgeng.h"
//#include "dbgext.h"
//#include "dbgeventcb.h"
//#include "dbgexcept.h"
//#include "dbgmodule.h"
//#include "dbgsynsym.h"
//#include "dbgbreak.h"
//#include "dbgevent.h"
//
/////////////////////////////////////////////////////////////////////////////////////
//
//DbgEventCallbacksManager::DbgEventCallbacksManager( IDebugClient  *client )
//{
//    m_debugClient = client;
//    m_debugClient->AddRef();
//    
//    HRESULT     hres;
//
//     hres = m_debugClient->SetEventCallbacks(this);
//     if (FAILED(hres))
//        throw DbgException( "IDebugClient::SetEventCallbacks" );               
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//DbgEventCallbacksManager::~DbgEventCallbacksManager()
//{
//    if ( m_debugClient )
//    {
//        m_debugClient->SetEventCallbacks( NULL );
//        m_debugClient->Release();
//    }        
//}
//
//
/////////////////////////////////////////////////////////////////////////////////////
//
//HRESULT DbgEventCallbacksManager::GetInterestMask(
//    __out PULONG Mask
//)
//{
//    *Mask = 
//        DEBUG_EVENT_CHANGE_SYMBOL_STATE |
//        DEBUG_EVENT_BREAKPOINT;
////        DEBUG_EVENT_LOAD_MODULE | 
////        DEBUG_EVENT_UNLOAD_MODULE
//    return S_OK;
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//HRESULT DbgEventCallbacksManager::ChangeSymbolState(
//    __in ULONG Flags,
//    __in ULONG64 Argument
//)
//{
//    if ((DEBUG_CSS_LOADS & Flags))
//    {
//        if (Argument)
//        {
//            DEBUG_MODULE_PARAMETERS dbgModuleParameters;
//            HRESULT hres = dbgExt->symbols3->GetModuleParameters(
//                1,
//                &Argument,
//                0,
//                &dbgModuleParameters);
//
//            if (SUCCEEDED(hres))
//            {
//                 ModuleInfo     moduleInfo(dbgModuleParameters);
//                 restoreSyntheticSymbolForModule(moduleInfo);
//            }
//
//            return S_OK;
//        }
//
//        //// f.e. is case ".reload /f image.exe", if for image.exe no symbols
//        restoreSyntheticSymbolForAllModules();
//
//        return S_OK;
//    }
//
//    return DEBUG_STATUS_NO_CHANGE;
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//HRESULT DbgEventCallbacksManager::Breakpoint(
//    __in IDebugBreakpoint *  bp
//)
//{
//    return dbgBreakpointClass::onBreakpointEvnet( bp );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
////
////HRESULT DbgEventCallbacksManager::LoadModule(
////    __in ULONG64 ImageFileHandle,
////    __in ULONG64 BaseOffset,
////    __in ULONG ModuleSize,
////    __in PCSTR ModuleName,
////    __in PCSTR ImageName,
////    __in ULONG CheckSum,
////    __in ULONG TimeDateStamp
////)
////{
////    try
////    {
////        return debugEvent::moduleLoaded(BaseOffset);
////    }
////    catch (std::exception &)
////    {
////    }
////    return DEBUG_STATUS_NO_CHANGE;
////}
////
///////////////////////////////////////////////////////////////////////////////////////
////
////HRESULT DbgEventCallbacksManager::UnloadModule(
////    __in PCSTR ImageBaseName,
////    __in ULONG64 BaseOffset
////)
////{
////    try
////    {
////        return debugEvent::moduleUnloaded(BaseOffset);
////    }
////    catch (std::exception &)
////    {
////    }
////    return DEBUG_STATUS_NO_CHANGE;
////}
////
///////////////////////////////////////////////////////////////////////////////////////
////
////HRESULT DbgEventCallbacksManager::SessionStatus( 
////    __in ULONG status 
////)
////{
////    try
////    {
////        return debugEvent::sessionStatus( status );
////    }
////    catch( std::exception& )
////    {
////    }
////    
////    return DEBUG_STATUS_NO_CHANGE;
////}
////
///////////////////////////////////////////////////////////////////////////////////////
