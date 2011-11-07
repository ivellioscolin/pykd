#pragma once

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

ULONG64
addr64( ULONG64  addr );

///////////////////////////////////////////////////////////////////////////////////

void
readMemory( ULONG64 address, PVOID buffer, ULONG length, BOOLEAN phyAddr = FALSE );

void
writeMemory( ULONG64 address, PVOID buffer, ULONG length, BOOLEAN phyAddr = FALSE );

///////////////////////////////////////////////////////////////////////////////////

std::string loadChars( ULONG64 address, ULONG  number, bool phyAddr = FALSE );

std::wstring loadWChars( ULONG64 address, ULONG  number, bool phyAddr = FALSE );

python::list loadBytes( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

python::list loadWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

python::list loadDWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

python::list loadQWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

python::list loadSignBytes( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

python::list loadSignWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

python::list loadSignDWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

python::list loadSignQWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

///////////////////////////////////////////////////////////////////////////////////

};

