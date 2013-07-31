#pragma once

#include <boost/python/tuple.hpp>
namespace python = boost::python;

#include "kdlib/dbgengine.h"
#include "kdlib/cpucontext.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

class CPUContextAdaptor
{
public:
    static python::object getRegisterByName( kdlib::CPUContext& cpu, const std::wstring &name );
    static python::object getRegisterByIndex( kdlib::CPUContext& cpu, unsigned long index );
    static python::list getStack( kdlib::CPUContext& cpu );
};

struct StackFrame {
    kdlib::MEMOFFSET_64 ip, ret, fp, sp;
};

std::wstring printStackFrame( StackFrame& frame );

inline python::object getRegisterByName( const std::wstring &name )
{
    return CPUContextAdaptor::getRegisterByName( *kdlib::loadCPUCurrentContext().get(), name );
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

inline python::list getCurrentStack() {
    return CPUContextAdaptor::getStack( *kdlib::loadCPUCurrentContext() );
}


///////////////////////////////////////////////////////////////////////////////

} //end namespace pykd
