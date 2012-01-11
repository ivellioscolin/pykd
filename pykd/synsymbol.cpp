
#include "stdafx.h"

#include <boost/algorithm/string.hpp>

#include "dbgmem.h"
#include "dbgexcept.h"
#include "dbgclient.h"
#include "synsymhelpers.h"

namespace pykd {


////////////////////////////////////////////////////////////////////////////////

SyntheticSymbols::SyntheticSymbols(
    IDebugSymbols3 &symbols,
    DebugClient &dbgClient
)   : m_symbols(symbols)
    , m_dbgClient(dbgClient)
{
    m_allSymbolsLock.reset(new boost::recursive_mutex);
}

////////////////////////////////////////////////////////////////////////////////

void SyntheticSymbols::add(
    ULONG64 addr,
    ULONG size,
    const std::string &symName
)
{
    addr = m_dbgClient.addr64(addr);

    // add new synthetic symbol to debug engine
    DEBUG_MODULE_AND_ID dbgModuleAndId = { 0 };
    HRESULT hres = 
        m_symbols.AddSyntheticSymbol(
            addr,
            size,
            symName.c_str(),
            DEBUG_ADDSYNTHSYM_DEFAULT,
            &dbgModuleAndId);
    if ( FAILED( hres ) )
        throw DbgException("IDebugSymbols3::AddSyntheticSymbol", hres);

    // add/update symbol for target module (in internal map)
    SymbolsScopedLock lock(*m_allSymbolsLock);

    ModSymbols &modSymbols = 
        m_allSymbols[SynSymHelper(m_symbols).modByBase(dbgModuleAndId.ModuleBase)];
    SymbolData &symData = modSymbols[addr - dbgModuleAndId.ModuleBase];

    symData.m_name = symName;
    symData.m_size = size;
}

////////////////////////////////////////////////////////////////////////////////

ULONG SyntheticSymbols::remove(ULONG64 addr)
{
    addr = m_dbgClient.addr64(addr);

    SynSymHelper synSymHelper(m_symbols);

    // find target module
    ULONG64 moduleBase;
    SymbolsScopedLock lock(*m_allSymbolsLock);
    AllSymbols::iterator itModSymbols = 
        m_allSymbols.find(synSymHelper.modByOffset(addr, moduleBase));
    if (itModSymbols == m_allSymbols.end())
        return 0;

    // remove symbol from internal map
    itModSymbols->second.erase(addr - moduleBase);

    // remove symbol from debug engine
    return synSymHelper.removeSyntheticSymbols(addr);
}

////////////////////////////////////////////////////////////////////////////////

ULONG SyntheticSymbols::removeByMask(
    const std::string &moduleName,
    const std::string &symName
)
{
    SynSymHelper synSymHelper(m_symbols);

    std::vector<DEBUG_MODULE_AND_ID> dbgSymbols;
    HRESULT hres = 
        synSymHelper.getSymbolsByMaks(
            moduleName + "!" + symName,
            dbgSymbols);
    if (FAILED(hres))
        throw DbgException("IDebugSymbols3::GetSymbolEntriesByName", hres);

    if (dbgSymbols.empty())
        return 0;

    ULONG removed = 0;

    SymbolsScopedLock lock(*m_allSymbolsLock);
    for (ULONG i =0; i < dbgSymbols.size(); ++i)
    {
        DEBUG_SYMBOL_ENTRY dbgSymbolInfo;
        HRESULT hres = 
            m_symbols.GetSymbolEntryInformation(
                &dbgSymbols[i],
                &dbgSymbolInfo);
        if (FAILED(hres))
            continue;

        // try find modules in internal map
        AllSymbols::iterator itModSymbols = 
            m_allSymbols.find(synSymHelper.modByBase(dbgSymbolInfo.ModuleBase));
        if (itModSymbols == m_allSymbols.end())
            continue;

        // try find symbol
        ModSymbols::iterator itSymbol = 
            itModSymbols->second.find(dbgSymbolInfo.Offset - dbgSymbolInfo.ModuleBase);
        if (itSymbol == itModSymbols->second.end())
            continue;

        // remove from debug engine
        if (synSymHelper.removeSyntheticSymbols(dbgSymbols[i]))
            ++removed;

        // remove from internal map
        itModSymbols->second.erase(itSymbol);

        if (itModSymbols->second.empty())
            m_allSymbols.erase(itModSymbols);
    }

    return removed;
}

////////////////////////////////////////////////////////////////////////////////

void SyntheticSymbols::clear()
{
    SymbolsScopedLock lock(*m_allSymbolsLock);

    // clean symbols from debug engine
    forEachLoadedModule( SynSymRemoveAll(m_symbols) );

    // clean internal map
    m_allSymbols.clear();
}

////////////////////////////////////////////////////////////////////////////////

void SyntheticSymbols::restoreForModule(ULONG64 moduleBase)
{
    SymbolsScopedLock lock(*m_allSymbolsLock);

    SynSymRestore restorer(m_symbols);
    AllSymbols::iterator itFoundModule = 
        m_allSymbols.find( restorer.modByBase(moduleBase) );
    if (itFoundModule == m_allSymbols.end())
        return;

    forEachFromModule(moduleBase, itFoundModule->second, restorer);
}

////////////////////////////////////////////////////////////////////////////////

void SyntheticSymbols::restoreAll()
{
    SymbolsScopedLock lock(*m_allSymbolsLock);

    // clean symbols from debug engine
    forEachLoadedModule( SynSymRestore(m_symbols) );
}

////////////////////////////////////////////////////////////////////////////////

ULONG64 SyntheticSymbols::getRvaByName(
    ULONG timeDataStamp,
    ULONG checkSumm,
    const std::string &symName
)
{
    SymbolsScopedLock lock(*m_allSymbolsLock);
    AllSymbols::iterator itFoundModule = 
        m_allSymbols.find( ModuleId(timeDataStamp, checkSumm) );
    if (itFoundModule == m_allSymbols.end())
        throw DbgException("Synthetic symbol is not found");


    ModSymbols::iterator itSynSymbol = itFoundModule->second.begin();
    while (itSynSymbol != itFoundModule->second.end())
    {
        if (boost::iequals(symName, itSynSymbol->second.m_name))
            return itSynSymbol->first;

        ++itSynSymbol;
    }
    throw DbgException("Synthetic symbol is not found");
}

////////////////////////////////////////////////////////////////////////////////

void SyntheticSymbols::forEachFromModule(
    ULONG64 moduleBase,
    const ModSymbols &modSymbols,
    ISynSymForEach &forEach
)
{
    ModSymbols::const_iterator itSynSymbol = modSymbols.begin();
    while (itSynSymbol != modSymbols.end())
    {
        forEach.symbol(moduleBase, itSynSymbol->first, itSynSymbol->second);
        ++itSynSymbol;
    }
}

////////////////////////////////////////////////////////////////////////////////

void SyntheticSymbols::forEachLoadedModule(ISynSymForEach &forEach)
{
    // for loaded and unloaded modules...
    std::vector<DEBUG_MODULE_PARAMETERS> dbgModules;
    if (!SUCCEEDED(forEach.getAllModules(dbgModules)) || dbgModules.empty())
    {
        SymbolsScopedLock lock(*m_allSymbolsLock);
        m_allSymbols.clear();
        return;
    }

    std::vector<DEBUG_MODULE_PARAMETERS>::iterator itModule = dbgModules.begin();
    while (itModule != dbgModules.end())
    {
        // ...do it!
        AllSymbols::const_iterator itFoundModule = 
            m_allSymbols.find( ModuleId(*itModule) );
        if (itFoundModule != m_allSymbols.end())
            forEachFromModule(itModule->Base, itFoundModule->second, forEach);

        ++itModule;
    }
}

////////////////////////////////////////////////////////////////////////////////

};  // namespace pykd
