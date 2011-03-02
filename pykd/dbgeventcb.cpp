
#include "stdafx.h"

#include "dbgext.h"
#include "dbgmem.h"
#include "dbgmodule.h"
#include "dbgexcept.h"
#include "dbgsynsym.h"
#include "dbgeventcb.h"

/////////////////////////////////////////////////////////////////////////////////

DbgEventCallbacks *dbgEventCallbacks = NULL;

/////////////////////////////////////////////////////////////////////////////////

DbgEventCallbacks::DbgEventCallbacks()
  : m_ReferenceCount(1)
  , m_dbgClient(NULL)
  , m_dbgSymbols3(NULL)
{
    HRESULT hres;
    try
    {
        // monitor "global" WinDbg events
        hres = DebugCreate(
            __uuidof(IDebugClient),
            reinterpret_cast<PVOID *>(&m_dbgClient));
        if (FAILED(hres))
            throw hres;

        hres = m_dbgClient->QueryInterface(
            __uuidof(IDebugSymbols3),
            reinterpret_cast<PVOID *>(&m_dbgSymbols3));
        if (FAILED(hres))
            throw hres;

        hres = m_dbgClient->SetEventCallbacks(this);
        if (FAILED(hres))
            throw hres;

        hres = S_OK;
    }
    catch(HRESULT _hres)
    {
        hres = _hres;
    }
    catch(...)
    {
        hres = S_FALSE;
    }
    if (S_OK != hres)
    {
        Deregister();
        throw hres;
    }
}

/////////////////////////////////////////////////////////////////////////////////

void DbgEventCallbacks::Deregister()
{
    if (m_dbgSymbols3)
    {
        m_dbgSymbols3->Release();
        m_dbgSymbols3 = NULL;
    }
    if (m_dbgClient)
    {
        m_dbgClient->Release();
        m_dbgClient = NULL;
    }
    Release();
}

/////////////////////////////////////////////////////////////////////////////////

ULONG DbgEventCallbacks::AddRef()
{
    return InterlockedIncrement(&m_ReferenceCount);
}

/////////////////////////////////////////////////////////////////////////////////

ULONG DbgEventCallbacks::Release()
{
    ULONG nResult = InterlockedDecrement(&m_ReferenceCount);
    if (!nResult)
    {
        dbgEventCallbacks = NULL;
        delete this;
    }
    return nResult;
}

/////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacks::GetInterestMask(
    __out PULONG Mask
)
{
    *Mask = DEBUG_EVENT_CHANGE_SYMBOL_STATE;
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacks::ChangeSymbolState(
    __in ULONG Flags,
    __in ULONG64 Argument
)
{
    if ((DEBUG_CSS_LOADS & Flags))
    {
        if (Argument)
            return doSymbolsLoaded(Argument);

        // f.e. is case ".reload /f image.exe", if for image.exe no symbols
        restoreSyntheticSymbolForAllModules(m_dbgSymbols3);
        return S_OK;
    }

    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacks::doSymbolsLoaded(
    ULONG64 moduleBase
)
{
    try
    {
        DEBUG_MODULE_PARAMETERS dbgModuleParameters;
        HRESULT hres = m_dbgSymbols3->GetModuleParameters(
            1,
            &moduleBase,
            0,
            &dbgModuleParameters);
        if (SUCCEEDED(hres))
        {
            ModuleInfo moduleInfo(dbgModuleParameters);
            restoreSyntheticSymbolForModule(moduleInfo, m_dbgSymbols3);
        }
    }
    catch (...)
    {
    }
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////
