
#pragma once

// monitoring and processing debug events
class DbgEventCallbacks : public IDebugEventCallbacks
{
public:

    DbgEventCallbacks()
      : m_dbgClient(NULL)
      , m_prevCallbacks(NULL)
      , m_dbgSymbols(NULL)
      , m_dbgSymbols3(NULL)
    {
    }
    ~DbgEventCallbacks()
    {
        Deregister();
    }

    // [de]register debug event handler
    HRESULT Register();
    void Deregister();

private:

    /////////////////////////////////////////////////////////////////////////////////
    // IUnknown interface implementation

    STDMETHOD(QueryInterface)(__in REFIID InterfaceId, __out PVOID *Interface)
    {
        if (IsEqualIID(InterfaceId, __uuidof(IUnknown)) ||
            IsEqualIID(InterfaceId, __uuidof(IDebugEventCallbacks)))
        {
            *Interface = this;
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    STDMETHOD_(ULONG, AddRef)() { return 2; }
    STDMETHOD_(ULONG, Release)() { return 1; }

    /////////////////////////////////////////////////////////////////////////////////
    // IDebugEventCallbacks interface implementation

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

    STDMETHOD(CreateThread)(
        __in ULONG64 Handle,
        __in ULONG64 DataOffset,
        __in ULONG64 StartOffset
    );

    STDMETHOD(ExitThread)(
        __in ULONG ExitCode
    );

    STDMETHOD(CreateProcess)(
        __in ULONG64 ImageFileHandle,
        __in ULONG64 Handle,
        __in ULONG64 BaseOffset,
        __in ULONG ModuleSize,
        __in_opt PCSTR ModuleName,
        __in_opt PCSTR ImageName,
        __in ULONG CheckSum,
        __in ULONG TimeDateStamp,
        __in ULONG64 InitialThreadHandle,
        __in ULONG64 ThreadDataOffset,
        __in ULONG64 StartOffset
    );

    STDMETHOD(ExitProcess)(
        __in ULONG ExitCode
    );

    STDMETHOD(LoadModule)(
        __in ULONG64 ImageFileHandle,
        __in ULONG64 BaseOffset,
        __in ULONG ModuleSize,
        __in_opt PCSTR ModuleName,
        __in_opt PCSTR ImageName,
        __in ULONG CheckSum,
        __in ULONG TimeDateStamp
    );

    STDMETHOD(UnloadModule)(
        __in_opt PCSTR ImageBaseName,
        __in ULONG64 BaseOffset
    );

    STDMETHOD(SystemError)(
        __in ULONG Error,
        __in ULONG Level
    );

    STDMETHOD(SessionStatus)(
        __in ULONG Status
    );

    STDMETHOD(ChangeDebuggeeState)(
        __in ULONG Flags,
        __in ULONG64 Argument
    );

    STDMETHOD(ChangeEngineState)(
        __in ULONG Flags,
        __in ULONG64 Argument
    );

    STDMETHOD(ChangeSymbolState)(
        __in ULONG Flags,
        __in ULONG64 Argument
    );

    /////////////////////////////////////////////////////////////////////////////////

    HRESULT doSymbolsLoaded(
        ULONG64 moduleBase
    );

    /////////////////////////////////////////////////////////////////////////////////

    IDebugClient *m_dbgClient;
    IDebugEventCallbacks *m_prevCallbacks;
    IDebugSymbols *m_dbgSymbols;
    IDebugSymbols3 *m_dbgSymbols3;
};

/////////////////////////////////////////////////////////////////////////////////
// global singleton

extern DbgEventCallbacks dbgEventCallbacks;

/////////////////////////////////////////////////////////////////////////////////
