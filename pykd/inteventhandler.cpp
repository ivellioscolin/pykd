
#include "stdafx.h"

#include "inteventhandler.h"
#include "dbgexcept.h"

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

InternalDbgEventHandler::InternalDbgEventHandler(
    IDebugClient4 *client,
    SynSymbolsPtr synSymbols
)   : m_synSymbols(synSymbols)
{
    HRESULT hres = client->CreateClient(&m_client);
    if (FAILED(hres))
        throw DbgException("Call IDebugClient::CreateClient failed");

    m_client->SetEventCallbacks(this);
}

////////////////////////////////////////////////////////////////////////////////

InternalDbgEventHandler::~InternalDbgEventHandler()
{
    m_client->Release();
}

////////////////////////////////////////////////////////////////////////////////

HRESULT InternalDbgEventHandler::GetInterestMask(
    __out PULONG Mask
)
{
    *Mask = 
        DEBUG_EVENT_CHANGE_SYMBOL_STATE;

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT InternalDbgEventHandler::ChangeSymbolState(
    __in ULONG Flags,
    __in ULONG64 Argument
)
{
    HRESULT hres = S_OK;

    if (DEBUG_CSS_LOADS & Flags)
        hres = symLoaded(Argument);

    return hres;
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT InternalDbgEventHandler::symLoaded(
    __in ULONG64 ModuleAddress
)
{
    if (!ModuleAddress)
    {
        // f.e. is case ".reload /f image.exe", if for image.exe no symbols
        m_synSymbols->restoreAll();
        return S_OK;
    }

    m_synSymbols->restoreForModule(ModuleAddress);
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////

};  // namespace pykd
