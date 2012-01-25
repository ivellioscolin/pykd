// 
// Stack frame: DEBUG_STACK_FRAME wrapper
// 

#include "stdafx.h"
#include "stkframe.h"

////////////////////////////////////////////////////////////////////////////////

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

StackFrame::StackFrame(DebugClientPtr client, const DEBUG_STACK_FRAME &frame)
    : m_client(client)
    , m_frameNumber(frame.FrameNumber)
    , m_instructionOffset(frame.InstructionOffset)
    , m_returnOffset(frame.ReturnOffset)
    , m_frameOffset(frame.FrameOffset)
    , m_stackOffset(frame.StackOffset)
{
}

////////////////////////////////////////////////////////////////////////////////

std::string StackFrame::print() const
{
    std::stringstream sstream;

    sstream << std::dec << "(" << m_frameNumber << ")";

    sstream << " ip= 0x" << std::hex << m_instructionOffset;
    sstream << ", ret= 0x" << std::hex << m_returnOffset;
    sstream << ", frame= 0x" << std::hex << m_frameOffset;
    sstream << ", stack= 0x" << std::hex << m_stackOffset;

    return sstream.str();

}

////////////////////////////////////////////////////////////////////////////////

python::dict StackFrame::getLocals(ContextPtr ctx /*= ContextPtr()*/)
{
    ctx = ctx   ? ctx->forkByStackFrame(*this)
                : m_client->getThreadContext()->forkByStackFrame(*this);
    return m_client->getLocals(ctx);
}

////////////////////////////////////////////////////////////////////////////////

} // namespace pykd

////////////////////////////////////////////////////////////////////////////////

