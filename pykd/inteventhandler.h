// 
// Internal debug event handler
// 

#pragma once

#include <DbgEng.h>
#include "synsymbol.h"

////////////////////////////////////////////////////////////////////////////////


namespace pykd {

////////////////////////////////////////////////////////////////////////////////

struct BpCallbackMap;
class DebugClient;

////////////////////////////////////////////////////////////////////////////////

class InternalDbgEventHandler : public DebugBaseEventCallbacks {
public:

    InternalDbgEventHandler(
        IDebugClient4 *client,
        DebugClient *parentClient,
        SynSymbolsPtr synSymbols,
        BpCallbackMap &bpCallbacks
    );
    ~InternalDbgEventHandler();

protected:

    // IUnknown impls
    STDMETHOD_(ULONG, AddRef)() { return 1; }
    STDMETHOD_(ULONG, Release)() { return 1; }

    // IDebugEventCallbacks impls
    STDMETHOD(GetInterestMask)(
        __out PULONG Mask
    );

    STDMETHOD(ChangeSymbolState)(
        __in ULONG Flags,
        __in ULONG64 Argument
    ) override;

    STDMETHOD(ChangeEngineState)(
        __in ULONG Flags,
        __in ULONG64 Argument
    ) override;

    STDMETHOD(Breakpoint)(
        __in IDebugBreakpoint *bp
    ) override;

private:

    HRESULT symLoaded(__in ULONG64 ModuleAddress);

    HRESULT bpChanged(ULONG Id);

    IDebugClient *m_client;
    IDebugControl *m_control;

    DebugClient *m_parentClient;

    SynSymbolsPtr m_synSymbols;
    BpCallbackMap &m_bpCallbacks;
};

////////////////////////////////////////////////////////////////////////////////

};  // namespace pykd

////////////////////////////////////////////////////////////////////////////////