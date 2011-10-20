#pragma once

#include <windows.h>

#include "windbg.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

class PyThreadStateSaver {

public:

    PyThreadStateSaver() {
        m_index = TlsAlloc();
    }

    ~PyThreadStateSaver() {
        TlsFree( m_index );
    }

    void saveState() {
        if ( !WindbgGlobalSession::isInit() )
            TlsSetValue( m_index, PyEval_SaveThread() );
        else
            WindbgGlobalSession::SavePyState();                
    }

    void restoreState() {
        if ( !WindbgGlobalSession::isInit() )
        {
            PyThreadState*      state = (PyThreadState*)TlsGetValue( m_index );
            if ( state )
                PyEval_RestoreThread( state );
        }
        else
        {
            WindbgGlobalSession::RestorePyState();
        }
    }

private:

    DWORD   m_index;
};


//  --> call back 
//  { PyThread_StateSave  state( winext->getThreadState() );
//    do_callback();
//  }
//
//  ≈сли  был вызван колбек то перед выполнением питоновского кода нужно восстановить контекст,
//  а после возврата управлени€,  снова сохранить его

class PyThread_StateSave {

public:

    PyThread_StateSave( PyThreadStateSaver  &threadState) :
        m_threadState( threadState )
    {
        m_threadState.restoreState();
    }

    ~PyThread_StateSave() {
        m_threadState.saveState();
    }

private:

    PyThreadStateSaver      &m_threadState;

};

// { PyThread_StateRestore   state;
//   long_or_block_opreration();
// }

class PyThread_StateRestore
{
public:

    PyThread_StateRestore(PyThreadStateSaver  &threadState) :
        m_threadState( threadState )
    {
        m_threadState.saveState();
    }

    ~PyThread_StateRestore() {
        m_threadState.restoreState();
    }
private:

    PyThreadStateSaver      &m_threadState;
};

/////////////////////////////////////////////////////////////////////////////////

}; //end namespace pykd