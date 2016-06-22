#pragma once

#include <string>
#include <list>

#include "pymodule.h"

class PythonInterpreter;

PythonInterpreter*  activateInterpreter(bool global = true, int majorVersion = -1, int minorVersion = -1);


void releaseInterpretor(PythonInterpreter* interpret);

struct  InterpreterDesc {
    int  majorVersion;
    int  minorVersion;
    std::string  imagePath;
};

inline bool operator < (const InterpreterDesc& d1, const InterpreterDesc& d2)
{
    if (d1.majorVersion != d2.majorVersion)
        return d1.majorVersion < d2.majorVersion;

    if (d1.minorVersion != d2.minorVersion)
        return d1.minorVersion < d2.minorVersion;

    return d1.imagePath < d2.imagePath;
}

std::list<InterpreterDesc>  getInstalledInterpreter();

bool isInterpreterLoaded(int majorVersion, int minorVersion);

void stopAllInterpreter();

void checkPykd();


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






