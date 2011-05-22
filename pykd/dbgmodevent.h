/////////////////////////////////////////////////////////////////////////////////
// Load/Unload module events
/////////////////////////////////////////////////////////////////////////////////

#include <set>

#include <boost/interprocess/sync/interprocess_recursive_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

interface moduleEvents
{
    moduleEvents();
    virtual ~moduleEvents();

    virtual ULONG onLoad(const dbgModuleClass &/* module */)
    {
        return DEBUG_STATUS_NO_CHANGE;
    }
    virtual ULONG onUnload(const dbgModuleClass &/* module */)
    {
        return DEBUG_STATUS_NO_CHANGE;
    }

    static ULONG onLoadModule(__in ULONG64 addr);
    static ULONG onUnloadModule(__in ULONG64 addr);

private:

    typedef std::set<moduleEvents *> modCallbacksColl;
    static modCallbacksColl modCallbacks;

    typedef boost::interprocess::interprocess_recursive_mutex modCallbacksLock;
    static modCallbacksLock modCallbacksMtx;
    typedef boost::interprocess::scoped_lock<modCallbacksLock> modCallbacksScopedLock;
};

// python wrapper for moduleEvents
struct moduleEventsWrap : moduleEvents, boost::python::wrapper<moduleEvents>
{
    ULONG onLoad(const dbgModuleClass &module);
    ULONG onLoadDef(const dbgModuleClass &module) 
    {
        return moduleEvents::onLoad(module);
    }

    ULONG onUnload(const dbgModuleClass &module);
    ULONG onUnloadDef(const dbgModuleClass &module)
    {
        return moduleEvents::onUnload(module);
    }
};

/////////////////////////////////////////////////////////////////////////////////
