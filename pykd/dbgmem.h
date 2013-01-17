#pragma once

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

ULONG64 addr64( ULONG64 offset );
ULONG64 addr64NoSafe( ULONG64 addr );
void readMemory( ULONG64 offset, PVOID buffer, ULONG length, bool phyAddr = FALSE, ULONG *readed = NULL );
bool readMemoryUnsafe( ULONG64 offset, PVOID buffer, ULONG length, bool phyAddr = FALSE, ULONG *readed = NULL );
bool readMemoryUnsafeNoSafe( ULONG64 offset, PVOID buffer, ULONG length, bool phyAddr = FALSE, ULONG *readed = NULL );
bool isVaValid( ULONG64 addr );
bool compareMemory( ULONG64 addr1, ULONG64 addr2, ULONG length, bool phyAddr = FALSE );
ULONG getVaProtect( ULONG64 offset );

void findMemoryRegion( ULONG64 beginOffset, ULONG64 *startOffset, ULONG64* length );
python::tuple findMemoryRegionPy( ULONG64 beginOffset );

ULONG64 ptrByte( ULONG64 offset );
ULONG64 ptrWord( ULONG64 offset );
ULONG64 ptrDWord( ULONG64 offset );
ULONG64 ptrQWord( ULONG64 offset );
ULONG64 ptrMWord( ULONG64 offset );
LONG64 ptrSignByte( ULONG64 offset );
LONG64 ptrSignWord( ULONG64 offset );
LONG64 ptrSignDWord( ULONG64 offset );
LONG64 ptrSignQWord( ULONG64 offset );
LONG64 ptrSignMWord( ULONG64 offset );
ULONG64 ptrPtr( ULONG64 offset );

double ptrSingleFloat( ULONG64 offset );
double ptrDoubleFloat( ULONG64 offset );

python::list loadBytes( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
python::list loadWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
python::list loadDWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
python::list loadQWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
python::list loadSignBytes( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
python::list loadSignWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
python::list loadSignDWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
python::list loadSignQWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

python::list loadFloats(  ULONG64 offset, ULONG count, bool phyAddr = FALSE );
python::list loadDoubles(  ULONG64 offset, ULONG count, bool phyAddr = FALSE );

python::list loadPtrList( ULONG64 offset );
python::list loadPtrArray( ULONG64 offset, ULONG  number );

std::string loadChars( ULONG64 offset, ULONG  number, bool phyAddr = FALSE );
std::wstring loadWChars( ULONG64 offset, ULONG  number, bool phyAddr = FALSE );

std::string loadCStr( ULONG64 offset );
std::wstring loadWStr( ULONG64 offset );

std::string loadAnsiStr( ULONG64 offset );
std::wstring loadUnicodeStr( ULONG64 offset );

///////////////////////////////////////////////////////////////////////////////////

}; // end pykd namespace

