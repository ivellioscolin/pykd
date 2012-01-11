
#include <stdafx.h>

#include "dbgexcept.h"
#include "synsymhelpers.h"

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

bool SynSymHelper::removeSyntheticSymbols(
    const DEBUG_MODULE_AND_ID &dbgSymbols
)
{
    return SUCCEEDED(
        m_symbols.RemoveSyntheticSymbol(
            const_cast<DEBUG_MODULE_AND_ID *>(&dbgSymbols)
        )
    );
}

////////////////////////////////////////////////////////////////////////////////

ULONG SynSymHelper::removeSyntheticSymbols(
    const std::vector<DEBUG_MODULE_AND_ID> &arrSymbols
)
{
    ULONG countOfRemoved = 0;
    for (ULONG i = 0; i < arrSymbols.size(); ++i)
    {
        if (removeSyntheticSymbols(arrSymbols[i]))
            ++countOfRemoved;
    }
    return countOfRemoved;
}

////////////////////////////////////////////////////////////////////////////////

ULONG SynSymHelper::removeSyntheticSymbols(ULONG64 addr)
{
    ULONG entries = 0;
    m_symbols.GetSymbolEntriesByOffset(addr, 0, NULL, NULL, 0, &entries);
    if (!entries)
        return 0;

    std::vector<DEBUG_MODULE_AND_ID> arrSymbols(entries);
    HRESULT hres = m_symbols.GetSymbolEntriesByOffset(
        addr,
        0,
        &arrSymbols[0],
        NULL,
        (ULONG)arrSymbols.size(),
        NULL);
    if (SUCCEEDED(hres))
        return removeSyntheticSymbols(arrSymbols);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

SyntheticSymbols::ModuleId SynSymHelper::modByBase(ULONG64 moduleBase)
{
    DEBUG_MODULE_PARAMETERS dbgModuleParameters;
    HRESULT hres = 
        m_symbols.GetModuleParameters(
            1,
            &moduleBase,
            0,
            &dbgModuleParameters);
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbols3::GetModuleParameters", hres );

    return SyntheticSymbols::ModuleId(dbgModuleParameters);
}

////////////////////////////////////////////////////////////////////////////////

SyntheticSymbols::ModuleId SynSymHelper::modByOffset(
    ULONG64 moduleOffset,
    ULONG64 &moduleBase
)
{
    HRESULT hres = 
        m_symbols.GetModuleByOffset(moduleOffset, 0, NULL, &moduleBase);
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbols3::GetModuleByOffset", hres );

    return modByBase(moduleBase);
}

////////////////////////////////////////////////////////////////////////////////

HRESULT SynSymHelper::getAllModules(
    std::vector<DEBUG_MODULE_PARAMETERS> &dbgModules
)
{
    ULONG nLoaded;
    ULONG nUnloaded;
    HRESULT hres = m_symbols.GetNumberModules(&nLoaded, &nUnloaded);
    if (FAILED(hres))
        return hres;

    if (!nLoaded && !nUnloaded)
    {
        dbgModules.clear();
        return S_OK;
    }

    dbgModules.resize(nLoaded + nUnloaded);
    return 
        m_symbols.GetModuleParameters(
            nLoaded + nUnloaded,
            NULL,
            0,
            &dbgModules[0]);
}

////////////////////////////////////////////////////////////////////////////////

HRESULT SynSymHelper::getSymbolsByMaks(
    const std::string &symMask,
    std::vector<DEBUG_MODULE_AND_ID> &dbgSymbols
)
{
    ULONG entries = 0;
    m_symbols.GetSymbolEntriesByName(
        symMask.c_str(),
        0,
        NULL,
        0,
        &entries);
    if (!entries)
    {
        dbgSymbols.clear();
        return S_OK;
    }

    dbgSymbols.resize(entries);

    return 
        m_symbols.GetSymbolEntriesByName(
            symMask.c_str(),
            0,
            &dbgSymbols[0],
            entries,
            NULL);
}

////////////////////////////////////////////////////////////////////////////////

};  // namespace pykd
