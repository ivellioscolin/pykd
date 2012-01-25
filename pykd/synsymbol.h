// 
// Synthetic symbol
// 

#pragma once

#include "dbgexcept.h"
#include <DbgEng.h>
#include <map>

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

interface ISynSymForEach;
class DebugClient;

////////////////////////////////////////////////////////////////////////////////

class SyntheticSymbols {
public:
    SyntheticSymbols(
        IDebugSymbols3 &symbols,
        DebugClient &dbgClient
    );

    ////////////////////////////////////////////////////////////////////////////////
    // Python API:

    // add new synthetic symbol
    void add(
        ULONG64 addr,
        ULONG size,
        const std::string &symName
    );

    // remove synthetic symbols by address
    ULONG remove(ULONG64 addr);

    // remove synthetic symbols by module and symbol names
    ULONG removeByMask(
        const std::string &moduleName,
        const std::string &symName
    );

    // remove all synthetic symbols from all modules
    void clear();

    ////////////////////////////////////////////////////////////////////////////////
    // Debug event callback manager API:

    void restoreForModule(ULONG64 moduleBase);
    void restoreAll();

    ////////////////////////////////////////////////////////////////////////////////
    // Module API:

    ULONG64 getRvaByName(
        ULONG timeDataStamp,
        ULONG checkSumm,
        const std::string &symName
    );

public:

    // unique module ID
    struct ModuleId {
        ULONG m_timeDataStamp;
        ULONG m_checkSumm;

        ModuleId(ULONG timeDataStamp, ULONG checkSumm)
            : m_timeDataStamp(timeDataStamp)
            , m_checkSumm(checkSumm)
        {
        }

        ModuleId(const DEBUG_MODULE_PARAMETERS &dbgModuleParameters)
            : m_timeDataStamp(dbgModuleParameters.TimeDateStamp)
            , m_checkSumm(dbgModuleParameters.Checksum)
        {
        }

        bool operator < (const ModuleId &rhs) const
        {
            return makeQword() < rhs.makeQword();
        }

    private:
        LONG64 makeQword() const {
            return 
                static_cast<LONG64>(m_timeDataStamp) | 
                (static_cast<LONG64>(m_checkSumm) << 32);
        }
    };

    // symbol data
    struct SymbolData {
        std::string m_name;
        ULONG m_size;
    };

    // synthetic symbols of one module: offset -> name+size
    typedef ULONG64 SYM_OFFSET;
    typedef std::map<SYM_OFFSET, SymbolData> ModSymbols;

    // synthetic symbols of all modules
    typedef std::map<ModuleId, ModSymbols> AllSymbols;

private:

    // lock of static map
    typedef boost::shared_ptr<boost::recursive_mutex> SymbolsLock;
    typedef boost::recursive_mutex::scoped_lock SymbolsScopedLock;

private:

    // execute for all symbols from modules
    void forEachFromModule(
        ULONG64 moduleBase,
        const ModSymbols &modSymbols,
        ISynSymForEach &forEach
    );

    // execute for all symbols all loaded modules
    void forEachLoadedModule(ISynSymForEach &forEach);

private:

    SymbolsLock m_allSymbolsLock;
    AllSymbols m_allSymbols;

    DebugClient &m_dbgClient;
    IDebugSymbols3 &m_symbols;
};

////////////////////////////////////////////////////////////////////////////////

typedef boost::shared_ptr<SyntheticSymbols> SynSymbolsPtr;

////////////////////////////////////////////////////////////////////////////////

};  // namespace pykd
