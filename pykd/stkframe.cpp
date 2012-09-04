// 
// Stack frame: DEBUG_STACK_FRAME wrapper
// 

#include "stdafx.h"
#include "stkframe.h"
#include "dbgengine.h"
#include "localvar.h"

////////////////////////////////////////////////////////////////////////////////

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

StackFrame::StackFrame( const STACK_FRAME_DESC& desc )
{
    m_frameNumber = desc.number;
    m_instructionOffset = desc.instructionOffset;
    m_returnOffset = desc.returnOffset;
    m_frameOffset = desc.frameOffset;
    m_stackOffset = desc.stackOffset;
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

python::dict StackFrame::getLocals() const
{
    return getLocalsByFrame( *this );
}

///////////////////////////////////////////////////////////////////////////////

ULONG64 StackFrame::getValue(RegRealativeId rri, LONG64 offset /*= 0*/) const
{
    switch (rri)
    {
    case rriInstructionPointer: return m_instructionOffset + offset;
    case rriStackFrame: return m_frameOffset + offset;
    case rriStackPointer: return m_stackOffset + offset;
    }

    BOOST_ASSERT(!"Unexcepted error");
    throw DbgException(__FUNCTION__ " Unexcepted error" );
}

////////////////////////////////////////////////////////////////////////////////

python::list getCurrentStack()
{
    std::vector<STACK_FRAME_DESC> frames; 
    getStackTrace( frames );

    python::list frameList;

    for ( ULONG i = 0; i < frames.size(); ++i )
    {
        python::object  frameObj( StackFrame( frames.at(i) ) ); 
        frameList.append( frameObj );
    }

    return frameList; 
}

////////////////////////////////////////////////////////////////////////////////

StackFrame getCurrentStackFrame()
{
    std::vector<STACK_FRAME_DESC> frames; 
    getStackTrace( frames );
    return frames[0];
}

///////////////////////////////////////////////////////////////////////////////

} // namespace pykd



