#pragma once

#include "kdlib/dbgengine.h"
#include "pythreadstate.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

struct DebugEvent
{
    kdlib::EventType  eventType;
    kdlib::PROCESS_DEBUG_ID  process;
    kdlib::THREAD_DEBUG_ID  thread;
};

inline DebugEvent getLastEvent()
{
    AutoRestorePyState  pystate;

    kdlib::EventType eventType = kdlib::getLastEventType();
    kdlib::PROCESS_DEBUG_ID  processId = kdlib::getLastEventProcessId();
    kdlib::THREAD_DEBUG_ID  threadId = kdlib::getLastEventThreadId();

    DebugEvent  ev = { eventType, processId, threadId };

    return ev;
}

inline kdlib::ExceptionInfo getLastException()
{
    AutoRestorePyState  pystate;

    return kdlib::getLastException();
}

///////////////////////////////////////////////////////////////////////////////

} // pykd namespace
