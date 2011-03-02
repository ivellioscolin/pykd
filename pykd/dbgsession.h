#pragma once

void
dbgCreateSession();

extern
bool    dbgSessionStarted;

inline void setDbgSessionStarted()
{
    dbgEventCallbacks.Register();

    dbgSessionStarted = true;
}

bool
dbgIsSessionStart();

