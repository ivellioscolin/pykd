
#include "stdafx.h"

#include "dbgext.h"
#include "dbgmem.h"
#include "dbgmodule.h"
#include "dbgexcept.h"
#include "dbgsynsym.h"
#include "dbgeventcb.h"

/////////////////////////////////////////////////////////////////////////////////

DbgEventCallbacks dbgEventCallbacks;

/////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacks::Register()
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
            __uuidof(IDebugSymbols),
            reinterpret_cast<PVOID *>(&m_dbgSymbols));
        if (FAILED(hres))
            throw hres;

        hres = m_dbgClient->QueryInterface(
            __uuidof(IDebugSymbols3),
            reinterpret_cast<PVOID *>(&m_dbgSymbols3));
        if (FAILED(hres))
            throw hres;

        hres = m_dbgClient->GetEventCallbacks(&m_prevCallbacks);
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
        Deregister();

    return hres;
}

/////////////////////////////////////////////////////////////////////////////////

void DbgEventCallbacks::Deregister()
{
    if (m_dbgSymbols3)
    {
        m_dbgSymbols3->Release();
        m_dbgSymbols3 = NULL;
    }
    if (m_dbgSymbols)
    {
        m_dbgSymbols->Release();
        m_dbgSymbols = NULL;
    }
    if (m_dbgClient)
    {
        m_dbgClient->SetEventCallbacks(m_prevCallbacks);

        m_dbgClient->Release();
        m_dbgClient = NULL;
    }
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

HRESULT DbgEventCallbacks::Breakpoint(
    __in PDEBUG_BREAKPOINT  /* Bp */
)
{
    return DEBUG_STATUS_IGNORE_EVENT;
}

/////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacks::Exception(
    __in PEXCEPTION_RECORD64 /* Exception */,
    __in ULONG /* FirstChance */
)
{
    return DEBUG_STATUS_IGNORE_EVENT;
}

/////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacks::CreateThread(
    __in ULONG64 /* Handle */,
    __in ULONG64 /* DataOffset */,
    __in ULONG64 /* StartOffset */
)
{
    return DEBUG_STATUS_IGNORE_EVENT;
}

/////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacks::ExitThread(
    __in ULONG /* ExitCode */
)
{
    return DEBUG_STATUS_IGNORE_EVENT;
}

/////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacks::CreateProcess(
    __in ULONG64 /* ImageFileHandle */,
    __in ULONG64 /* Handle */,
    __in ULONG64 /* BaseOffset */,
    __in ULONG /* ModuleSize */,
    __in_opt PCSTR /* ModuleName */,
    __in_opt PCSTR /* ImageName */,
    __in ULONG /* CheckSum */,
    __in ULONG /* TimeDateStamp */,
    __in ULONG64 /* InitialThreadHandle */,
    __in ULONG64 /* ThreadDataOffset */,
    __in ULONG64 /* StartOffset */
)
{
    return DEBUG_STATUS_IGNORE_EVENT;
}

/////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacks::ExitProcess(
    __in ULONG /* ExitCode */
)
{
    return DEBUG_STATUS_IGNORE_EVENT;
}

/////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacks::LoadModule(
    __in ULONG64 /* ImageFileHandle */,
    __in ULONG64 /* BaseOffset */,
    __in ULONG /* ModuleSize */,
    __in_opt PCSTR /* ModuleName */,
    __in_opt PCSTR /* ImageName */,
    __in ULONG /* CheckSum */,
    __in ULONG /* TimeDateStamp*/
)
{
    return DEBUG_STATUS_IGNORE_EVENT;
}

/////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacks::UnloadModule(
    __in_opt PCSTR /* ImageBaseName */,
    __in ULONG64 /* BaseOffset */
)
{
    return DEBUG_STATUS_IGNORE_EVENT;
}

/////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacks::SystemError(
    __in ULONG /* Error */,
    __in ULONG /* Level */
)
{
    return DEBUG_STATUS_IGNORE_EVENT;
}

/////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacks::SessionStatus(
    __in ULONG /* Status */
)
{
    return DEBUG_STATUS_IGNORE_EVENT;
}

/////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacks::ChangeDebuggeeState(
    __in ULONG /* Flags */,
    __in ULONG64 /* Argument */
)
{
    return DEBUG_STATUS_IGNORE_EVENT;
}

/////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacks::ChangeEngineState(
    __in ULONG /* Flags */,
    __in ULONG64 /* Argument */
)
{
    return DEBUG_STATUS_IGNORE_EVENT;
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
        restoreSyntheticSymbolForAllModules(m_dbgSymbols, m_dbgSymbols3);
        return S_OK;
    }

    return DEBUG_STATUS_IGNORE_EVENT;
}

/////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacks::doSymbolsLoaded(
    ULONG64 moduleBase
)
{
    try
    {
        DEBUG_MODULE_PARAMETERS dbgModuleParameters;
        HRESULT hres = m_dbgSymbols->GetModuleParameters(
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
