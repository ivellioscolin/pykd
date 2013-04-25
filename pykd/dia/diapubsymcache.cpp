
#include "stdafx.h"

#include <set>
#include "dbghelp.h"

#include "dia\diadecls.h"
#include "dia\diapubsymcache.h"

#include "win\utils.h"

namespace pykd {

//////////////////////////////////////////////////////////////////////////////

DiaPublicSymbolCache::DiaPublicSymbolCache(DiaSymbolPtr globalScope)
{
    const CTime startTime = CTime::GetCurrentTime();

    DiaEnumSymbolsPtr publicSymbols;
    HRESULT hres = 
        globalScope->findChildren(
            ::SymTagPublicSymbol,
            NULL,
            nsNone,
            &publicSymbols);
    if (S_OK != hres)
        throw DiaException("Call IDiaSymbol::findChildren", hres);

    std::set< std::string > ambiguousSymbols;

    DiaSymbolPtr currentSym;
    ULONG celt;
    while ( SUCCEEDED(publicSymbols->Next(1, &currentSym, &celt)) && (celt == 1) )
    {
        struct ClearSymbol : boost::noncopyable
        {
            DiaSymbolPtr &m_sym;
            ClearSymbol(DiaSymbolPtr &sym) : m_sym(sym) {}
            ~ClearSymbol() { m_sym = NULL; }
        } clearSymbol(currentSym);

        BSTR bstrTemp;
        hres = currentSym->get_name(&bstrTemp);
        if (hres != S_OK)
            continue;
        std::string name = autoBstr(bstrTemp).asStr();

        hres = currentSym->get_undecoratedNameEx(UNDNAME_NAME_ONLY, &bstrTemp);
        if (hres != S_OK)
            continue;
        std::string undecoratedName = autoBstr(bstrTemp).asStr();

        if (undecoratedName.empty() || undecoratedName == name)
            continue;

        if (ambiguousSymbols.find(undecoratedName) != ambiguousSymbols.end())
            continue;

        if (m_impl.find(undecoratedName) != m_impl.end())
        {
            ambiguousSymbols.insert(undecoratedName);
            m_impl.erase(undecoratedName);
        }

        m_impl[undecoratedName] = currentSym;
    }

    m_buildTimeInSeconds = 
        (CTime::GetCurrentTime() - startTime).GetTotalSeconds();
}

//////////////////////////////////////////////////////////////////////////////

DiaSymbolPtr DiaPublicSymbolCache::lookup(const std::string &name) const
{
    Impl::const_iterator it = m_impl.find(name);
    return (it != m_impl.end()) ? it->second : DiaSymbolPtr();
}

///////////////////////////////////////////////////////////////////////////////

}; // pykd namespace end

