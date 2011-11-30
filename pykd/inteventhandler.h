// 
// Internal debug event handler
// 

#pragma once

#include <DbgEng.h>
#include "synsymbol.h"

namespace pykd {

class InternalDbgEventHandler : public DebugBaseEventCallbacks {
public:

    InternalDbgEventHandler(
        IDebugClient4 *client,
        SynSymbolsPtr synSymbols
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
    );

private:

    HRESULT symLoaded(__in ULONG64 ModuleAddress);

    IDebugClient *m_client;
    SynSymbolsPtr m_synSymbols;
};

};  // namespace pykd
