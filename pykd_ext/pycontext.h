#pragma once

#include "pyapi.h"

class AutoRestorePyState
{
public:

    AutoRestorePyState()
    {
        m_state = PyEval_SaveThread();
    }

    ~AutoRestorePyState()
    {
        PyEval_RestoreThread(m_state);
    }

private:

    PyThreadState*    m_state;
};

