#pragma once

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

ULONG64 getCurrentProcess();

ULONG64 getImplicitThread();

python::list getCurrentStack();

std::string getProcessorMode();

std::string getProcessorType();

python::list getThreadList();

///////////////////////////////////////////////////////////////////////////////////

}
