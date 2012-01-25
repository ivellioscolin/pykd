// 
// Stack frame: DEBUG_STACK_FRAME wrapper
// 

#pragma once

#include "dbgclient.h"

////////////////////////////////////////////////////////////////////////////////

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

class StackFrame
{
public:
    StackFrame(DebugClientPtr client, const DEBUG_STACK_FRAME &frame);

    ULONG m_frameNumber;

    ULONG64 m_instructionOffset;
    ULONG64 m_returnOffset;
    ULONG64 m_frameOffset;
    ULONG64 m_stackOffset;

    python::dict getLocals(ContextPtr ctx = ContextPtr());

    std::string print() const;
private:
    DebugClientPtr m_client;
};

////////////////////////////////////////////////////////////////////////////////

} // namespace pykd

////////////////////////////////////////////////////////////////////////////////
