/////////////////////////////////////////////////////////////////////////////////
// Load/Unload module events
/////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <memory>
#include <dbgeng.h>

#include "dbgmodule.h"
#include "dbgcallback.h"
#include "dbgmodevent.h"

/////////////////////////////////////////////////////////////////////////////////

moduleEvents::modCallbacksColl moduleEvents::modCallbacks;
moduleEvents::modCallbacksLock moduleEvents::modCallbacksMtx;

/////////////////////////////////////////////////////////////////////////////////

moduleEvents::moduleEvents()
{
    modCallbacksScopedLock lock(modCallbacksMtx);
    modCallbacks.insert(this);
}

/////////////////////////////////////////////////////////////////////////////////

moduleEvents::~moduleEvents()
{
    modCallbacksScopedLock lock(modCallbacksMtx);
    modCallbacks.erase(this);
}

/////////////////////////////////////////////////////////////////////////////////

ULONG moduleEvents::onLoadModule(__in ULONG64 addr)
{
    modCallbacksScopedLock lock(modCallbacksMtx);
    if (modCallbacks.empty())
        return DEBUG_STATUS_NO_CHANGE;

    ULONG64 moduleBase;
    ULONG moduleSize;
    std::string moduleName;

    std::auto_ptr<OutputReader> silentMode( new OutputReader(dbgExt->client) );
    queryModuleParams(addr, moduleName, moduleBase, moduleSize);
    dbgModuleClass module(moduleName, moduleBase, moduleSize);
    silentMode.reset();

    modCallbacksColl::iterator itCallback = modCallbacks.begin();
    while (itCallback != modCallbacks.end())
    {
        const ULONG retValue = (*itCallback)->onLoad(module);
        if (DEBUG_STATUS_NO_CHANGE != retValue)
            return retValue;

        ++itCallback;
    }
    return DEBUG_STATUS_NO_CHANGE;
}

/////////////////////////////////////////////////////////////////////////////////

ULONG moduleEvents::onUnloadModule(__in ULONG64 addr)
{
    modCallbacksScopedLock lock(modCallbacksMtx);
    if (modCallbacks.empty())
        return DEBUG_STATUS_NO_CHANGE;

    ULONG64 moduleBase;
    ULONG moduleSize;
    std::string moduleName;

    std::auto_ptr<OutputReader> silentMode( new OutputReader(dbgExt->client) );
    queryModuleParams(addr, moduleName, moduleBase, moduleSize);
    dbgModuleClass module(moduleName, moduleBase, moduleSize);
    silentMode.reset();

    modCallbacksColl::iterator itCallback = modCallbacks.begin();
    while (itCallback != modCallbacks.end())
    {
        const ULONG retValue = (*itCallback)->onUnload(module);
        if (DEBUG_STATUS_NO_CHANGE != retValue)
            return retValue;

        ++itCallback;
    }
    return DEBUG_STATUS_NO_CHANGE;
}

/////////////////////////////////////////////////////////////////////////////////

ULONG moduleEventsWrap::onLoad(
    const dbgModuleClass &module
)
{
    if (boost::python::override override = get_override("onLoad"))
        return override(module);

    return moduleEvents::onLoad(module);
}

/////////////////////////////////////////////////////////////////////////////////

ULONG moduleEventsWrap::onUnload(
    const dbgModuleClass &module
)
{
    if (boost::python::override override = get_override("onUnload"))
        return override(module);

    return moduleEvents::onUnload(module);
}

/////////////////////////////////////////////////////////////////////////////////
