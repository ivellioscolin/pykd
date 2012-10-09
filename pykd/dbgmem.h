#pragma once

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

ULONG64 addr64( ULONG64 offset );
HRESULT readMemoryImpl(ULONG64 offset, PVOID buffer, ULONG length, ULONG *readed = NULL, bool phyAddr = false);
void readMemory( ULONG64 offset, PVOID buffer, ULONG length, bool phyAddr = FALSE );
bool isVaValid( ULONG64 addr );
bool compareMemory( ULONG64 addr1, ULONG64 addr2, ULONG length, bool phyAddr = FALSE );

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

python::list loadBytes( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
python::list loadWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
python::list loadDWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
python::list loadQWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
python::list loadSignBytes( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
python::list loadSignWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
python::list loadSignDWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
python::list loadSignQWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

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

































//namespace pykd {
//
/////////////////////////////////////////////////////////////////////////////////////
//
//ULONG64
//addr64( ULONG64  addr );
//
//bool
//isVaValid( ULONG64 addr );
//
/////////////////////////////////////////////////////////////////////////////////////
//
//HRESULT readVirtual(
//    IDebugDataSpaces4 *dbgDataSpace,
//    ULONG64 address,
//    PVOID buffer,
//    ULONG length,
//    PULONG readed
//);
//
/////////////////////////////////////////////////////////////////////////////////////
//
//void
//readMemory( IDebugDataSpaces4*  dbgDataSpace, ULONG64 address, PVOID buffer, ULONG length, bool phyAddr = FALSE );
//
//void
//readMemoryPtr( IDebugDataSpaces4*  dbgDataSpace, ULONG64 address, PULONG64 ptrValue );
//
//void
//writeMemory( IDebugDataSpaces4*  dbgDataSpace, ULONG64 address, PVOID buffer, ULONG length, bool phyAddr = FALSE );
//
//bool 
//compareMemoryRange( IDebugDataSpaces4*  dbgDataSpace, ULONG64 addr1, ULONG64 addr2, ULONG length, bool phyAddr = FALSE );
//
/////////////////////////////////////////////////////////////////////////////////////
//
//std::string loadChars( ULONG64 address, ULONG  number, bool phyAddr = FALSE );
//
//std::wstring loadWChars( ULONG64 address, ULONG  number, bool phyAddr = FALSE );
//
//std::string loadCStr( ULONG64 offset );
//
//std::wstring loadWStr( ULONG64 offset );
//
//python::list loadBytes( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
//
//python::list loadWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
//
//python::list loadDWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
//
//python::list loadQWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
//
//python::list loadSignBytes( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
//
//python::list loadSignWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
//
//python::list loadSignDWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
//
//python::list loadSignQWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );
//
//ULONG64 ptrByte( ULONG64 offset );
//
//ULONG64 ptrWord( ULONG64 offset );
//
//ULONG64 ptrDWord( ULONG64 offset );
//
//ULONG64 ptrDWord( ULONG64 offset, IDebugDataSpaces4*  dbgDataSpace );
//
//ULONG64 ptrQWord( ULONG64 offset );
//
//ULONG64 ptrQWord( ULONG64 offset, IDebugDataSpaces4*  dbgDataSpace );
//
//ULONG64 ptrMWord( ULONG64 offset );
//
//ULONG64 ptrMWord( ULONG64 offset, IDebugDataSpaces4*  dbgDataSpace  );
//
//LONG64 ptrSignByte( ULONG64 offset );
//
//LONG64 ptrSignWord( ULONG64 offset );
//
//LONG64 ptrSignDWord( ULONG64 offset );
//
//LONG64 ptrSignQWord( ULONG64 offset );
//
//LONG64 ptrSignMWord( ULONG64 offset );
//
//ULONG64 ptrPtr( ULONG64 offset );
//
//ULONG64 ptrPtr( ULONG64 offset, IDebugDataSpaces4*  dbgDataSpace );
//
//bool 
//compareMemory( ULONG64 addr1, ULONG64 addr2, ULONG length, bool phyAddr = FALSE );
//
//std::wstring loadUnicodeStr( ULONG64 address );
//
//std::string loadAnsiStr( ULONG64 address );
//
//python::list loadPtrList( ULONG64 address );
//
//python::list loadPtrArray( ULONG64 address, ULONG  number );
//
/////////////////////////////////////////////////////////////////////////////////////
//
//};

