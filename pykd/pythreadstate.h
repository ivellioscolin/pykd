
#pragma once

#include <Python.h>

namespace pykd {

class AutoRestorePyState 
{
public:

    AutoRestorePyState() 
    {
        m_state =  PyEval_SaveThread();
    }

    explicit AutoRestorePyState(PyThreadState **state) 
    {
        *state = PyEval_SaveThread();
        m_state =  *state;
    }

    ~AutoRestorePyState() 
    {
        PyEval_RestoreThread( m_state );
    }

private:

    PyThreadState*    m_state;
};

class AutoSavePythonState
{
public:

    explicit AutoSavePythonState(PyThreadState **state) {
        PyEval_RestoreThread(*state);
        m_state = state;
    }

    ~AutoSavePythonState() {
        *m_state = PyEval_SaveThread();

    }

private:

    PyThreadState**    m_state;


};

}