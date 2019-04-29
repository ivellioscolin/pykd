#pragma once

#include <boost/python/tuple.hpp>
namespace python = boost::python;

#include "kdlib/dbgengine.h"
#include "kdlib/cpucontext.h"
#include "kdlib/stack.h"

#include "dbgexcept.h"
#include "pythreadstate.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

class StackFrameAdapter {

public:

    static kdlib::MEMOFFSET_64 getIP( kdlib::StackFramePtr& frame ) 
    {
        AutoRestorePyState  pystate;
        return frame->getIP();
    }

    static kdlib::MEMOFFSET_64 getRET( kdlib::StackFramePtr& frame ) 
    {
       AutoRestorePyState  pystate;
        return frame->getRET();
    }

    static kdlib::MEMOFFSET_64  getFP( kdlib::StackFramePtr& frame ) 
    {
        AutoRestorePyState  pystate;
        return frame->getFP();
    }

    static kdlib::MEMOFFSET_64  getSP( kdlib::StackFramePtr& frame ) 
    {
        AutoRestorePyState  pystate;
        return frame->getSP();
    }

    static std::wstring print( kdlib::StackFramePtr& frame );

    static python::list getParamsList( kdlib::StackFramePtr& frame);

    static python::dict getParamsDict( kdlib::StackFramePtr& frame);

    static kdlib::TypedVarPtr getParam( kdlib::StackFramePtr& frame, const std::wstring &paramName ) 
    {
        AutoRestorePyState  pystate;
        return frame->getTypedParam(paramName);
    }

    static python::list getLocalsList(kdlib::StackFramePtr& frame);

    static python::dict getLocalsDict(kdlib::StackFramePtr& frame);

    static kdlib::TypedVarPtr getLocal( kdlib::StackFramePtr& frame, const std::wstring &paramName ) {
        AutoRestorePyState  pystate;
        if (frame->findStaticVar(paramName))
            return frame->getStaticVar(paramName);
        return frame->getLocalVar(paramName);
    }

    static void switchTo(kdlib::StackFramePtr& frame)
    {
        AutoRestorePyState  pystate;
        frame->switchTo();
    }

    static python::tuple findSymbol(kdlib::StackFramePtr& frame);

    static bool isInline(kdlib::StackFramePtr& frame)
    {
        AutoRestorePyState  pystate;
        return frame->isInline();
    }

    static python::tuple getSourceLine(kdlib::StackFramePtr& frame);
};

/////////////////////////////////////////////////////////////////////////////////

python::object getRegisterByName( const std::wstring &name );

python::object getRegisterByIndex( unsigned long index );

std::wstring getRegisterName(unsigned long index);

void setRegisterByName(const std::wstring& name, const python::object& value);

void setRegisterByIndex(unsigned long index, const python::object& value);

inline unsigned long getNumberRegisters() {
    AutoRestorePyState  pystate;
    return kdlib::getRegisterNumber();
}

inline kdlib::MEMOFFSET_64 getIP()
{
    AutoRestorePyState  pystate;
    return kdlib::getInstructionOffset();
}

inline kdlib::MEMOFFSET_64 getSP()
{
    AutoRestorePyState  pystate;
    return kdlib::getStackOffset(); 
}

inline kdlib::MEMOFFSET_64 getFP()
{
    AutoRestorePyState  pystate;
    return kdlib::getFrameOffset();
}

inline void setIP(kdlib::MEMOFFSET_64 offset)
{
    AutoRestorePyState  pystate;
    kdlib::setInstructionOffset(offset);
}

inline void setSP(kdlib::MEMOFFSET_64 offset)
{
    AutoRestorePyState  pystate;
    return kdlib::setStackOffset(offset); 
}

inline void setFP(kdlib::MEMOFFSET_64 offset)
{
    AutoRestorePyState  pystate;
    return kdlib::setFrameOffset(offset);
}
inline unsigned long long loadMSR( unsigned long  msrIndex ) 
{
    AutoRestorePyState  pystate;
    return kdlib::loadMSR(msrIndex);
}

inline void setMSR( unsigned long  msrIndex, unsigned long long value )
{  
    AutoRestorePyState  pystate;
    return kdlib::setMSR( msrIndex, value );
}

inline kdlib::CPUType getProcessorMode() {
    AutoRestorePyState  pystate;
    return kdlib::getCPUMode();
}

inline kdlib::CPUType getProcessorType() {
    AutoRestorePyState  pystate;
    return kdlib::getCPUType();
}

inline void setProcessorMode( kdlib::CPUType mode ) {
    AutoRestorePyState  pystate;
    kdlib::setCPUMode(mode);
}


inline void switchProcessorMode() {
    AutoRestorePyState  pystate;
    kdlib::switchCPUMode();
}

python::list getStack(bool inlineFrames = false);

inline kdlib::StackFramePtr getCurrentFrame() {
    AutoRestorePyState  pystate;
    return kdlib::getCurrentStackFrame();
}

inline unsigned long getCurrentFrameNumber() {
    AutoRestorePyState  pystate;
    return kdlib::getCurrentStackFrameNumber();
}

inline void setCurrentFrame( kdlib::StackFramePtr& stackFrame) {
    AutoRestorePyState  pystate;
    kdlib::setCurrentStackFrame(stackFrame);
}

inline void setCurrentFrameByIndex( unsigned long frameIndex) {
    AutoRestorePyState  pystate;
    kdlib::setCurrentStackFrameByIndex(frameIndex);
}

inline void resetCurrentFrame() {
    AutoRestorePyState  pystate;
    kdlib::resetCurrentStackFrame();
} 

inline python::list getParams() {
    return StackFrameAdapter::getParamsList( getCurrentFrame() );
}

inline kdlib::TypedVarPtr getParam( const std::wstring &name ) {
    return StackFrameAdapter::getParam( getCurrentFrame(), name );
}

inline kdlib::TypedVarPtr getLocal( const std::wstring &name ) {
    return StackFrameAdapter::getLocal( getCurrentFrame(), name );
}

inline python::list getLocals() {
    return StackFrameAdapter::getLocalsList( getCurrentFrame() );
}

inline kdlib::MEMOFFSET_64 stackAlloc(unsigned short byteCount) {
    AutoRestorePyState  pystate;
    return kdlib::stackAlloc(byteCount);
}

inline kdlib::MEMOFFSET_64 stackFree(unsigned short byteCount) {
    AutoRestorePyState  pystate;
    return kdlib::stackFree(byteCount);
}

inline void pushStack( const kdlib::NumVariant& value )
{
    AutoRestorePyState  pystate;
    return kdlib::pushInStack(value);
}

inline kdlib::NumVariant popStack() {
    AutoRestorePyState  pystate;
    kdlib::NumVariant  value;
    kdlib::popFromStack(value);
    return value;
}


class CPUContextAdapter
{
public:

    CPUContextAdapter()
    {}

    python::object getRegisterByName(const std::wstring &name)
    {
        return pykd::getRegisterByName(name);
    }

    python::tuple getRegisterByIndex(unsigned long index);

    unsigned long getRegisterNumber()
    {
        return pykd::getNumberRegisters();
    }

    kdlib::CPUType getCPUType() 
    {
        return  pykd::getProcessorType();
    }

    kdlib::CPUType getCPUMode()
    {
        return pykd::getProcessorMode();
    }

    kdlib::MEMOFFSET_64 getIP() 
    {
        return pykd::getIP();
    }
        
    kdlib::MEMOFFSET_64 getSP()
    {
        return pykd::getSP();
    }

    kdlib::MEMOFFSET_64 getFP()
    {
        return pykd::getFP();
    }
};

///////////////////////////////////////////////////////////////////////////////

} //end namespace pykd
