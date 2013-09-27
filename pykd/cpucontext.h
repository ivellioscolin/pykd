#pragma once

#include <boost/python/tuple.hpp>
namespace python = boost::python;

#include "kdlib/dbgengine.h"
#include "kdlib/cpucontext.h"
#include "kdlib/stack.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

class CPUContextAdaptor
{
public:
    static python::object getRegisterByName( kdlib::CPUContextPtr& cpu, const std::wstring &name );
    static python::object getRegisterByIndex( kdlib::CPUContextPtr& cpu, unsigned long index );
    static python::list getStack( kdlib::CPUContextPtr& cpu );
};

//struct StackFrame {
//    kdlib::MEMOFFSET_64 ip, ret, fp, sp;
//};

std::wstring printStackFrame( kdlib::StackFramePtr& frame );

inline python::object getRegisterByName( const std::wstring &name )
{
    return CPUContextAdaptor::getRegisterByName( kdlib::loadCPUCurrentContext(), name );
}

inline unsigned long long loadMSR( unsigned long  msrIndex ) 
{
    return kdlib::loadCPUCurrentContext()->loadMSR( msrIndex );
}

inline void setMSR( unsigned long  msrIndex, unsigned long long value )
{  
    return kdlib::loadCPUCurrentContext()->setMSR( msrIndex, value );
}

inline kdlib::CPUType getProcessorMode() {
    return kdlib::loadCPUCurrentContext()->getCPUMode();
}

inline kdlib::CPUType getProcessorType() {
    return kdlib::loadCPUCurrentContext()->getCPUType();
}

inline void setProcessorMode( kdlib::CPUType mode ) {
    kdlib::loadCPUCurrentContext()->setCPUMode(mode);
}

inline void switchProcessorMode() {
    kdlib::loadCPUCurrentContext()->switchCPUMode();
}

inline python::list getCurrentStack() {
    return CPUContextAdaptor::getStack( kdlib::loadCPUCurrentContext() );
}

inline kdlib::StackFramePtr getCurrentFrame() {
    return kdlib::getStack()->getFrame(0);
}

///////////////////////////////////////////////////////////////////////////////

} //end namespace pykd
