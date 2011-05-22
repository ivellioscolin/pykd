
#pragma once

//////////////////////////////////////////////////////////////////////////////

// monitoring and processing debug events
class DbgEventCallbacksManager : public DebugBaseEventCallbacks
{
public:

    DbgEventCallbacksManager( IDebugClient  *client = NULL );
    
    virtual ~DbgEventCallbacksManager();

private:

    /////////////////////////////////////////////////////////////////////////////////
    // IUnknown interface implementation

    STDMETHOD_(ULONG, AddRef)() { return 1; }
    STDMETHOD_(ULONG, Release)() { return 1; }
    
    /////////////////////////////////////////////////////////////////////////////////
    // IDebugEventCallbacks interface implementation

    STDMETHOD(GetInterestMask)(
        __out PULONG Mask
    );

    STDMETHOD(ChangeSymbolState)(
        __in ULONG Flags,
        __in ULONG64 Argument
    );

    STDMETHOD(Breakpoint)(
        __in PDEBUG_BREAKPOINT Bp
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

private:
    IDebugClient       *m_debugClient;
};

//////////////////////////////////////////////////////////////////////////////