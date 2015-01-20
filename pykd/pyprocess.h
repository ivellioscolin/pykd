#pragma once

#include <kdlib/process.h>

#include "pythreadstate.h"

namespace pykd {

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

    static unsigned long getNumberProcesses()
    {
        AutoRestorePyState  pystate;
        return kdlib::TargetProcess::getNumber();
    }

    static kdlib::PROCESS_ID getSystemId(kdlib::TargetProcess& process)
    {
        AutoRestorePyState  pystate;
        return process.getSystemId();
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

    static kdlib::TargetThreadPtr getCurrentThread(kdlib::TargetProcess& process)
    {
        AutoRestorePyState  pystate;
        return process.getCurrentThread();
    }
};


struct TargetThreadAdapter {

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
};

} // pykd namespace

