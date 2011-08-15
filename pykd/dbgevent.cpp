///////////////////////////////////////////////////////////////////////////////////
//// Load/Unload module events
///////////////////////////////////////////////////////////////////////////////////
//
#include "stdafx.h"
#include "dbgevent.h"
#include "dbgio.h"
#include "dbgexcept.h"
#include "pyaux.h"

///////////////////////////////////////////////////////////////////////////////////

debugEvent::debugEvent()
{
     HRESULT    hres;
    
     hres = dbgExt->client->CreateClient( &m_debugClient );
     if ( FAILED( hres ) )
        throw DbgException( "IDebugClient::CreateClient" );    
     
     hres = m_debugClient->SetEventCallbacks(this);
     if (FAILED(hres))
        throw DbgException( "IDebugClient::SetEventCallbacks" );      
}

///////////////////////////////////////////////////////////////////////////////////

debugEvent::~debugEvent()
{
    m_debugClient->SetEventCallbacks( NULL );
    
    m_debugClient->Release();
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT debugEvent::GetInterestMask(
    __out PULONG Mask
)    
{
    *Mask = 0;

    *Mask |= DEBUG_EVENT_LOAD_MODULE;
    *Mask |= DEBUG_EVENT_UNLOAD_MODULE;  
    *Mask |= DEBUG_EVENT_SESSION_STATUS;
        
    return S_OK;     
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT debugEvent::LoadModule(
    __in ULONG64 ImageFileHandle,
    __in ULONG64 BaseOffset,
    __in ULONG ModuleSize,
    __in PCSTR ModuleName,
    __in PCSTR ImageName,
    __in ULONG CheckSum,
    __in ULONG TimeDateStamp
)
{
    std::auto_ptr<OutputReader> silentMode( new OutputReader(dbgExt->client) );

    ULONG64         moduleBase;
    ULONG           moduleSize;
    std::string     moduleName;       
    
    queryModuleParams(BaseOffset, moduleName, moduleBase, moduleSize);
    dbgModuleClass module(moduleName, moduleBase, moduleSize);
    silentMode.reset(); 

    PyThread_StateSave( dbgExt->getThreadState() ); 
    
    return onLoadModule( module );
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT debugEvent::UnloadModule(
    __in PCSTR ImageBaseName,
    __in ULONG64 BaseOffset
)
{
    std::auto_ptr<OutputReader> silentMode( new OutputReader(dbgExt->client) );

    ULONG64         moduleBase;
    ULONG           moduleSize;
    std::string     moduleName;       
    
    queryModuleParams(BaseOffset, moduleName, moduleBase, moduleSize);
    dbgModuleClass module(moduleName, moduleBase, moduleSize);
    silentMode.reset(); 

    PyThread_StateSave( dbgExt->getThreadState() ); 
    
    return onUnloadModule( module );
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT debugEvent::SessionStatus(
        __in ULONG  Status
)
{
    PyThread_StateSave( dbgExt->getThreadState() ); 

    return onChangeSessionStatus( Status );
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT debugEvent::ChangeDebuggeeState(
        __in ULONG  Flags,
        __in ULONG64 Argument
)
{
    PyThread_StateSave( dbgExt->getThreadState() ); 

    return onChangeDebugeeState();
}

///////////////////////////////////////////////////////////////////////////////////
      
