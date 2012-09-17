// 
// Internal debug event handler
// 

#pragma once

#include "dbgengine.h"

namespace pykd {

//////////////////////////////////////////////////////////////////////////////////

class EventHandler : public DEBUG_EVENT_CALLBACK {

public:

    EventHandler();

    virtual ~EventHandler();

private:

    virtual DEBUG_CALLBACK_RESULT DEBUG_CALLBACK_METHODTYPE OnBreakpoint( ULONG bpId ) {
        return DebugCallbackNoChange; 
    }
};

extern EventHandler     *g_eventHandler;

//////////////////////////////////////////////////////////////////////////////////

};  // namespace pykd
