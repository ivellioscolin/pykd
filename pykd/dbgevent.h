/////////////////////////////////////////////////////////////////////////////////
// Load/Unload module events
/////////////////////////////////////////////////////////////////////////////////

#include <set>

#include <boost/interprocess/sync/interprocess_recursive_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

interface debugEvent
{
    debugEvent();
    virtual ~debugEvent();

    virtual ULONG onLoadModule(const dbgModuleClass &/* module */)
    {
        return DEBUG_STATUS_NO_CHANGE;
    }
    virtual ULONG onUnloadModule(const dbgModuleClass &/* module */)
    {
        return DEBUG_STATUS_NO_CHANGE;
    }

    // call from debug engine
    static ULONG moduleLoaded(__in ULONG64 addr);
    static ULONG moduleUnloaded(__in ULONG64 addr);

private:

    typedef std::set<debugEvent *> modCallbacksColl;
    static modCallbacksColl modCallbacks;

    typedef boost::interprocess::interprocess_recursive_mutex modCallbacksLock;
    static modCallbacksLock modCallbacksMtx;
    typedef boost::interprocess::scoped_lock<modCallbacksLock> modCallbacksScopedLock;
};

// python wrapper for debugEvent
struct debugEventWrap : debugEvent, boost::python::wrapper<debugEvent>
{
    ULONG onLoadModule(const dbgModuleClass &module);
    ULONG onLoadModuleDef(const dbgModuleClass &module) 
    {
        return debugEvent::onLoadModule(module);
    }

    ULONG onUnloadModule(const dbgModuleClass &module);
    ULONG onUnloadModuleDef(const dbgModuleClass &module)
    {
        return debugEvent::onUnloadModule(module);
    }
};

/////////////////////////////////////////////////////////////////////////////////
