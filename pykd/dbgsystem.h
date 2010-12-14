#pragma once

#include <string>

/////////////////////////////////////////////////////////////////////////////////

bool
is64bitSystem();

int
ptrSize(); 

std::string
dbgSymPath();

std::string
getPdbFile( ULONG64  moduleBase );

std::string
getImageFile( ULONG64  moduleBase );

void
reloadModule( const char * moduleName  );

bool
isKernelDebugging();

/////////////////////////////////////////////////////////////////////////////////