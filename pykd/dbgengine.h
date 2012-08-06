#pragma once

#include "dbgmem.h"

namespace pykd {

// manage debug target
ULONG startProcess( const std::wstring  &processName );
void detachProcess( ULONG processId = -1);
void terminateProcess( ULONG processId = -1);

void debugGo();

// system properties
ULONG ptrSize();

//manage debug module
ULONG64 findModuleBase( const std::string &moduleName );
ULONG64 findModuleBase( ULONG64 offset );
ULONG64 findModuleBySymbol( const std::string &symbolName );
std::string getModuleName( ULONG64 baseOffset );
std::string getModuleImageName( ULONG64 baseOffset ); 
std::string getModuleSymbolFileName( ULONG64 baseOffset );
ULONG getModuleTimeStamp( ULONG64 baseOffset );
ULONG getModuleCheckSum( ULONG64 baseOffset );

// это нужно сделать по-другому!
std::string getSymbolByOffset( ULONG64 offset );

};

