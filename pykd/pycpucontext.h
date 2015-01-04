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
        return frame->getLocalVar(paramName);
    }
};

/////////////////////////////////////////////////////////////////////////////////

python::object getRegisterByName( const std::wstring &name );

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

python::list getCurrentStack();

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


class CPUContextAdapter
{
public:

    static  kdlib::CPUContextPtr getCPUContext() {
        AutoRestorePyState  pystate;
        return kdlib::loadCPUContext();
    }

    static python::object getRegisterByName( kdlib::CPUContextPtr& cpu, const std::wstring &name );

    static python::tuple getRegisterByIndex( kdlib::CPUContextPtr& cpu, unsigned long index );

    static unsigned long getRegisterNumber( kdlib::CPUContextPtr& cpu) 
    {
        AutoRestorePyState  pystate;
        return cpu->getRegisterNumber();
    }

    static kdlib::MEMOFFSET_64 getIP( kdlib::CPUContextPtr& cpu ) 
    {
        AutoRestorePyState  pystate;
        return cpu->getIP();
    }
    
    static kdlib::MEMOFFSET_64 getSP( kdlib::CPUContextPtr& cpu )
    {
        AutoRestorePyState  pystate;
        return cpu->getSP();
    }

    static kdlib::MEMOFFSET_64 getFP( kdlib::CPUContextPtr& cpu )
    {
        AutoRestorePyState  pystate;
        return cpu->getFP();
    }

    static kdlib::CPUType getCPUType( kdlib::CPUContextPtr& cpu )
    {
        AutoRestorePyState  pystate;
        return cpu->getCPUType();
    }

    static kdlib::CPUType getCPUMode( kdlib::CPUContextPtr& cpu )
    {
        AutoRestorePyState  pystate;
        return cpu->getCPUMode();
    }

};

///////////////////////////////////////////////////////////////////////////////

} //end namespace pykd
