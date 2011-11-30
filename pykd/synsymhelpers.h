// 
// Synthetic symbol helpers
// 

#pragma once

#include <vector>

#include "synsymbol.h"

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

class SynSymHelper {
public:
    SynSymHelper(IDebugSymbols3 &symbols) : m_symbols(symbols) {}

    // remove array of synthetic symbols from debug engine
    bool removeSyntheticSymbols(
        const DEBUG_MODULE_AND_ID &dbgSymbols
    );
    ULONG removeSyntheticSymbols(
        const std::vector<DEBUG_MODULE_AND_ID> &arrSymbols
    );
    ULONG removeSyntheticSymbols(ULONG64 addr);

    // buid ModuleId by base of image
    SyntheticSymbols::ModuleId modByBase(ULONG64 moduleBase);

    // buid ModuleId by offset in image
    SyntheticSymbols::ModuleId modByOffset(
        ULONG64 moduleOffset,
        ULONG64 &moduleBase
    );

    // get array of all modules
    HRESULT getAllModules(std::vector<DEBUG_MODULE_PARAMETERS> &dbgModules);

    // get symbols by mask
    HRESULT getSymbolsByMaks(
        const std::string &symMask,
        std::vector<DEBUG_MODULE_AND_ID> &dbgSymbols
    );

protected:
    IDebugSymbols3 &m_symbols;
};

////////////////////////////////////////////////////////////////////////////////
// Enumerator all synthetic symbols for loaded modules
////////////////////////////////////////////////////////////////////////////////
interface ISynSymForEach : public SynSymHelper {
    ISynSymForEach(IDebugSymbols3 &symbols) : SynSymHelper(symbols) {}

    virtual void symbol(
        const ULONG64 moduleBase,
        SyntheticSymbols::SYM_OFFSET offset,
        const SyntheticSymbols::SymbolData &symData
    ) = 0;
};


////////////////////////////////////////////////////////////////////////////////
// Remove symbols from loaded modules
////////////////////////////////////////////////////////////////////////////////
class SynSymRemoveAll : public ISynSymForEach
{
public:
    SynSymRemoveAll(IDebugSymbols3 &symbols) : ISynSymForEach(symbols) {}

    virtual void symbol(
        const ULONG64 moduleBase,
        SyntheticSymbols::SYM_OFFSET offset,
        const SyntheticSymbols::SymbolData &/*symData*/
    ) override
    {
        removeSyntheticSymbols(moduleBase + offset);
    }
};

////////////////////////////////////////////////////////////////////////////////
// Restore symbols for loaded modules
////////////////////////////////////////////////////////////////////////////////
class SynSymRestore : public ISynSymForEach
{
public:
    SynSymRestore(IDebugSymbols3 &symbols) : ISynSymForEach(symbols) {}

    virtual void symbol(
        const ULONG64 moduleBase,
        SyntheticSymbols::SYM_OFFSET offset,
        const SyntheticSymbols::SymbolData &symData
    ) override
    {
        m_symbols.AddSyntheticSymbol(
            moduleBase + offset,
            symData.m_size,
            symData.m_name.c_str(),
            DEBUG_ADDSYNTHSYM_DEFAULT,
            NULL);
    }
};

////////////////////////////////////////////////////////////////////////////////

};  // namespace pykd
