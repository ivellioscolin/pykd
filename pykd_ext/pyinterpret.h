#pragma once

#include <string>
#include <list>

#include "pymodule.h"

class PythonInterpreter;

PythonInterpreter*  activateInterpreter(bool global = true, int majorVersion = -1, int minorVersion = -1);

void releaseInterpretor(PythonInterpreter* interpret);

class AutoInterpreter
{
public:

    explicit AutoInterpreter(bool global = true, int majorVersion = -1, int minorVersion = -1)
    {
        m_interpreter = activateInterpreter(global, majorVersion, minorVersion);
    }
    
    ~AutoInterpreter()
    {
        if (m_interpreter)
            releaseInterpretor(m_interpreter);
    }

private:

    AutoInterpreter(const AutoInterpreter&) = delete;

    PythonInterpreter*  m_interpreter;
};


struct  InterpreterDesc {
    int  majorVersion;
    int  minorVersion;
    std::string  imagePath;
};

std::list<InterpreterDesc>  getInstalledInterpreter();



