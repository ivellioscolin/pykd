
#include "stdafx.h"
#include "dbgeng.h"
#include "dbgext.h"
#include "dbgeventcb.h"
#include "dbgexcept.h"
#include "dbgmodule.h"
#include "dbgsynsym.h"
#include "dbgcmd.h"

///////////////////////////////////////////////////////////////////////////////////

DbgEventCallbacksManager::DbgEventCallbacksManager( IDebugClient  *client )
{
    HRESULT     hres;

    m_debugClient = NULL;
    
    try {

        if ( client == NULL )
        {
            // случай, когда мы работаем в windbg. ћы не хотим мен€ть поведение клиента отладчика - он
            // должен продолжать обработку событий, поэтому мы создаем своего клиента
            hres = DebugCreate( __uuidof(IDebugClient4),  reinterpret_cast<PVOID*>(&m_debugClient));
            if (FAILED(hres))
                throw DbgException( "DebugCreate failed" );  
        }
        else
        {
            // случай, когда мы работаем отдельно. ¬ этом случае клиент весь в нашем распор€жении
            hres =client->QueryInterface( __uuidof(IDebugClient4),  reinterpret_cast<PVOID*>(&m_debugClient));
            if (FAILED(hres))
                throw DbgException( "DebugCreate failed" );
        }                
            
        hres = m_debugClient->SetEventCallbacks(this);
        if (FAILED(hres))
            throw DbgException( "IDebugClient::SetEventCallbacks" );           
            
    }    
  	catch( std::exception& )
	{
		//dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		//dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}
}

///////////////////////////////////////////////////////////////////////////////////

DbgEventCallbacksManager::~DbgEventCallbacksManager()
{
    if ( m_debugClient )
        m_debugClient->Release();
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacksManager::GetInterestMask(
    __out PULONG Mask
)
{
    *Mask =  DEBUG_EVENT_CHANGE_SYMBOL_STATE | DEBUG_EVENT_BREAKPOINT;
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacksManager::ChangeSymbolState(
    __in ULONG Flags,
    __in ULONG64 Argument
)
{
    DbgExt      ext( m_debugClient );

    if ((DEBUG_CSS_LOADS & Flags))
    {
        if (Argument)
        {
            DEBUG_MODULE_PARAMETERS dbgModuleParameters={};
            
            HRESULT hres = dbgExt->symbols3->GetModuleParameters(
                1,
                &Argument,
                0,
                &dbgModuleParameters);
    
            if (SUCCEEDED(hres))
            {
                 ModuleInfo     moduleInfo(dbgModuleParameters);
                 
                 restoreSyntheticSymbolForModule(moduleInfo);
            }        
            
            return S_OK;        
        }

        //// f.e. is case ".reload /f image.exe", if for image.exe no symbols
        restoreSyntheticSymbolForAllModules();
        
        return S_OK;
    }

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT DbgEventCallbacksManager::Breakpoint(
    __in IDebugBreakpoint *  bp
)
{
    return dbgBreakpointClass::onBreakpointEvnet( bp );
}    

///////////////////////////////////////////////////////////////////////////////////

