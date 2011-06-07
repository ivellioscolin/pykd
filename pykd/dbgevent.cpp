/////////////////////////////////////////////////////////////////////////////////
// Load/Unload module events
/////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <memory>
#include <dbgeng.h>

#include "dbgmodule.h"
#include "dbgio.h"
#include "dbgevent.h"

/////////////////////////////////////////////////////////////////////////////////

debugEvent::modCallbacksColl debugEvent::modCallbacks;
debugEvent::modCallbacksLock debugEvent::modCallbacksMtx;

/////////////////////////////////////////////////////////////////////////////////

debugEvent::debugEvent()
{
    modCallbacksScopedLock lock(modCallbacksMtx);
    modCallbacks.insert(this);
}

/////////////////////////////////////////////////////////////////////////////////

debugEvent::~debugEvent()
{
    modCallbacksScopedLock lock(modCallbacksMtx);
    modCallbacks.erase(this);
}

/////////////////////////////////////////////////////////////////////////////////

ULONG debugEvent::moduleLoaded(__in ULONG64 addr)
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
        const ULONG retValue = (*itCallback)->onLoadModule(module);
        if (DEBUG_STATUS_NO_CHANGE != retValue)
            return retValue;

        ++itCallback;
    }
    return DEBUG_STATUS_NO_CHANGE;
}

/////////////////////////////////////////////////////////////////////////////////

ULONG debugEvent::moduleUnloaded(__in ULONG64 addr)
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
        const ULONG retValue = (*itCallback)->onUnloadModule(module);
        if (DEBUG_STATUS_NO_CHANGE != retValue)
            return retValue;

        ++itCallback;
    }
    return DEBUG_STATUS_NO_CHANGE;
}

/////////////////////////////////////////////////////////////////////////////////

ULONG debugEventWrap::onLoadModule(const dbgModuleClass &module)
{
    if (boost::python::override override = get_override("onLoadModule"))
        return override(module);

    return debugEvent::onLoadModule(module);
}

/////////////////////////////////////////////////////////////////////////////////

ULONG debugEventWrap::onUnloadModule(const dbgModuleClass &module)
{
    if (boost::python::override override = get_override("onUnloadModule"))
        return override(module);

    return debugEvent::onUnloadModule(module);
}

/////////////////////////////////////////////////////////////////////////////////
