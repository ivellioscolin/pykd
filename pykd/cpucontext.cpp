
#include "stdafx.h"

#include "cpucontext.h"
#include "variant.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

python::object CPUContextAdaptor::getRegisterByName( kdlib::CPUContextPtr& cpu, const std::wstring &name )
{
    kdlib::NumVariant var = cpu->getRegisterByName(name);
    return NumVariantAdaptor::convertToPython( var );
}

///////////////////////////////////////////////////////////////////////////////

python::object CPUContextAdaptor::getRegisterByIndex( kdlib::CPUContextPtr& cpu, unsigned long  index )
{
    kdlib::NumVariant var = cpu->getRegisterByIndex(index);
    std::wstring name = cpu->getRegisterName(index);

    return python::make_tuple( name, NumVariantAdaptor::convertToPython( var ) );
}

///////////////////////////////////////////////////////////////////////////////

python::list  CPUContextAdaptor::getStack( kdlib::CPUContextPtr& cpu )
{
    kdlib::StackPtr  stack = kdlib::getStack(cpu);

    unsigned long  numberFrames = stack->getFrameCount();
     python::list  lst;

    for ( unsigned long  i = 0; i < numberFrames; ++i )
        lst.append( stack->getFrame(i) );

    return lst;
}

///////////////////////////////////////////////////////////////////////////////

std::wstring printStackFrame( kdlib::StackFramePtr& frame )
{
    std::wstringstream sstr;
    sstr << L"Frame: ";
    sstr << L"IP=" << std::hex << frame->getIP() << L"  ";
    sstr << L"Return=" << std::hex << frame->getRET() << L"  ";
    sstr << L"Frame Offset=" << std::hex << frame->getFP() << L"  ";
    sstr << L"Stack Offset=" << std::hex << frame->getSP();

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////

} // end namespace pykd
