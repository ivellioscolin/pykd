#include "stdafx.h"

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_recursive_mutex.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <vector>
#include <list>

#include "dbgext.h"
#include "dbgmem.h"
#include "dbgmodule.h"
#include "dbgexcept.h"
#include "dbgsynsym.h"

/////////////////////////////////////////////////////////////////////////////////

struct SyntheticSymbol
{
    std::string m_name;
    ULONG m_size;

    SyntheticSymbol()
      : m_size(0)
    {
    }
    SyntheticSymbol(
        const std::string &name
    ) : m_name(name)
      , m_size(0)
    {
    }
    SyntheticSymbol(
        const SyntheticSymbol &rhs
    ) : m_name(rhs.m_name)
      , m_size(rhs.m_size)
    {
    }

    bool operator ==(const SyntheticSymbol &rhs) const
    {
        return m_name == rhs.m_name;
    }
};

/////////////////////////////////////////////////////////////////////////////////

// map of one module synthetic symbols
typedef std::map<ULONG64, SyntheticSymbol> SynSymbolsForModule;

// global synthetic symbols map by modules
typedef std::map<ModuleInfo, SynSymbolsForModule> SynSymbolsMap;

// synchro-object for global synthetic symbols map
typedef boost::interprocess::interprocess_recursive_mutex SynSymbolsMapLockType;
typedef boost::interprocess::interprocess_mutex SynSymbolsMapLockWriteType;

// scoped lock synchro-object for global synthetic symbols map
typedef boost::interprocess::scoped_lock<SynSymbolsMapLockType> SynSymbolsMapScopedLock;
typedef boost::interprocess::scoped_lock<SynSymbolsMapLockWriteType> SynSymbolsMapScopedLockWrite;

static struct _GlobalSyntheticSymbolMap : public SynSymbolsMap
{
    SynSymbolsMapLockType m_Lock;
    SynSymbolsMapLockWriteType m_LockWrite;
} g_SyntheticSymbolMap;

#define _SynSymbolsMapScopedLock()  \
    SynSymbolsMapScopedLock _lock(g_SyntheticSymbolMap.m_Lock)
#define _SynSymbolsMapScopedLockWrite() \
    _SynSymbolsMapScopedLock();         \
    SynSymbolsMapScopedLockWrite _lockw(g_SyntheticSymbolMap.m_LockWrite)


/////////////////////////////////////////////////////////////////////////////////

bool addSyntheticSymbol(
    ULONG64 addr,
    ULONG size,
    const std::string &symName
)
{
    try
    {

        DEBUG_MODULE_AND_ID dbgModuleAndId = { 0 };
        addr = addr64(addr);
        HRESULT hres = 
            dbgExt->symbols3->AddSyntheticSymbol(
                addr,
                size,
                symName.c_str(),
                DEBUG_ADDSYNTHSYM_DEFAULT,
                &dbgModuleAndId);
        if ( FAILED( hres ) )
            throw DbgException( "call IDebugSymbol3::AddSyntheticSymbol(...) failed" );

        DEBUG_MODULE_PARAMETERS dbgModuleParameters;
        hres = dbgExt->symbols->GetModuleParameters(
            1, 
            &dbgModuleAndId.ModuleBase,
            0,
            &dbgModuleParameters);
        if ( FAILED( hres ) )
        {
            dbgExt->symbols3->RemoveSyntheticSymbol(&dbgModuleAndId);
            throw DbgException( "call IDebugSymbol3::GetModuleParameters(...) failed" );
        }

        _SynSymbolsMapScopedLockWrite();

        ModuleInfo moduleInfo(dbgModuleParameters);
        SynSymbolsForModule &mapSynSymbolsForModule = 
            g_SyntheticSymbolMap[moduleInfo];

        ULONG64 addrSymbolOffset = addr - moduleInfo.m_base;
        SyntheticSymbol &synSymbol = mapSynSymbolsForModule[addrSymbolOffset];

        synSymbol.m_name = symName;
        synSymbol.m_size = size;

        return true;
    }
    catch( std::exception  &e )
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
    }
    catch(...)
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////////

bool addSyntheticSymbolForModule(
    ULONG64 offset,
    ULONG size,
    const std::string &symName,
    const ModuleInfo &moduleInfo
)
{
    try
    {
        DEBUG_MODULE_AND_ID dbgModuleAndId = { 0 };
        offset = addr64(offset);
        HRESULT hres = 
            dbgExt->symbols3->AddSyntheticSymbol(
                offset + moduleInfo.m_base,
                size,
                symName.c_str(),
                DEBUG_ADDSYNTHSYM_DEFAULT,
                &dbgModuleAndId);
        if ( FAILED( hres ) )
            throw DbgException( "call IDebugSymbol3::AddSyntheticSymbol(...) failed" );

        {
            DEBUG_MODULE_PARAMETERS dbgModuleParameters;
            hres = dbgExt->symbols->GetModuleParameters(
                1, 
                &dbgModuleAndId.ModuleBase,
                0,
                &dbgModuleParameters);
            if ( FAILED( hres ) )
            {
                dbgExt->symbols3->RemoveSyntheticSymbol(&dbgModuleAndId);
                throw DbgException( "call IDebugSymbol3::GetModuleParameters(...) failed" );
            }
            ModuleInfo gettedModuleInfo(dbgModuleParameters);
            if (!(gettedModuleInfo == moduleInfo))
            {
                // module mismatch
                dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd!" __FUNCTION__ " mismatch modules\n" );
                dbgExt->symbols3->RemoveSyntheticSymbol(&dbgModuleAndId);
                return false;
            }
        }

        _SynSymbolsMapScopedLockWrite();

        SynSymbolsForModule &mapSynSymbolsForModule = 
            g_SyntheticSymbolMap[moduleInfo];

        SyntheticSymbol &synSymbol = mapSynSymbolsForModule[offset];

        synSymbol.m_name = symName;
        synSymbol.m_size = size;

        return true;
    }
    catch( std::exception  &e )
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
    }
    catch(...)
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////////

struct SynSymbolNameCmp
{
    SynSymbolNameCmp(const std::string &symName ) : m_symName(symName) { }
    bool operator() (const std::pair<ULONG64, SyntheticSymbol> &rhs) const 
    {
        return m_symName == rhs.second.m_name;
    }
    const std::string &m_symName;
};

/////////////////////////////////////////////////////////////////////////////////

ULONG64 getSyntheticSymbol(
    const ModuleInfo &moduleInfo,
    const std::string &symName
)
{
    ULONG64 nAddress = 0;

    try
    {
        _SynSymbolsMapScopedLock();

        // try find module
        SynSymbolsMap::const_iterator itSynSymbols = 
            g_SyntheticSymbolMap.find( moduleInfo );
        if (itSynSymbols != g_SyntheticSymbolMap.end())
        {
            // try find symbol by name
            const SynSymbolsForModule &mapSynSymbolsForModule = itSynSymbols->second;
            SynSymbolNameCmp nameCmp(symName);

            // FIXME: get first matched by name
            SynSymbolsForModule::const_iterator itSynSymbol  = 
                find_if(
                    mapSynSymbolsForModule.begin(),
                    mapSynSymbolsForModule.end(),
                    nameCmp);
            if (itSynSymbol != mapSynSymbolsForModule.end())
                nAddress = moduleInfo.m_base + itSynSymbol->first;
        }
    }
    catch( std::exception  &e )
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
    }
    catch(...)
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
    }

    return nAddress;
}

/////////////////////////////////////////////////////////////////////////////////

// remove synthetic symbols by array of DEBUG_MODULE_AND_ID
inline ULONG RemoveSyntheticSymbols(
    const std::vector<DEBUG_MODULE_AND_ID> &arrSymbols
)
{
    ULONG nResult = 0;
    for (ULONG i = 0; i < arrSymbols.size(); ++i)
    {
        HRESULT hres = 
            dbgExt->symbols3->RemoveSyntheticSymbol(
                const_cast<PDEBUG_MODULE_AND_ID>(&arrSymbols[i]));
        if (FAILED(hres))
            continue;
        ++nResult;
    }
    return nResult;
}

/////////////////////////////////////////////////////////////////////////////////

static ULONG RemoveSynSymbolsByAddress(ULONG64 addr)
{
    ULONG nEntries = 0;
    dbgExt->symbols3->GetSymbolEntriesByOffset(
        addr,
        0,
        NULL,
        NULL,
        0,
        &nEntries);
    if (nEntries)
    {
        std::vector<DEBUG_MODULE_AND_ID> arrSymbols(nEntries);
        HRESULT hres = dbgExt->symbols3->GetSymbolEntriesByOffset(
            addr,
            0,
            &arrSymbols[0],
            NULL,
            (ULONG)arrSymbols.size(),
            NULL);
        if (SUCCEEDED(hres))
            return RemoveSyntheticSymbols(arrSymbols);
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////

static void DeleteSynSymbolsForModule(
    const std::pair<ModuleInfo, SynSymbolsForModule> &synSymbolsForModule
)
{
    DEBUG_MODULE_PARAMETERS dbgModuleParameters;
    HRESULT hres = dbgExt->symbols->GetModuleParameters(
        1, 
        const_cast<PULONG64>(&synSymbolsForModule.first.m_base),
        0,
        &dbgModuleParameters);
    if ( FAILED( hres ) )
        return;
    ModuleInfo gettedModuleInfo(dbgModuleParameters);
    if (gettedModuleInfo == synSymbolsForModule.first)
    {
        // no clear - global map will be cleared on exit
        SynSymbolsForModule::const_iterator itSynSymbol = 
            synSymbolsForModule.second.begin();
        while (itSynSymbol != synSymbolsForModule.second.end())
        {
            RemoveSynSymbolsByAddress(
                gettedModuleInfo.m_base + itSynSymbol->first);
            ++itSynSymbol;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////

void delAllSyntheticSymbols()
{
    try
    {
        _SynSymbolsMapScopedLockWrite();

        for_each(
            g_SyntheticSymbolMap.begin(),
            g_SyntheticSymbolMap.end(),
            DeleteSynSymbolsForModule);
        g_SyntheticSymbolMap.clear();
    }
    catch( std::exception  &e )
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
    }
    catch(...)
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
    }
}

/////////////////////////////////////////////////////////////////////////////////

void delAllSyntheticSymbolsForModule(
    const ModuleInfo &moduleInfo
)
{
    try
    {
        _SynSymbolsMapScopedLockWrite();

        SynSymbolsMap::iterator itSynSymbols = 
            g_SyntheticSymbolMap.find(moduleInfo);
        if (itSynSymbols != g_SyntheticSymbolMap.end())
        {
            std::pair<ModuleInfo, SynSymbolsForModule> synSymbolsForModule(
                moduleInfo, itSynSymbols->second);
            DeleteSynSymbolsForModule(synSymbolsForModule);
            g_SyntheticSymbolMap.erase(itSynSymbols);
        }
    }
    catch( std::exception  &e )
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
    }
    catch(...)
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
    }
}

/////////////////////////////////////////////////////////////////////////////////

ULONG delSyntheticSymbolForModuleNoLock(
    ULONG64 offset,
    const ModuleInfo &moduleInfo
)
{
    SynSymbolsMap::iterator itSynSymbols = 
        g_SyntheticSymbolMap.find(moduleInfo);
    if (itSynSymbols != g_SyntheticSymbolMap.end())
    {
        offset = addr64(offset);
        SynSymbolsForModule::iterator itSynSymbol = 
            itSynSymbols->second.find( offset );
        if (itSynSymbol != itSynSymbols->second.end())
        {
            ULONG nResult = 
                RemoveSynSymbolsByAddress(moduleInfo.m_base + offset);
            itSynSymbols->second.erase(itSynSymbol);
            return nResult;
        }
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////

ULONG delSyntheticSymbol(
    ULONG64 addr
)
{
    try
    {
        // query module information
        addr = addr64(addr);
        ULONG64 ptrModuleBase;
        HRESULT hres = 
            dbgExt->symbols->GetModuleByOffset(addr, 0, NULL, &ptrModuleBase);
        if ( SUCCEEDED(hres) )
        {
            DEBUG_MODULE_PARAMETERS dbgModuleParameters;
            hres = 
                dbgExt->symbols->GetModuleParameters(
                    1,
                    &ptrModuleBase,
                    0,
                    &dbgModuleParameters);
            if ( SUCCEEDED(hres) )
            {
                _SynSymbolsMapScopedLockWrite();

                ModuleInfo moduleInfo(dbgModuleParameters);
                return 
                    delSyntheticSymbolForModuleNoLock(
                        addr - moduleInfo.m_base,
                        moduleInfo);
            }
        }
    }
    catch( std::exception  &e )
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
    }
    catch(...)
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////

ULONG delSyntheticSymbolForModule(
    ULONG64 offset,
    const ModuleInfo &moduleInfo
)
{
    try
    {
        _SynSymbolsMapScopedLockWrite();
        return delSyntheticSymbolForModuleNoLock(offset, moduleInfo);
    }
    catch( std::exception  &e )
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
    }
    catch(...)
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////

// remove synthetic symbols from global map
inline void RemoveSyntheticSymbolsFromMap(
    const std::vector<DEBUG_MODULE_AND_ID> &arrSymbols
)
{
    for (ULONG i = 0; i < arrSymbols.size(); ++i)
    {
        DEBUG_SYMBOL_ENTRY dbgSymbolEntry;
        HRESULT hres = dbgExt->symbols3->GetSymbolEntryInformation(
            const_cast<PDEBUG_MODULE_AND_ID>(&arrSymbols[i]),
            &dbgSymbolEntry);
        if (FAILED(hres))
            continue;

        DEBUG_MODULE_PARAMETERS dbgModuleParameters;
        hres = dbgExt->symbols->GetModuleParameters(
            1,
            &dbgSymbolEntry.ModuleBase,
            0,
            &dbgModuleParameters);
        if (FAILED(hres))
            continue;

        ModuleInfo moduleInfo(dbgModuleParameters);
        SynSymbolsMap::iterator itSynSymbols = 
            g_SyntheticSymbolMap.find(moduleInfo);
        if (itSynSymbols != g_SyntheticSymbolMap.end())
        {
            ULONG64 offset = dbgSymbolEntry.Offset - dbgSymbolEntry.ModuleBase;
            SynSymbolsForModule::iterator itSynSymbol = 
                itSynSymbols->second.find( offset );
            if (itSynSymbol != itSynSymbols->second.end())
                itSynSymbols->second.erase(itSynSymbol);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////

ULONG delSyntheticSymbolsMask(
    const std::string &moduleName,
    const std::string &symName
)
{
    ULONG nResult = 0;

    try
    {
        std::string symMask = moduleName + "!" + symName;
        ULONG nEntries = 0;
        dbgExt->symbols3->GetSymbolEntriesByName(
            symMask.c_str(),
            0,
            NULL,
            0,
            &nEntries);
        if (nEntries)
        {
            std::vector<DEBUG_MODULE_AND_ID> arrSymbols(nEntries);
            HRESULT hres = dbgExt->symbols3->GetSymbolEntriesByName(
                symMask.c_str(),
                0,
                &arrSymbols[0],
                (ULONG)arrSymbols.size(),
                NULL);
            if (FAILED(hres))
                throw DbgException( "call IDebugSymbol3::GetSymbolEntriesByOffset(...) failed" );
            
            {
                _SynSymbolsMapScopedLockWrite();
                RemoveSyntheticSymbolsFromMap(arrSymbols);
            }

            return RemoveSyntheticSymbols(arrSymbols);
        }
    }
    catch( std::exception  &e )
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
    }
    catch(...)
    {
        dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
    }

    return nResult;
}

/////////////////////////////////////////////////////////////////////////////////

static void restoreSyntheticSymbolForModuleNoLock(
    const ModuleInfo &moduleInfo 
)
{
    SynSymbolsMap::const_iterator itSynSymbols = 
        g_SyntheticSymbolMap.find( moduleInfo );
    if (itSynSymbols != g_SyntheticSymbolMap.end())
    {
        SynSymbolsForModule::const_iterator itSynSymbol = 
            itSynSymbols->second.begin();
        while (itSynSymbol != itSynSymbols->second.end())
        {
            DEBUG_MODULE_AND_ID dbgModuleAndId;
            dbgExt->symbols3->AddSyntheticSymbol(
                moduleInfo.m_base + itSynSymbol->first,
                itSynSymbol->second.m_size,
                itSynSymbol->second.m_name.c_str(),
                DEBUG_ADDSYNTHSYM_DEFAULT,
                &dbgModuleAndId);

            ++itSynSymbol;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////

void restoreSyntheticSymbolForModule(
    const ModuleInfo &moduleInfo 
)
{
    _SynSymbolsMapScopedLock();

    // see (**1)
    restoreSyntheticSymbolForModuleNoLock(moduleInfo);
}

/////////////////////////////////////////////////////////////////////////////////

void restoreSyntheticSymbolForAllModules()
{
    try
    {
        _SynSymbolsMapScopedLock();

        if (g_SyntheticSymbolMap.empty())
            return;

        ULONG nLoaded;
        ULONG nUnloaded;

        HRESULT hres = dbgExt->symbols3->GetNumberModules(&nLoaded, &nUnloaded);
        if (SUCCEEDED(hres) && (nLoaded || nUnloaded))
        {
            std::vector<DEBUG_MODULE_PARAMETERS> arrModules(nLoaded + nUnloaded);
            hres = 
                dbgExt->symbols3->GetModuleParameters(
                    (ULONG)arrModules.size(),
                    NULL,
                    0,
                    &arrModules[0]);
            if (SUCCEEDED(hres))
            {
                for (ULONG i = 0; i < arrModules.size(); ++i)
                {
                    ModuleInfo moduleInfo(arrModules[i]);
                    restoreSyntheticSymbolForModuleNoLock(moduleInfo);
                }
            }
        }
    }
    catch (...)
    {
    }
}

/////////////////////////////////////////////////////////////////////////////////
