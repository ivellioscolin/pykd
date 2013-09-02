#include "stdafx.h"

#include "kdlib/dbgengine.h"
#include "kdlib/typeinfo.h"

#include "dbgengine.h"

namespace pykd {

class AutoRestorePyState 
{
public:

    AutoRestorePyState() 
    {
        m_state =  PyEval_SaveThread();
    }

    ~AutoRestorePyState() 
    {
        PyEval_RestoreThread( m_state );
    }

private:

    PyThreadState*    m_state;
};

///////////////////////////////////////////////////////////////////////////////

kdlib::ExecutionStatus targetGo()
{
    kdlib::ExecutionStatus  status;

    AutoRestorePyState  pystate;

    status = kdlib::targetGo();

    return status;
}

///////////////////////////////////////////////////////////////////////////////

void targetBreak()
{
    AutoRestorePyState  pystate;

    kdlib::targetBreak();
}

///////////////////////////////////////////////////////////////////////////////

kdlib::ExecutionStatus targetStep()
{
    kdlib::ExecutionStatus  status;

    AutoRestorePyState  pystate;

    status =  kdlib::targetStep();

    return status;
}

///////////////////////////////////////////////////////////////////////////////

kdlib::ExecutionStatus targetStepIn()
{
    kdlib::ExecutionStatus  status;

    AutoRestorePyState  pystate;

    status = kdlib::targetStepIn();

    return status;
}

///////////////////////////////////////////////////////////////////////////////

kdlib::PROCESS_DEBUG_ID startProcess( const std::wstring  &processName )
{
    kdlib::PROCESS_DEBUG_ID  id;

    AutoRestorePyState  pystate;

    id = kdlib::startProcess(processName);

    return id;
}

///////////////////////////////////////////////////////////////////////////////

kdlib::PROCESS_DEBUG_ID attachProcess( kdlib::PROCESS_ID pid )
{
    kdlib::PROCESS_DEBUG_ID  id;
    
    AutoRestorePyState  pystate;

    id = kdlib::attachProcess(pid);

    return id;
}

///////////////////////////////////////////////////////////////////////////////

void loadDump( const std::wstring &fileName )
{
    AutoRestorePyState  pystate;

    kdlib::loadDump(fileName);
}

///////////////////////////////////////////////////////////////////////////////

void attachKernel( const std::wstring &connectOptions )
{
    AutoRestorePyState  pystate;

    kdlib::attachKernel(connectOptions);
}

///////////////////////////////////////////////////////////////////////////////

std::wstring debugCommand( const std::wstring &command )
{
    AutoRestorePyState  pystate;

    std::wstring outstr = kdlib::debugCommand(command);

    return outstr;
}

///////////////////////////////////////////////////////////////////////////////

unsigned long long evaluate( const std::wstring  &expression )
{
    AutoRestorePyState  pystate;

    unsigned long long result = kdlib::evaluate(expression);

    return result;
}

///////////////////////////////////////////////////////////////////////////////

python::tuple getSourceLine( kdlib::MEMOFFSET_64 offset )
{
    std::wstring  fileName;
    unsigned long  lineno;
    long  displacement;

    kdlib::getSourceLine( fileName, lineno, displacement, offset );

    return python::make_tuple( fileName, lineno, displacement );
}

///////////////////////////////////////////////////////////////////////////////

python::tuple findSymbolAndDisp( ULONG64 offset )
{
    kdlib::MEMDISPLACEMENT  displacement = 0;
    std::wstring  symbolName = kdlib::findSymbol( offset, displacement );
    return python::make_tuple(symbolName,displacement);
}

///////////////////////////////////////////////////////////////////////////////

kdlib::SystemInfo getSystemVersion()
{
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
