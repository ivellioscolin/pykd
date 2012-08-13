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

python::dict StackFrame::getLocals()
{
    return getLocalsByFrame( *this );
}

////////////////////////////////////////////////////////////////////////////////

python::list getCurrentStack()
{
    ULONG  frameCount = getStackTraceFrameCount();

    std::vector<STACK_FRAME_DESC>  frames(frameCount); 

    getStackTrace( &frames[0], frameCount );

    python::list    frameList;

    for ( ULONG i = 0; i < frameCount; ++i )
    {
        python::object  frameObj( StackFrame( frames.at(i) ) ); 

        frameList.append( frameObj );
    }

    return frameList; 
}

////////////////////////////////////////////////////////////////////////////////

StackFrame getCurrentStackFrame()
{
    ULONG  frameCount = getStackTraceFrameCount();

    std::vector<STACK_FRAME_DESC>  frames(frameCount); 

    getStackTrace( &frames[0], frameCount );

    return frames[0];
}

///////////////////////////////////////////////////////////////////////////////

} // namespace pykd



