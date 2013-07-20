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
    static python::object getRegisterByIndex( kdlib::CPUContext& cpu, size_t index );
};

inline python::object getRegisterByName( const std::wstring &name )
{
    return CPUContextAdaptor::getRegisterByName( *kdlib::loadCPUCurrentContext().get(), name );
}

inline unsigned long long loadMSR( size_t msrIndex ) 
{
    return kdlib::loadCPUCurrentContext()->loadMSR( msrIndex );
}

inline void setMSR( size_t msrIndex, unsigned long long value )
{  
    return kdlib::loadCPUCurrentContext()->setMSR( msrIndex, value );
}


///////////////////////////////////////////////////////////////////////////////

} //end namespace pykd
