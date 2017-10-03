#pragma once

#include <kdlib/process.h>

#include "pythreadstate.h"
#include "pyeventhandler.h"
#include "dbgexcept.h"

namespace pykd {

struct TargetSystemAdapter {

    static unsigned long getNumberSystems()
    {
        AutoRestorePyState  pystate;
        return kdlib::TargetSystem::getNumber();
    }

    static kdlib::TargetSystemPtr getCurrent()
    {
        AutoRestorePyState  pystate;
        return kdlib::TargetSystem::getCurrent();
    }

    static kdlib::TargetSystemPtr getSystem(unsigned long index)
    {
        AutoRestorePyState  pystate;
        return kdlib::TargetSystem::getByIndex(index);
    }

    static kdlib::TargetSystemPtr getSystemById(kdlib::SYSTEM_DEBUG_ID id)
    {
        AutoRestorePyState  pystate;
        return kdlib::TargetSystem::getById(id);
    }

    static void setCurrent(kdlib::TargetSystem& system)
    {
        AutoRestorePyState  pystate;
        system.setCurrent();
    }

    static std::wstring getDescription(kdlib::TargetSystem& system)
    {
        AutoRestorePyState  pystate;
        return system.getDescription();
    }

    static kdlib::SYSTEM_DEBUG_ID getId(kdlib::TargetSystem& system)
    {
        AutoRestorePyState  pystate;
        return system.getId();
    }

    static bool isDumpAnalyzing(kdlib::TargetSystem& system)
    {
        AutoRestorePyState  pystate;
        return system.isDumpAnalyzing();
    }

    static bool isKernelDebugging(kdlib::TargetSystem& system)
    {
        AutoRestorePyState  pystate;
        return system.isKernelDebugging();
    }

    static bool is64bitSystem(kdlib::TargetSystem& system)
    {
        AutoRestorePyState  pystate;
        return system.is64bitSystem();
    }

    static unsigned long getNumberProcesses(kdlib::TargetSystem& system)
    {
        AutoRestorePyState  pystate;
        return system.getNumberProcesses();
    }

    static kdlib::TargetProcessPtr getProcessByIndex(kdlib::TargetSystem& system, unsigned long index)
    {
        AutoRestorePyState  pystate;
        return system.getProcessByIndex(index);
    }

    static kdlib::TargetProcessPtr getProcessById(kdlib::TargetSystem& system, kdlib::PROCESS_DEBUG_ID id)
    {
        AutoRestorePyState  pystate;
        return system.getProcessById(id);
    }

    static kdlib::TargetProcessPtr  getProcessBySystemId(kdlib::TargetSystem& system, kdlib::PROCESS_ID pid)
    {
        AutoRestorePyState  pystate;
        return system.getProcessBySystemId(pid);
    }

    static kdlib::TargetProcessPtr getCurrentProcess(kdlib::TargetSystem& system)
    {
        AutoRestorePyState  pystate;
        return system.getCurrentProcess(); 
    }

    static bool isCurrent(kdlib::TargetSystem& system)
    {
        AutoRestorePyState  pystate;
        return system.isCurrent();
    }

    static python::list getProcessesList(kdlib::TargetSystem& system);

    static std::wstring print(kdlib::TargetSystem& system);
};


struct TargetProcessAdapter {

    static kdlib::TargetProcessPtr getCurrent()
    {
        AutoRestorePyState  pystate;
        return kdlib::TargetProcess::getCurrent();
    }

    static kdlib::TargetProcessPtr getProcess(unsigned long index)
    {
        AutoRestorePyState  pystate;
        return kdlib::TargetProcess::getByIndex(index);
    }

    static kdlib::TargetProcessPtr getProcessById(kdlib::PROCESS_DEBUG_ID id)
    {
        AutoRestorePyState  pystate;
        return kdlib::TargetProcess::getById(id);
    }

    static unsigned long getNumberProcesses()
    {
        AutoRestorePyState  pystate;
        return kdlib::TargetProcess::getNumber();
    }

    static void setCurrent(kdlib::TargetProcess& process)
    {   
        AutoRestorePyState  pystate;
        return process.setCurrent();
    }

    static kdlib::PROCESS_ID getSystemId(kdlib::TargetProcess& process)
    {
        AutoRestorePyState  pystate;
        return process.getSystemId();
    }

    static kdlib::PROCESS_DEBUG_ID getId(kdlib::TargetProcess& process)
    {
        AutoRestorePyState  pystate;
        return process.getId();
    }

    static kdlib::MEMOFFSET_64 getPebOffset(kdlib::TargetProcess& process)
    {
        AutoRestorePyState  pystate;
        return process.getPebOffset();
    }

    static std::wstring getExeName(kdlib::TargetProcess& process)
    {
        AutoRestorePyState  pystate;
        return process.getExecutableName();
    }

    static unsigned long getNumberThreads(kdlib::TargetProcess& process )
    {
        AutoRestorePyState  pystate;
        return process.getNumberThreads();
    }

    static kdlib::TargetThreadPtr getThreadByIndex(kdlib::TargetProcess& process, unsigned long index) 
    {
        AutoRestorePyState  pystate;
        return process.getThreadByIndex(index);
    }

    static kdlib::TargetThreadPtr getThreadById(kdlib::TargetProcess& process, kdlib::THREAD_DEBUG_ID id)
    {
        AutoRestorePyState  pystate;
        return process.getThreadById(id);
    }

    static kdlib::TargetThreadPtr getThreadBySystemId(kdlib::TargetProcess& process, kdlib::THREAD_ID tid)
    {
        AutoRestorePyState  pystate;
        return process.getThreadBySystemId(tid);
    }

    static kdlib::TargetThreadPtr getCurrentThread(kdlib::TargetProcess& process)
    {
        AutoRestorePyState  pystate;
        return process.getCurrentThread();
    }

    static unsigned long getNumberBreakpoints(kdlib::TargetProcess& process)
    {
        AutoRestorePyState  pystate;
        return process.getNumberBreakpoints();
    }

    static Breakpoint* getBreakpointByIndex(kdlib::TargetProcess& process, unsigned long index)
    {
        kdlib::BreakpointPtr  bp;

        {
            AutoRestorePyState  pystate;
            bp = process.getBreakpoint(index);
        }

        return new Breakpoint(bp);
    }

    static bool isCurrent(kdlib::TargetProcess& process)
    {
        AutoRestorePyState  pystate;
        return process.isCurrent();
    }

    static unsigned long getNumberModules(kdlib::TargetProcess& process)
    {
        AutoRestorePyState  pystate;
        return process.getNumberModules();
    }

    static kdlib::ModulePtr getModuleByIndex(kdlib::TargetProcess& process, unsigned long index)
    {
        AutoRestorePyState  pystate;
        return process.getModuleByIndex(index);
    }

    static kdlib::ModulePtr getModuleByOffset(kdlib::TargetProcess& process, kdlib::MEMOFFSET_64 offset)
    {
        AutoRestorePyState  pystate;
        return process.getModuleByOffset(offset);
    }

    static kdlib::ModulePtr getModuleByName(kdlib::TargetProcess& process, const std::wstring& name)
    {
        AutoRestorePyState  pystate;
        return process.getModuleByName(name);
    }

    static bool isManaged(kdlib::TargetProcess& process) 
    {
        AutoRestorePyState  pystate;
        return process.isManaged();
    }

    static kdlib::TargetHeapPtr getManagedHeap(kdlib::TargetProcess& process) 
    {
        AutoRestorePyState  pystate;
        return process.getManagedHeap();
    }

    static kdlib::TypedVarPtr getManagedVar(kdlib::TargetProcess& process, kdlib::MEMOFFSET_64 address)
    {
        AutoRestorePyState  pystate;
        return process.getManagedVar(address);
    }

    static python::list getThreadList(kdlib::TargetProcess& process);

    static python::list getBreakpointsList(kdlib::TargetProcess& process);

    static python::list getModulesList(kdlib::TargetProcess& process);

    static std::wstring print(kdlib::TargetProcess& process);
};


struct TargetThreadAdapter {

    static unsigned long getNumberThreads()
    {
        AutoRestorePyState  pystate;
        return kdlib::TargetThread::getNumber();
    }

    static kdlib::TargetThreadPtr getCurrent()
    {
        AutoRestorePyState  pystate;
        return  kdlib::TargetThread::getCurrent();
    }

    static kdlib::TargetThreadPtr getThread(unsigned long index)
    {
        AutoRestorePyState  pystate;
        return kdlib::TargetThread::getByIndex(index);
    }

    static kdlib::TargetThreadPtr getThreadById(kdlib::THREAD_DEBUG_ID id)
    {
        AutoRestorePyState  pystate;
        return kdlib::TargetThread::getById(id);
    }

    static kdlib::THREAD_DEBUG_ID getId(kdlib::TargetThread& thread)
    {
        AutoRestorePyState  pystate;
        return thread.getId();
    }

    static kdlib::THREAD_ID getSystemId(kdlib::TargetThread& thread )
    {
        AutoRestorePyState  pystate;
        return thread.getSystemId();
    }

    static kdlib::MEMOFFSET_64 getTebOffset(kdlib::TargetThread& thread )
    {
        AutoRestorePyState  pystate;
        return thread.getTebOffset();
    }

    static void setCurrent(kdlib::TargetThread& thread)
    {
        AutoRestorePyState  pystate;
        return thread.setCurrent();
    }

    static bool isCurrent(kdlib::TargetThread& thread)
    {
        AutoRestorePyState  pystate;
        return thread.isCurrent();
    }

    static python::list getStack(kdlib::TargetThread& thread);

    static kdlib::MEMOFFSET_64  getIP(kdlib::TargetThread& thread)
    {
        AutoRestorePyState  pystate;
        return thread.getInstructionOffset();
    }
   
    static kdlib::MEMOFFSET_64  getSP(kdlib::TargetThread& thread)
    {
        AutoRestorePyState  pystate;
        return thread.getStackOffset();
    }

    static kdlib::MEMOFFSET_64  getFP(kdlib::TargetThread& thread)
    {
        AutoRestorePyState  pystate;
        return thread.getFrameOffset();
    }

    static std::wstring print(kdlib::TargetThread& thread);
};


class TargetHeapIterator
{
public:

    explicit TargetHeapIterator(kdlib::TargetHeapEnumPtr&  heapEnum) :
        m_heapEnum(heapEnum)
    {}

    static python::object self(const python::object& obj)
    {
        return obj;
    }

    python::tuple next()
    {
        AutoRestorePyState  pystate;

        kdlib::MEMOFFSET_64  addr;
        std::wstring  name;
        size_t  size;

        if (!m_heapEnum->next(addr, name, size))
            throw StopIteration("No more data.");

        return python::make_tuple(addr, name, size);
    }

    size_t length()
    {
        AutoRestorePyState  pystate;
        return m_heapEnum->getCount();
    }

private:

    kdlib::TargetHeapEnumPtr  m_heapEnum;
};

struct TargetHeapAdapter {

    static TargetHeapIterator* getEntries(kdlib::TargetHeap& heap, const std::wstring& typeName=L"", size_t minSize=0, size_t maxSize=-1)
    {
        return new TargetHeapIterator(heap.getEnum(typeName, minSize, maxSize));
    }

};

} // pykd namespace

