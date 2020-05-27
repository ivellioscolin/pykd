#pragma once

#include <boost/python/tuple.hpp>
namespace python = boost::python;

#include "kdlib/dbgengine.h"

#include "pythreadstate.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

inline
kdlib::PROCESS_DEBUG_ID startProcess(const std::wstring  &processName, const kdlib::ProcessDebugFlags& flags = kdlib::ProcessDebugDefault)
{
    AutoRestorePyState  pystate;
    return kdlib::startProcess(processName, flags);
}

inline
kdlib::PROCESS_DEBUG_ID attachProcess(kdlib::PROCESS_ID pid, const kdlib::ProcessDebugFlags& flags = kdlib::ProcessDebugDefault)
{
    AutoRestorePyState  pystate;
    return kdlib::attachProcess(pid, flags);
}

inline
void detachProcess( kdlib::PROCESS_DEBUG_ID processId = -1 ) 
{
    AutoRestorePyState  pystate;
    kdlib::detachProcess(processId);
}

inline
void detachAllProcesses()
{
    AutoRestorePyState  pystate;
    kdlib::detachAllProcesses();
}

inline
void terminateProcess( kdlib::PROCESS_DEBUG_ID processId = -1)
{
    AutoRestorePyState  pystate;
    kdlib::terminateProcess(processId);
}

inline
void terminateAllProcesses()
{
    AutoRestorePyState  pystate;
    kdlib::terminateAllProcesses();
}

inline
kdlib::PROCESS_DEBUG_ID loadDump( const std::wstring &fileName )
{
    AutoRestorePyState  pystate;
    return kdlib::loadDump(fileName);
}

inline 
void closeDump(kdlib::PROCESS_DEBUG_ID processId = -1)
{
    AutoRestorePyState  pystate;
    kdlib::closeDump(processId);
}

inline
void writeDump( const std::wstring &fileName, bool smallDump )
{
    AutoRestorePyState  pystate;
    kdlib::writeDump(fileName, smallDump);
}

inline
void attachKernel( const std::wstring &connectOptions = L"" )
{
    AutoRestorePyState  pystate;
    kdlib::attachKernel(connectOptions);
}

inline
bool isLocalKernelDebuggerEnabled()
{
    AutoRestorePyState  pystate;
    return kdlib::isLocalKernelDebuggerEnabled();
}

inline
bool isDumpAnalyzing()
{
    AutoRestorePyState  pystate;
    return kdlib::isDumpAnalyzing();
}

inline
bool isKernelDebugging()
{
    AutoRestorePyState  pystate;
    return kdlib::isKernelDebugging();
}


inline
python::object debugCommand( const std::wstring &command,  bool suppressOutput = true, const kdlib::OutputFlagsSet& captureFlags = kdlib::OutputFlag::Normal)
{
    std::wstring  debugResult;

    {
        AutoRestorePyState  pystate;
        debugResult = kdlib::debugCommand(command, suppressOutput, captureFlags);
    }

    if (debugResult.size() > 0 )
        return python::object(debugResult);

    return python::object();
}

inline
kdlib::DebugOptionsSet getDebugOptions()
{
    AutoRestorePyState  pystate;
    return kdlib::getDebugOptions();
}

inline
void changeDebugOptions(kdlib::DebugOptionsSet &addOptions, kdlib::DebugOptionsSet &removeOptions)
{
    AutoRestorePyState  pystate;
    kdlib::changeDebugOptions(addOptions, removeOptions);
}

inline
kdlib::OutputFlagsSet getOutputMask()
{
    AutoRestorePyState  pystate;
    return kdlib::getOutputFlags();
}

inline
void setOutputMask(const kdlib::OutputFlagsSet& outputMask)
{
    AutoRestorePyState  pystate;
    kdlib::setOutputFlags(outputMask);
}

inline
kdlib::DumpType getDumpType()
{
    AutoRestorePyState  pystate;
    return kdlib::getDumpType();
}

///////////////////////////////////////////////////////////////////////////////

// processes end threads
inline unsigned long getNumberThreads() 
{
    AutoRestorePyState  pystate;
    return kdlib::getNumberThreads();
}

inline kdlib::THREAD_DEBUG_ID getThreadIdByIndex(unsigned long index)
{
    AutoRestorePyState  pystate;
    return kdlib::getThreadIdByIndex(index);
}

inline kdlib::THREAD_DEBUG_ID getCurrentThreadId()
{
    AutoRestorePyState  pystate;
    return kdlib::getCurrentThreadId();
}

inline kdlib::THREAD_DEBUG_ID getThreadIdByOffset(kdlib::MEMOFFSET_64 offset)
{
    AutoRestorePyState  pystate;
    return kdlib::getThreadIdByOffset(offset);
}

inline kdlib::THREAD_DEBUG_ID getThreadIdBySystemId(kdlib::THREAD_ID tid = -1)
{
    AutoRestorePyState  pystate;
    return kdlib::getThreadIdBySystemId(tid);
}

inline kdlib::THREAD_ID getThreadSystemId(kdlib::THREAD_DEBUG_ID id = -1)
{   
    AutoRestorePyState  pystate;
    return kdlib::getThreadSystemId(id);
}

inline kdlib::MEMOFFSET_64 getThreadOffset(kdlib::THREAD_DEBUG_ID id = -1)
{  
    AutoRestorePyState  pystate;
    return kdlib::getThreadOffset(id);
}

inline kdlib::MEMOFFSET_64 getCurrentThread()
{  
    AutoRestorePyState  pystate;
    return kdlib::getCurrentThread();
}

inline void setCurrentThreadId(kdlib::THREAD_DEBUG_ID id)
{
    AutoRestorePyState  pystate;
    kdlib::setCurrentThreadById(id);
}

inline void setCurrentThread(kdlib::MEMOFFSET_64 offset)
{
    AutoRestorePyState  pystate;
    kdlib::setCurrentThread(offset);
}

inline void setImplicitThread(kdlib::MEMOFFSET_64 offset)
{
    AutoRestorePyState  pystate;
    kdlib::setImplicitThread(offset);
}

inline kdlib::MEMOFFSET_64 getImplicitThreadOffset()
{
    AutoRestorePyState  pystate;
    return kdlib::getImplicitThreadOffset();
}

inline unsigned long getNumberProcesses()
{
    AutoRestorePyState  pystate;
    return kdlib::getNumberProcesses();
}

inline kdlib::PROCESS_DEBUG_ID getProcessIdByIndex(unsigned long index)
{
    AutoRestorePyState  pystate;
    return kdlib::getProcessIdByIndex(index);
}

inline kdlib::MEMOFFSET_64 getCurrentProcess()
{
    AutoRestorePyState  pystate;
    return kdlib::getCurrentProcess();
}

inline kdlib::PROCESS_DEBUG_ID getCurrentProcessId()
{
    AutoRestorePyState  pystate;
    return kdlib::getCurrentProcessId();
}

inline void setCurrentProcess(kdlib::MEMOFFSET_64 offset)
{   
    AutoRestorePyState  pystate;
    kdlib::setCurrentProcess(offset);
}

inline void setCurrentProcessId( kdlib::PROCESS_DEBUG_ID id )
{
    AutoRestorePyState  pystate;
    kdlib::setCurrentProcessById(id);
}

inline kdlib::PROCESS_DEBUG_ID getProcessIdByOffset( kdlib::MEMOFFSET_64 offset )
{
    AutoRestorePyState  pystate;
    return kdlib::getProcessIdByOffset(offset);
}

inline kdlib::PROCESS_DEBUG_ID getProcessIdBySystemId( kdlib::PROCESS_ID pid)
{
    AutoRestorePyState  pystate;
    return kdlib::getProcessIdBySystemId(pid);
}

inline kdlib::PROCESS_ID  getProcessSystemId( kdlib::PROCESS_DEBUG_ID id = -1)
{
    AutoRestorePyState  pystate;
    return kdlib::getProcessSystemId(id);
}

inline kdlib::MEMOFFSET_64 getProcessOffset( kdlib::PROCESS_DEBUG_ID id  = -1)
{
    AutoRestorePyState  pystate;
    return kdlib::getProcessOffset(id);
}

inline std::wstring getProcessExecutableName(kdlib::PROCESS_DEBUG_ID id = -1)
{
    AutoRestorePyState  pystate;
    return kdlib::getProcessExecutableName(id);
}

inline void setImplicitProcess(kdlib::MEMOFFSET_64 offset)
{
    AutoRestorePyState  pystate;
    kdlib::setImplicitProcess(offset);
}

inline kdlib::MEMOFFSET_64 getImplicitProcessOffset()
{
    AutoRestorePyState  pystate;
    return kdlib::getImplicitProcessOffset();
}


python::list getProcessThreads();
python::list getTargetProcesses();
python::list getLocalProcesses();

std::wstring getHostProcessPath();

///////////////////////////////////////////////////////////////////////////////

inline
void targetBreak()
{
    AutoRestorePyState  pystate;
    kdlib::targetBreak();
}

inline
kdlib::ExecutionStatus targetGo()
{
    AutoRestorePyState  pystate;
    return kdlib::targetGo();
}

inline
kdlib::ExecutionStatus targetStep()
{
    AutoRestorePyState  pystate;
    return  kdlib::targetStep();
}

inline
kdlib::ExecutionStatus targetStepIn()
{
    AutoRestorePyState  pystate;
    return kdlib::targetStepIn();
}

inline
kdlib::ExecutionStatus targetStepOut()
{
    AutoRestorePyState  pystate;
    return kdlib::targetStepOut();
}

inline
kdlib::ExecutionStatus targetExecutionStatus()
{
    AutoRestorePyState  pystate;
    return kdlib::targetExecutionStatus();
}

inline
kdlib::ExecutionStatus sourceStep()
{
    AutoRestorePyState  pystate;
    return kdlib::sourceStepIn();
}

inline
kdlib::ExecutionStatus sourceStepOver()
{
    AutoRestorePyState  pystate;
    return kdlib::sourceStepOver();
}


///////////////////////////////////////////////////////////////////////////////
// system properties

inline
size_t ptrSize()
{
    AutoRestorePyState  pystate;
    return kdlib::ptrSize();
}

inline
bool is64bitSystem()
{
    AutoRestorePyState  pystate;
    return kdlib::is64bitSystem();
}

inline
size_t getPageSize()
{
    AutoRestorePyState  pystate;
    return kdlib::getPageSize();
}

inline
unsigned long getSystemUptime()
{
    AutoRestorePyState  pystate;
    return kdlib::getSystemUptime();
}

inline
unsigned long getCurrentTime()
{
    AutoRestorePyState  pystate;
    return kdlib::getCurrentTime();
}

///////////////////////////////////////////////////////////////////////////////

inline
std::wstring getExtensionSearchPath()
{
    AutoRestorePyState  pystate;
    return kdlib::getExtensionSearchPath();
}

inline
kdlib::EXTENSION_ID loadExtension( const std::wstring &extPath ) 
{
    AutoRestorePyState  pystate;
    return kdlib::loadExtension(extPath);
}

inline
void removeExtension( kdlib::EXTENSION_ID extId ) 
{
    AutoRestorePyState  pystate;
    kdlib::removeExtension(extId);
}

inline
void removeExtension( const std::wstring &extPath ) 
{
    AutoRestorePyState  pystate;
    kdlib::removeExtension(extPath);
}

inline
std::wstring callExtension( kdlib::EXTENSION_ID extId, const std::wstring command, const std::wstring  &params )
{
    AutoRestorePyState  pystate;
    return kdlib::callExtension(extId, command, params);
}

///////////////////////////////////////////////////////////////////////////////

python::object evaluate( const std::wstring  &expression, bool cplusplus = false );

python::tuple getSourceLine( kdlib::MEMOFFSET_64 offset = 0 );

inline std::wstring getSourceFile(kdlib::MEMOFFSET_64 offset = 0)
{
    AutoRestorePyState  pystate;
    return kdlib::getSourceFile(offset);
}

inline std::wstring getSourceFileFromSrcSrv(kdlib::MEMOFFSET_64 offset = 0)
{
    AutoRestorePyState  pystate;
    return kdlib::getSourceFileFromSrcSrv(offset);
}

kdlib::SystemInfo getSystemVersion();

std::wstring printSystemVersion( kdlib::SystemInfo& sysInfo );

python::list getExceptionInfoParameters( kdlib::ExceptionInfo& exceptionInfo );

std::wstring printExceptionInfo( kdlib::ExceptionInfo& exceptionInfo );

python::tuple getBugCheckData();

kdlib::SyntheticSymbol addSyntheticSymbol( kdlib::MEMOFFSET_64 offset, unsigned long size, const std::wstring &name );
void removeSyntheticSymbol(const kdlib::SyntheticSymbol& syntheticSymbol);
std::wstring printSyntheticSymbol(const kdlib::SyntheticSymbol& syntheticSymbol);

void addSyntheticModule(kdlib::MEMOFFSET_64 base, unsigned long size, const std::wstring &name, const std::wstring &path = std::wstring{});
void removeSyntheticModule(kdlib::MEMOFFSET_64 base);

///////////////////////////////////////////////////////////////////////////////

} //end namespace pykd
