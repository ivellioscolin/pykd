
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

python::list  CPUContextAdaptor::getStack( kdlib::CPUContext& cpu )
{
    size_t  numberFrames = cpu.getStackLength();
    python::list  lst;

    python::object  typeObj = python::object( python::handle<>(&PyType_Type) );
    python::object  frameType = typeObj("frameType", python::tuple(), python::dict() );

    for ( size_t i = 0; i < numberFrames; ++i )
    {
        StackFrame   frame;
        cpu.getStackFrame( i, frame.ip, frame.ret, frame.fp, frame.sp );
        lst.append( frame );
    }

    return lst;
}

///////////////////////////////////////////////////////////////////////////////

std::wstring printStackFrame( StackFrame& frame )
{
    std::wstringstream sstr;
    sstr << L"Frame: ";
    sstr << L"IP=" << std::hex << frame.ip << L"  ";
    sstr << L"Return=" << std::hex << frame.ret << L"  ";
    sstr << L"Frame Offset=" << std::hex << frame.fp << L"  ";
    sstr << L"Stack Offset=" << std::hex << frame.sp;

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////

} // end namespace pykd
