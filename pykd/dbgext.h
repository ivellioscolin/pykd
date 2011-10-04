#pragma once

#include <dbgeng.h>
#include <dbghelp.h>

class DbgExt {

public:

    IDebugClient            *client;
    IDebugClient4           *client4;
    IDebugClient5           *client5;

    IDebugControl           *control;
    IDebugControl4          *control4;

    IDebugRegisters         *registers;

    IDebugSymbols           *symbols;
    IDebugSymbols2          *symbols2;
    IDebugSymbols3          *symbols3;

    IDebugDataSpaces        *dataSpaces;
    IDebugDataSpaces4       *dataSpaces4;

    IDebugAdvanced2         *advanced2;

    IDebugSystemObjects     *system;
    IDebugSystemObjects2    *system2;

    DbgExt( IDebugClient4 *client );

    ~DbgExt();

private:

    DbgExt      *m_previosExt;
};

extern DbgExt    *dbgExt;


class WindbgGlobalSession 
{
public:
    
    static
    boost::python::object
    global() {
        return windbgGlobalSession->main.attr("__dict__");
    }
    
    static 
    VOID
    StartWindbgSession() {
        if ( 1 == InterlockedIncrement( &sessionCount ) )
        {
            windbgGlobalSession = new WindbgGlobalSession();
        }
    }
    
    static
    VOID
    StopWindbgSession() {
        if ( 0 == InterlockedDecrement( &sessionCount ) )
        {
            delete windbgGlobalSession;
            windbgGlobalSession = NULL;
        }            
    }
    
    static
    bool isInit() {
        return windbgGlobalSession != NULL;
    }

    static
    VOID
    RestorePyState() {
        PyEval_RestoreThread( windbgGlobalSession->pyState );
    }    

    static
    VOID
    SavePyState() {
        windbgGlobalSession->pyState = PyEval_SaveThread();
    }    

private:

    WindbgGlobalSession();
    
    ~WindbgGlobalSession();
   
    boost::python::object           main;

    PyThreadState                   *pyState;

    static volatile LONG            sessionCount;      
    
    static WindbgGlobalSession      *windbgGlobalSession;     

};

bool isWindbgExt();
