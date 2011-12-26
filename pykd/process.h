#pragma once

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

ULONG64 getCurrentProcess();

ULONG64 getImplicitThread();

python::list getCurrentStack();

std::string getProcessorMode();

std::string getProcessorType();

python::list getThreadList();

void setCurrentProcess( ULONG64 processAddr );

void setImplicitThread( ULONG64 threadAddr );

void setProcessorMode( const std::wstring &mode );

///////////////////////////////////////////////////////////////////////////////////

}
