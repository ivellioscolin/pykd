
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
    
private:
  
    IDebugClient4       *m_debugClient;
};

//////////////////////////////////////////////////////////////////////////////