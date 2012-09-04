// 
// Stack frame: DEBUG_STACK_FRAME wrapper
// 

#pragma once

#include "dbgengine.h"
#include "context.h"
#include "symengine.h"

////////////////////////////////////////////////////////////////////////////////

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

class StackFrame
{
public:
    StackFrame( const STACK_FRAME_DESC& desc );

    python::dict getLocals() const;

    std::string print() const;

    ULONG64 getValue(RegRealativeId rri, LONG64 offset = 0) const;

public:

    ULONG  m_frameNumber;
    ULONG64  m_instructionOffset;
    ULONG64  m_returnOffset;
    ULONG64  m_frameOffset;
    ULONG64  m_stackOffset;

};

///////////////////////////////////////////////////////////////////////////////

StackFrame getCurrentStackFrame();

python::list getCurrentStack();

///////////////////////////////////////////////////////////////////////////////

} // namespace pykd


