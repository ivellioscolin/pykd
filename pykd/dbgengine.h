#pragma once

#include "dbgmem.h"
#include "variant.h"

namespace pykd {

// manage debug target
ULONG startProcess( const std::wstring  &processName );
void detachProcess( ULONG processId = -1);
void terminateProcess( ULONG processId = -1);

void loadDump( const std::wstring &fileName );

bool isDumpAnalyzing();
bool isKernelDebugging();

void debugGo();

// debug output
void dprint( const std::wstring &str, bool dml = false );
void dprintln( const std::wstring &str, bool dml = false );
std::string dreadline();
void eprint( const std::wstring &str );
void eprintln( const std::wstring &str );

// system properties
ULONG ptrSize();
bool is64bitSystem();

//manage debug module
ULONG64 findModuleBase( const std::string &moduleName );
ULONG64 findModuleBase( ULONG64 offset );
ULONG64 findModuleBySymbol( const std::string &symbolName );
std::string getModuleName( ULONG64 baseOffset );
std::string getModuleImageName( ULONG64 baseOffset ); 
ULONG getModuleSize( ULONG64 baseOffset );
std::string getModuleSymbolFileName( ULONG64 baseOffset );
ULONG getModuleTimeStamp( ULONG64 baseOffset );
ULONG getModuleCheckSum( ULONG64 baseOffset );

// CPU registers
ULONG getRegIndexByName( const std::string &regName );
std::string getRegNameByIndex( ULONG index );
BaseTypeVariant getRegVariantValue( ULONG index );
ULONG64 getRegInstructionPointer();

// Stack and local variables
struct STACK_FRAME_DESC {
    ULONG number;
    ULONG64 instructionOffset;
    ULONG64 returnOffset;
    ULONG64 frameOffset;
    ULONG64 stackOffset;
};

ULONG getStackTraceFrameCount();
void getStackTrace( STACK_FRAME_DESC* frames, ULONG frameCount, ULONG* frameReturned = NULL );

};

