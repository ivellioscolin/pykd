
#include "stdafx.h"
#include "eventhandler.h"

namespace pykd {


EventHandler     *g_eventHandler = NULL;

///////////////////////////////////////////////////////////////////////////////

EventHandler::EventHandler()
{
    eventRegisterCallbacks( this );
}

///////////////////////////////////////////////////////////////////////////////


EventHandler::~EventHandler()
{
    eventRemoveCallbacks();
}

///////////////////////////////////////////////////////////////////////////////

};  // namespace pykd



























///////////////////////////////////////////////////////////////////////////////
//
//InternalDbgEventHandler::InternalDbgEventHandler(
//    IDebugClient4 *client,
//    DebugClient *parentClient,
//    SynSymbolsPtr synSymbols,
//    BpCallbackMap &bpCallbacks
//)   : m_parentClient(parentClient)
//    , m_synSymbols(synSymbols)
//    , m_bpCallbacks(bpCallbacks)
//{
//    HRESULT hres = client->CreateClient(&m_client);
//    if (FAILED(hres))
//        throw DbgException("Call IDebugClient::CreateClient failed");
//
//    hres = m_client->QueryInterface(__uuidof(IDebugControl), (void**)&m_control);
//    if ( FAILED( hres ) )
//        throw DbgException("QueryInterface IDebugControl failed");
//
//    m_client->SetEventCallbacks(this);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//InternalDbgEventHandler::~InternalDbgEventHandler()
//{
//    m_control->Release();
//    m_client->Release();
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//HRESULT InternalDbgEventHandler::GetInterestMask(
//    __out PULONG Mask
//)
//{
//    *Mask = 
//        DEBUG_EVENT_BREAKPOINT |
//        DEBUG_EVENT_CHANGE_ENGINE_STATE |
//        DEBUG_EVENT_CHANGE_SYMBOL_STATE;
//
//    return S_OK;
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//HRESULT InternalDbgEventHandler::ChangeSymbolState(
//    __in ULONG Flags,
//    __in ULONG64 Argument
//)
//{
//    HRESULT hres = S_OK;
//
//    if (DEBUG_CSS_LOADS & Flags)
//        hres = symLoaded(Argument);
//
//    return hres;
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//HRESULT InternalDbgEventHandler::ChangeEngineState(
//    __in ULONG Flags,
//    __in ULONG64 Argument
//)
//{
//    HRESULT hres = S_OK;
//
//    if (DEBUG_CES_BREAKPOINTS & Flags)
//        hres = bpChanged(static_cast<BPOINT_ID>(Argument));
//
//    return hres;
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//HRESULT InternalDbgEventHandler::Breakpoint(IDebugBreakpoint *bp)
//{
//    BPOINT_ID Id;
//    HRESULT hres = bp->GetId(&Id);
//    if (S_OK == hres)
//    {
//        boost::recursive_mutex::scoped_lock mapBpLock(*m_bpCallbacks.m_lock);
//        BpCallbackMapIml::iterator it = m_bpCallbacks.m_map.find(Id);
//        if (it != m_bpCallbacks.m_map.end())
//        {
//            PyThread_StateSave pyThreadSave( m_parentClient->getThreadState() );
//
//            try {
//
//                python::object      resObj = it->second(Id);
//       
//                if (resObj.is_none())
//                    return DEBUG_STATUS_NO_CHANGE;
//
//                python::extract<HRESULT> getRetCode( resObj );
//                if (getRetCode.check())
//                    return getRetCode();
//            }
//            catch (const python::error_already_set &) {
//                // TODO: some logging, alerting...
//                return DEBUG_STATUS_BREAK;
//            }
//            // TODO: python code return invalid value
//            //       some logging, alerting...
//            return DEBUG_STATUS_BREAK;
//        }
//    }
//    return DEBUG_STATUS_NO_CHANGE;
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//HRESULT InternalDbgEventHandler::symLoaded(
//    __in ULONG64 ModuleAddress
//)
//{
//    if (!ModuleAddress)
//    {
//        // f.e. is case ".reload /f image.exe", if for image.exe no symbols
//        m_synSymbols->restoreAll();
//        return S_OK;
//    }
//
//    m_synSymbols->restoreForModule(ModuleAddress);
//    return S_OK;
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//HRESULT InternalDbgEventHandler::bpChanged(BPOINT_ID Id)
//{
//    if (DEBUG_ANY_ID == Id)
//        return S_OK;
//
//    IDebugBreakpoint *bp;
//    HRESULT hres = m_control->GetBreakpointById(Id, &bp);
//    if (E_NOINTERFACE == hres)
//    {
//        // breakpoint was removed
//        boost::recursive_mutex::scoped_lock mapBpLock(*m_bpCallbacks.m_lock);
//        m_bpCallbacks.m_map.erase(Id);
//    }
//
//    return S_OK;
//}
//
//////////////////////////////////////////////////////////////////////////////////

