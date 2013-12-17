#include "stdafx.h"

#include "kdlib/dbgengine.h"
#include "kdlib/typeinfo.h"

#include "pydbgeng.h"
#include "variant.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

python::object evaluate( const std::wstring  &expression,  bool cplusplus )
{
    kdlib::NumVariant var;

    do {
        AutoRestorePyState  pystate;
        var = kdlib::evaluate(expression, cplusplus );
    } while(false);

    return pykd::NumVariantAdaptor::convertToPython( var );
}

///////////////////////////////////////////////////////////////////////////////

python::tuple getSourceLine( kdlib::MEMOFFSET_64 offset )
{
    std::wstring  fileName;
    unsigned long  lineno;
    long  displacement;

    do {
        AutoRestorePyState  pystate;
        kdlib::getSourceLine( fileName, lineno, displacement, offset );
    } while(false);

    return python::make_tuple( fileName, lineno, displacement );
}

///////////////////////////////////////////////////////////////////////////////

kdlib::SystemInfo getSystemVersion()
{
    AutoRestorePyState  pystate;

    kdlib::SystemInfo sysInfo;
    kdlib::getSystemInfo( sysInfo );
    return sysInfo;
}

///////////////////////////////////////////////////////////////////////////////

std::wstring printSystemVersion( kdlib::SystemInfo& sysInfo )
{
    std::wstringstream sstr;

    sstr << L"Major Version: " << sysInfo.majorVersion << std::endl;
    sstr << L"Minor Version: " << sysInfo.minorVersion << std::endl;;
    sstr << L"Description: " << sysInfo.buildDescription << std::endl;

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////

python::list getExceptionInfoParameters( kdlib::ExceptionInfo& exceptionInfo )
{
    python::list lst;
    for ( unsigned long i = 0; i < exceptionInfo.parameterCount; ++i )
        lst.append( exceptionInfo.parameters[i] );
    return lst;
}

///////////////////////////////////////////////////////////////////////////////

std::wstring printExceptionInfo( kdlib::ExceptionInfo& exceptionInfo )
{
    std::wstringstream sstream;

    sstream << L"FirstChance= " << (exceptionInfo.firstChance ? "True" : "False") << std::endl;

    sstream << L"ExceptionCode= 0x" << std::hex << exceptionInfo.exceptionCode << std::endl;
    sstream << L"ExceptionFlags= 0x" << std::hex <<  exceptionInfo.exceptionFlags << std::endl;
    sstream << L"ExceptionRecord= 0x" << std::hex <<  exceptionInfo.exceptionRecord << std::endl;
    sstream << L"ExceptionAddress= 0x" << std::hex <<  exceptionInfo.exceptionAddress << std::endl;

    for (ULONG i = 0; i < exceptionInfo.parameterCount; ++i)
    {
        sstream << L"Param["  << std::dec << i << L"]= 0x";
        sstream << std::hex <<  exceptionInfo.parameters[i] << std::endl;
    }

    return sstream.str();
}

///////////////////////////////////////////////////////////////////////////////

} //end namespace pykd
