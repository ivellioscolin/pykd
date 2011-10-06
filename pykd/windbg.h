#pragma once


namespace pykd {

///////////////////////////////////////////////////////////////////////////////////


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
    
    ~WindbgGlobalSession() {
    }
   
    python::object                  main;

    PyThreadState                   *pyState;

    static volatile LONG            sessionCount;      
    
    static WindbgGlobalSession      *windbgGlobalSession;     

};   

///////////////////////////////////////////////////////////////////////////////////


}; // namespace pykd