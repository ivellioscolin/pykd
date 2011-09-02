#pragma once

///////////////////////////////////////////////////////////////////////////////

typedef PyThreadState *PyThreadStatePtr;
extern __declspec( thread ) PyThreadStatePtr ptrPyThreadState;

//  --> call back 
//  { PyThread_StateSave  state( winext->getThreadState() );
//    do_callback();
//  }
//
//  Если колбек был вызван и при этом у текщего потока сохранен контекст ( был вызов setExecutionStatus )
//  то перед выполнением питоновского кода нужно восстановить контекст, а после возврата управления,
//  снова сохранить его

class PyThread_StateSave {

public:

    PyThread_StateSave() 
        : m_bRestored(false)
    {
        if (ptrPyThreadState)
        {
            PyEval_RestoreThread( ptrPyThreadState );
            m_bRestored = true;
        }
    }

    ~PyThread_StateSave() {
        if ( m_bRestored )
            ptrPyThreadState = PyEval_SaveThread();
    }

private:
    bool m_bRestored;
};

// { PyThread_StateRestore   state;
//   long_or_block_opreration();
// }

class PyThread_StateRestore
{
public:

    PyThread_StateRestore() {
        ptrPyThreadState = PyEval_SaveThread();
    }

    ~PyThread_StateRestore() {
        PyEval_RestoreThread( ptrPyThreadState );
    }
};

///////////////////////////////////////////////////////////////////////////////