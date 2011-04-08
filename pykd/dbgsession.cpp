#include "stdafx.h"

//#include "dbgext.h"
//#include "dbgeventcb.h"
//#include "dbgsession.h"
//
//DbgExt      dbgGlobalSession;
//
//bool        dbgSessionStarted = false;
//
//void
//dbgCreateSession()
//{
//    IDebugClient4     *client = NULL;
//    DebugCreate( __uuidof(IDebugClient4), (void **)&client );
//
//    SetupDebugEngine( client, &dbgGlobalSession );
//    dbgExt = &dbgGlobalSession;
//
//    setDbgSessionStarted();
//}
//
//bool
//dbgIsSessionStart()
//{
//    return dbgSessionStarted;
//}