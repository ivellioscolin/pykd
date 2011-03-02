
#pragma once

/////////////////////////////////////////////////////////////////////////////////
// Global functions

bool addSyntheticSymbol(
    ULONG64 addr,
    ULONG size,
    const std::string &symName
);

void delAllSyntheticSymbols();

ULONG delSyntheticSymbol(
    ULONG64 addr
);

ULONG delSyntheticSymbolsMask(
    const std::string &moduleName,
    const std::string &symName
);

/////////////////////////////////////////////////////////////////////////////////
// Functions for dbgModuleClass

ULONG64 getSyntheticSymbol(
    const ModuleInfo &moduleInfo,
    const std::string  &symName
);

bool addSyntheticSymbolForModule(
    ULONG64 offset,
    ULONG size,
    const std::string &symName,
    const ModuleInfo &moduleInfo
);

ULONG delSyntheticSymbolForModule(
    ULONG64 offset,
    const ModuleInfo &moduleInfo
);

void delAllSyntheticSymbolsForModule(
    const ModuleInfo &moduleInfo
);

/////////////////////////////////////////////////////////////////////////////////
// External callbacks

void restoreSyntheticSymbolForModule(
    const ModuleInfo &moduleInfo,
    IDebugSymbols3 *symbols3
);

void restoreSyntheticSymbolForAllModules(
    IDebugSymbols *symbols,
    IDebugSymbols3 *symbols3
);

/////////////////////////////////////////////////////////////////////////////////
