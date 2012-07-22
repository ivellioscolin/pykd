////////////////////////////////////////////////////////////////////////////////
// User-customizing debug event handler
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dbgevent.h"
#include "dbgclient.h"

////////////////////////////////////////////////////////////////////////////////

std::wstring fetchPythonExceptionStrings( python::object &tracebackModule );

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

EventHandler::EventHandler()
{
     HRESULT    hres;
    
     hres = g_dbgClient->client()->CreateClient( &m_handlerClient );
     if ( FAILED( hres ) )
        throw DbgException( "IDebugClient::CreateClient" );    
     
     hres = m_handlerClient->SetEventCallbacks(this);
     if (FAILED(hres))
        throw DbgException( "IDebugClient::SetEventCallbacks" );

     m_parentClient = g_dbgClient;
}

////////////////////////////////////////////////////////////////////////////////

EventHandler::EventHandler( DebugClientPtr  &client )
{
     HRESULT    hres;
    
     hres = client->client()->CreateClient( &m_handlerClient );
     if ( FAILED( hres ) )
        throw DbgException( "IDebugClient::CreateClient" );    
     
     hres = m_handlerClient->SetEventCallbacks(this);
     if (FAILED(hres))
        throw DbgException( "IDebugClient::SetEventCallbacks" );

     m_parentClient = client;
}

////////////////////////////////////////////////////////////////////////////////

EventHandler::~EventHandler()
{
}

////////////////////////////////////////////////////////////////////////////////

HRESULT EventHandler::GetInterestMask(
    __out PULONG Mask
)    
{
    *Mask = 0;

    *Mask |= DEBUG_EVENT_LOAD_MODULE;
    *Mask |= DEBUG_EVENT_UNLOAD_MODULE;
    *Mask |= DEBUG_EVENT_SESSION_STATUS;
    *Mask |= DEBUG_EVENT_EXCEPTION;
    *Mask |= DEBUG_EVENT_BREAKPOINT;

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////

HRESULT EventHandler::Breakpoint(
    __in PDEBUG_BREAKPOINT Bp
)
{
    ULONG Id;
    HRESULT hres = Bp->GetId(&Id);
    if (S_OK == hres)
    {
        PyThread_StateSave pyThreadSave( m_parentClient->getThreadState() );
        return onBreakpoint(Id);
    }

    return DEBUG_STATUS_NO_CHANGE;
}

////////////////////////////////////////////////////////////////////////////////

HRESULT EventHandler::Exception(
    __in PEXCEPTION_RECORD64 Exception,
    __in ULONG FirstChance
)
{
    python::list exceptParams;
    python::dict exceptData;

   // build list of parameters
    for (ULONG i = 0; i < Exception->NumberParameters; ++i)
        exceptParams.append( Exception->ExceptionInformation[i] );

    exceptData["Code"] = Exception->ExceptionCode;
    exceptData["Flags"] = Exception->ExceptionFlags;
    exceptData["Record"] = Exception->ExceptionRecord;
    exceptData["Address"] = Exception->ExceptionAddress;
    exceptData["Parameters"] = exceptParams;
    exceptData["FirstChance"] = (0 != FirstChance);

    PyThread_StateSave pyThreadSave( m_parentClient->getThreadState() );

    return onException(exceptData);
}

////////////////////////////////////////////////////////////////////////////////

HRESULT EventHandler::LoadModule(
    __in ULONG64 ImageFileHandle,
    __in ULONG64 BaseOffset,
    __in ULONG ModuleSize,
    __in PCSTR ModuleName,
    __in PCSTR ImageName,
    __in ULONG CheckSum,
    __in ULONG TimeDateStamp
)
{
    PyThread_StateSave pyThreadSave( m_parentClient->getThreadState() );
    return onLoadModule( m_parentClient->loadModuleByOffset(BaseOffset) );
}

////////////////////////////////////////////////////////////////////////////////

HRESULT EventHandler::UnloadModule(
    __in PCSTR ImageBaseName,
    __in ULONG64 BaseOffset
)
{
    PyThread_StateSave pyThreadSave( m_parentClient->getThreadState() );
    BaseOffset = m_parentClient->addr64(BaseOffset);
    return onUnloadModule( BaseOffset );
}

////////////////////////////////////////////////////////////////////////////////

HRESULT EventHandler::SessionStatus(
        __in ULONG  Status
)
{
    //PyThread_StateSave pyThreadSave;
    //return onChangeSessionStatus( Status );

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////

HRESULT EventHandler::ChangeDebuggeeState(
        __in ULONG  Flags,
        __in ULONG64 Argument
)
{
    //PyThread_StateSave pyThreadSave;
    //return onChangeDebugeeState();

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////

void EventHandlerWrap::onHandlerException()
{
    // TODO: some logging, alerting....
}

////////////////////////////////////////////////////////////////////////////////


}; // end pykd namespace
