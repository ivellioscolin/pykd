// 
// Cache of loaded executable module symbols:
// Map: module -> symbol session
// 

#include "stdafx.h"
#include "dbgengine.h"
#include "symsessioncache.h"

//////////////////////////////////////////////////////////////////////////////

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////////

namespace
{



/////////////////////////////////////////////////////////////////////////////////////

class Impl : protected DEBUG_EVENT_CALLBACK {
public:
    Impl() {
        eventRegisterCallbacks(this);
    }
    ~Impl() {
        eventRemoveCallbacks(this);
    }

    bool find(const SymCacheModuleKey &cacheKey, SymbolSessionPtr &symSession);

    void insert(ULONG64 modBase, const SymCacheModuleKey &cacheKey, SymbolSessionPtr symSession);

    void erase(const SymCacheModuleKey &cacheKey);

    void clear();

protected:

    // DEBUG_EVENT_CALLBACK:
    virtual DEBUG_CALLBACK_RESULT OnBreakpoint( ULONG ) {
        return DebugCallbackNoChange;
    }
    virtual DEBUG_CALLBACK_RESULT OnModuleLoad( ULONG64, const std::string & ) {
        return DebugCallbackNoChange;
    }
    virtual DEBUG_CALLBACK_RESULT OnModuleUnload( ULONG64, const std::string & );
    virtual DEBUG_CALLBACK_RESULT OnException( ExceptionInfoPtr ) {
        return DebugCallbackNoChange;
    }
    virtual void onExecutionStatusChange( ULONG ) {
    }
    virtual void onSymbolsLoaded(ULONG64 modBase) {
    }
    virtual void onSymbolsUnloaded(ULONG64 modBase OPTIONAL);

private:
    typedef std::map< ULONG64, SymCacheModuleKey > Base2Key;
    Base2Key m_base2Key;
    boost::recursive_mutex m_base2KeyLock;

    typedef std::map< SymCacheModuleKey, SymbolSessionPtr > Key2Sym;
    Key2Sym m_key2Sym;
    boost::recursive_mutex m_key2SymLock;
};

/////////////////////////////////////////////////////////////////////////////////////

void Impl::onSymbolsUnloaded(ULONG64 modBase)
{
    if (!modBase)
    {
        boost::recursive_mutex::scoped_lock l(m_key2SymLock);
        m_key2Sym.clear();
        return;
    }

    SymCacheModuleKey cacheKey;
    {
        boost::recursive_mutex::scoped_lock l(m_base2KeyLock);
        Base2Key::const_iterator it = m_base2Key.find(modBase);
        if (it == m_base2Key.end())
            return;
        cacheKey = it->second;
    }

    boost::recursive_mutex::scoped_lock l(m_key2SymLock);
    m_key2Sym.erase(cacheKey);
}

/////////////////////////////////////////////////////////////////////////////////////

bool Impl::find(const SymCacheModuleKey &cacheKey, SymbolSessionPtr &symSession)
{
    boost::recursive_mutex::scoped_lock l(m_key2SymLock);
    Key2Sym::const_iterator it = m_key2Sym.find(cacheKey);
    if (it == m_key2Sym.end())
    {
        symSession.reset();
        return false;
    }

    symSession = it->second;
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////

void Impl::insert(ULONG64 modBase, const SymCacheModuleKey &cacheKey, SymbolSessionPtr symSession)
{
    {
        boost::recursive_mutex::scoped_lock l(m_base2KeyLock);
        m_base2Key[modBase] = cacheKey;
    }

    {
        boost::recursive_mutex::scoped_lock l(m_key2SymLock);
        m_key2Sym[cacheKey] = symSession;
    }
}

/////////////////////////////////////////////////////////////////////////////////////

void Impl::erase(const SymCacheModuleKey &cacheKey)
{
    boost::recursive_mutex::scoped_lock l(m_key2SymLock);
    m_key2Sym.erase(cacheKey);
}

/////////////////////////////////////////////////////////////////////////////////////

void Impl::clear()
{
    boost::recursive_mutex::scoped_lock l(m_key2SymLock);
    m_key2Sym.clear();
}

/////////////////////////////////////////////////////////////////////////////////////

DEBUG_CALLBACK_RESULT Impl::OnModuleUnload( ULONG64 modBase, const std::string & )
{
    boost::recursive_mutex::scoped_lock l(m_base2KeyLock);
    if (modBase)
        m_base2Key.erase(modBase);
    else
        m_base2Key.clear();
    return DebugCallbackNoChange;
}

/////////////////////////////////////////////////////////////////////////////////////

Impl &getImpl()
{
    // construct after DebugEngine
    static Impl g_Impl;
    return g_Impl;
}

/////////////////////////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////////////

bool findSymCacheEntry(const SymCacheModuleKey &cacheKey, SymbolSessionPtr &symSession)
{
    return getImpl().find(cacheKey, symSession);
}

/////////////////////////////////////////////////////////////////////////////////////

void insertSymCacheEntry(ULONG64 modBase, const SymCacheModuleKey &cacheKey, SymbolSessionPtr symSession)
{
    return getImpl().insert(modBase, cacheKey, symSession);
}

/////////////////////////////////////////////////////////////////////////////////////

void eraseSymCacheEntry(const SymCacheModuleKey &cacheKey)
{
    return getImpl().erase(cacheKey);
}

/////////////////////////////////////////////////////////////////////////////////////

void clearSymCache()
{
    return getImpl().clear();
}

/////////////////////////////////////////////////////////////////////////////////////

}   // namespace pykd

//////////////////////////////////////////////////////////////////////////////
