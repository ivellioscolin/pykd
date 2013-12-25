#pragma once

#include <boost/python/tuple.hpp>
namespace python = boost::python;

#include "kdlib/dbgengine.h"
#include "kdlib/cpucontext.h"
#include "kdlib/stack.h"

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
};

///////////////////////////////////////////////////////////////////////////////


inline kdlib::CPUContextPtr loadCPUCurrentContext() {
    AutoRestorePyState  pystate;
    return kdlib::loadCPUCurrentContext();
}

inline kdlib::CPUContextPtr loadCPUContextByIndex( unsigned long index ) {
    AutoRestorePyState  pystate;
    return kdlib::loadCPUContextByIndex(index);
}

inline unsigned long long loadMSR( unsigned long  msrIndex ) 
{
    AutoRestorePyState  pystate;
    return kdlib::loadCPUCurrentContext()->loadMSR( msrIndex );
}

inline void setMSR( unsigned long  msrIndex, unsigned long long value )
{  
    AutoRestorePyState  pystate;
    return kdlib::loadCPUCurrentContext()->setMSR( msrIndex, value );
}

inline kdlib::CPUType getProcessorMode() {
    AutoRestorePyState  pystate;
    return kdlib::loadCPUCurrentContext()->getCPUMode();
}

inline kdlib::CPUType getProcessorType() {
    AutoRestorePyState  pystate;
    return kdlib::loadCPUCurrentContext()->getCPUType();
}

inline void setProcessorMode( kdlib::CPUType mode ) {
    AutoRestorePyState  pystate;
    kdlib::loadCPUCurrentContext()->setCPUMode(mode);
}

inline void switchProcessorMode() {
    AutoRestorePyState  pystate;
    kdlib::loadCPUCurrentContext()->switchCPUMode();
}

inline kdlib::StackFramePtr getCurrentFrame() {
    AutoRestorePyState  pystate;
    return kdlib::getStack()->getFrame(0);
}

inline python::list getParams() {
    return StackFrameAdapter::getParamsList( getCurrentFrame() );
}

inline kdlib::TypedVarPtr getParam( const std::wstring &name ) {
    return StackFrameAdapter::getParam( getCurrentFrame(), name );
}

class CPUContextAdapter
{
public:
    static python::object getRegisterByName( kdlib::CPUContextPtr& cpu, const std::wstring &name );
    static python::object getRegisterByIndex( kdlib::CPUContextPtr& cpu, unsigned long index );
    static python::list getStack( kdlib::CPUContextPtr& cpu );
    
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

    static void setCPUMode( kdlib::CPUContextPtr& cpu, kdlib::CPUType mode )
    {
        AutoRestorePyState  pystate;
        cpu->setCPUMode(mode);
    }

    static void switchCPUMode( kdlib::CPUContextPtr& cpu )
    {
        AutoRestorePyState  pystate;
        cpu->switchCPUMode();
    }
};

inline python::object getRegisterByName( const std::wstring &name )
{
    return CPUContextAdapter::getRegisterByName( kdlib::loadCPUCurrentContext(), name );
}

inline python::list getCurrentStack() {
    return CPUContextAdapter::getStack( kdlib::loadCPUCurrentContext() );
}

///////////////////////////////////////////////////////////////////////////////

} //end namespace pykd
