#pragma once

#include <string>

/////////////////////////////////////////////////////////////////////////////////

bool
is64bitSystem();

inline
int
ptrSize() {
    return is64bitSystem() ? 8 : 4;
}

std::string
dbgSymPath();

std::string
getPdbFile( ULONG64  moduleBase );

std::string
getImageFile( ULONG64  moduleBase );

void
reloadSymbols( const char * moduleName  );

/////////////////////////////////////////////////////////////////////////////////