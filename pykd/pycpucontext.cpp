
#include "stdafx.h"

#include "pycpucontext.h"
#include "variant.h"


namespace pykd {

///////////////////////////////////////////////////////////////////////////////

python::object CPUContextAdapter::getRegisterByName( kdlib::CPUContextPtr& cpu, const std::wstring &name )
{
    kdlib::NumVariant var;

    do {
        AutoRestorePyState  pystate;
        var = cpu->getRegisterByName(name);
    } while(false);

    return NumVariantAdaptor::convertToPython( var );
}

///////////////////////////////////////////////////////////////////////////////

python::object CPUContextAdapter::getRegisterByIndex( kdlib::CPUContextPtr& cpu, unsigned long  index )
{
    kdlib::NumVariant var;
    std::wstring name;

    do {
        AutoRestorePyState  pystate;
        var = cpu->getRegisterByIndex(index);
        name = cpu->getRegisterName(index);
    }while(false);

    return python::make_tuple( name, NumVariantAdaptor::convertToPython( var ) );
}

///////////////////////////////////////////////////////////////////////////////

python::list  CPUContextAdapter::getStack( kdlib::CPUContextPtr& cpu )
{
    kdlib::StackPtr  stack;
    unsigned long  numberFrames;

    do {
        AutoRestorePyState  pystate;
        stack = kdlib::getStack(cpu);
        numberFrames = stack->getFrameCount();
    } while(false);

    python::list  lst;
    for ( unsigned long  i = 0; i < numberFrames; ++i )
        lst.append( stack->getFrame(i) );

    return lst;
}

///////////////////////////////////////////////////////////////////////////////

std::wstring StackFrameAdapter::print( kdlib::StackFramePtr& frame )
{
    AutoRestorePyState  pystate;

    std::wstringstream sstr;
    sstr << L"Frame: ";
    sstr << L"IP=" << std::hex << frame->getIP() << L"  ";
    sstr << L"Return=" << std::hex << frame->getRET() << L"  ";
    sstr << L"Frame Offset=" << std::hex << frame->getFP() << L"  ";
    sstr << L"Stack Offset=" << std::hex << frame->getSP();

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////

python::list StackFrameAdapter::getParamsList( kdlib::StackFramePtr&  frame)
{
    typedef std::vector< std::pair< std::wstring, kdlib::TypedVarPtr> >  FuncParamList;

    FuncParamList  paramLst;
    unsigned long  paramCount;

    do {
        AutoRestorePyState  pystate;
        paramCount = frame->getTypedParamCount();
        for ( unsigned long i = 0; i < paramCount; ++i )
        {
            kdlib::TypedVarPtr  param = frame->getTypedParam(i);
            std::wstring  paramName =   frame->getTypedParamName(i);

            paramLst.push_back( std::make_pair( paramName, param) );
        }
    } while(false);

    python::list  pyLst;

    for ( unsigned long i = 0; i < paramCount; ++i )
        pyLst.append( python::make_tuple( paramLst[i].first, paramLst[i].second ) );

    return pyLst;
}

///////////////////////////////////////////////////////////////////////////////

python::dict StackFrameAdapter::getParamsDict( kdlib::StackFramePtr&  frame)
{
    typedef std::vector< std::pair< std::wstring, kdlib::TypedVarPtr> >  FuncParamList;

    FuncParamList  paramLst;
    unsigned long  paramCount;

    do {
        AutoRestorePyState  pystate;
        paramCount = frame->getTypedParamCount();
        for ( unsigned long i = 0; i < paramCount; ++i )
        {
            kdlib::TypedVarPtr  param = frame->getTypedParam(i);
            std::wstring  paramName =   frame->getTypedParamName(i);

            paramLst.push_back( std::make_pair( paramName, param) );
        }
    } while(false);

    python::dict  pyLst;

    for ( unsigned long i = 0; i < paramCount; ++i )
        pyLst[paramLst[i].first] = paramLst[i].second;

    return pyLst;
}

///////////////////////////////////////////////////////////////////////////////

} // end namespace pykd
