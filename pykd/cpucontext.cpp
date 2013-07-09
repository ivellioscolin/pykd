
#include "stdafx.h"

#include "cpucontext.h"
#include "variant.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

python::object CPUContextAdaptor::getRegisterByName( kdlib::CPUContext& cpu, const std::wstring &name )
{
    kdlib::NumVariant var = cpu.getRegisterByName(name);
    return NumVariantAdaptor::convertToPython( var );
}

///////////////////////////////////////////////////////////////////////////////

python::object CPUContextAdaptor::getRegisterByIndex( kdlib::CPUContext& cpu, size_t index )
{
    kdlib::NumVariant var = cpu.getRegisterByIndex(index);
    std::wstring name = cpu.getRegisterName(index);

    return python::make_tuple( name, NumVariantAdaptor::convertToPython( var ) );
}

///////////////////////////////////////////////////////////////////////////////

} // end namespace pykd
