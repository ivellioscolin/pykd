
#include "stdafx.h"
#include "dbgengine.h"

#include "win/dbgeng.h"

////////////////////////////////////////////////////////////////////////////////

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

EVENT_TYPE getLastEventType()
{
    ULONG eventType;
    ULONG ProcessId;
    ULONG ThreadId;
    HRESULT hres = 
        g_dbgEng->control->GetLastEventInformation(
            &eventType,
            &ProcessId,
            &ThreadId,
            NULL,
            0,
            NULL,
            NULL,
            0,
            NULL);
    if (S_OK != hres)
        throw DbgException("IDebugControl::GetLastEventInformation", hres);

    switch (eventType)
    {
    case DEBUG_EVENT_BREAKPOINT:
        return EventTypeBreakpoint;
    case DEBUG_EVENT_EXCEPTION:
        return EventTypeException;
    case DEBUG_EVENT_CREATE_THREAD:
        return EventTypeCreateThread;
    case DEBUG_EVENT_EXIT_THREAD:
        return EventTypeExitThread;
    case DEBUG_EVENT_CREATE_PROCESS:
        return EventTypeCreateProcess;
    case DEBUG_EVENT_EXIT_PROCESS:
        return EventTypeExitProcess;
    case DEBUG_EVENT_LOAD_MODULE:
        return EventTypeLoadModule;
    case DEBUG_EVENT_UNLOAD_MODULE:
        return EventTypeUnloadModule;
    case DEBUG_EVENT_SYSTEM_ERROR:
        return EventTypeSystemError;
    case DEBUG_EVENT_SESSION_STATUS:
        return EventTypeSessionStatus;
    case DEBUG_EVENT_CHANGE_DEBUGGEE_STATE:
        return EventTypeChangeDebuggeeState;
    case DEBUG_EVENT_CHANGE_ENGINE_STATE:
        return EventTypeChangeEngineState;
    case DEBUG_EVENT_CHANGE_SYMBOL_STATE:
        return EventTypeChangeSymbolState;
    }

    throw WrongEventTypeException(eventType);
}

ExceptionInfoPtr getLastExceptionInfo()
{
    ULONG eventType;
    ULONG ProcessId;
    ULONG ThreadId;
    DEBUG_LAST_EVENT_INFO_EXCEPTION LastException;
    ULONG retLength = sizeof(LastException);
    HRESULT hres = 
        g_dbgEng->control->GetLastEventInformation(
            &eventType,
            &ProcessId,
            &ThreadId,
            &LastException,
            sizeof(LastException),
            &retLength,
            NULL,
            0,
            NULL);
    if (S_OK != hres)
        throw DbgException("IDebugControl::GetLastEventInformation", hres);

    if (DEBUG_EVENT_EXCEPTION != eventType)
        throw WrongEventTypeException(eventType);

    return ExceptionInfoPtr( 
        new ExceptionInfo(LastException.FirstChance, LastException.ExceptionRecord)
    );
}

void readBugCheckData(BUG_CHECK_DATA &bugCheckData)
{
    HRESULT hres = 
        g_dbgEng->control->ReadBugCheckData(
            &bugCheckData.code,
            &bugCheckData.arg1,
            &bugCheckData.arg2,
            &bugCheckData.arg3,
            &bugCheckData.arg4);
    if (S_OK != hres)
        throw DbgException("IDebugControl::GetLastEventInformation", hres);
}

}

////////////////////////////////////////////////////////////////////////////////

