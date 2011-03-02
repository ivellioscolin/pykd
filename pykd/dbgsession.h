#pragma once

void
dbgCreateSession();

extern
bool    dbgSessionStarted;

inline void stopDbgEventMonotoring()
{
    if (dbgEventCallbacks)
        dbgEventCallbacks->Deregister();
}

inline HRESULT setDbgSessionStarted()
{
    HRESULT hres;
    try
    {
        stopDbgEventMonotoring();
        dbgEventCallbacks = new DbgEventCallbacks;
        hres = S_OK;
    }
    catch (HRESULT _hres)
    {
        hres = _hres;
    }
    catch (...)
    {
        hres = E_OUTOFMEMORY;
    }
    if (SUCCEEDED(hres))
        dbgSessionStarted = true;
    return hres;
}

bool
dbgIsSessionStart();

