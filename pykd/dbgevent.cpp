///////////////////////////////////////////////////////////////////////////////////
//// Load/Unload module events
///////////////////////////////////////////////////////////////////////////////////
//
#include "stdafx.h"
#include "dbgevent.h"
#include "dbgclient.h"


namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////

EventHandler::~EventHandler()
{
    m_handlerClient->SetEventCallbacks( NULL );
}

///////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////

HRESULT EventHandler::Breakpoint(
    __in PDEBUG_BREAKPOINT Bp
)
{
//    boost::python::dict bpParameters;
//
//    HRESULT hres;
//    ULONG Value = 0;
//    ULONG Value2 = 0;
//    ULONG64 Value64 = 0;
//    std::string str;
//
//#define _ADD_BP_ULONG(x)                                        \
//    hres = Bp->Get##x(&Value);                                  \
//    BOOST_ASSERT( SUCCEEDED( hres ) || hres == E_NOINTERFACE ); \
//    if (SUCCEEDED( hres ))                                      \
//        bpParameters[#x] = Value;
//
//#define _ADD_BP_ULONG2(x, n1, n2)                               \
//    hres = Bp->Get##x(&Value, &Value2);                         \
//    BOOST_ASSERT( SUCCEEDED( hres ) || hres == E_NOINTERFACE ); \
//    if (SUCCEEDED( hres ))                                      \
//    {                                                           \
//        bpParameters[n1] = Value; bpParameters[n2] = Value2;    \
//    }
//
//#define _ADD_BP_ULONG64(x)                                      \
//    hres = Bp->Get##x(&Value64);                                \
//    BOOST_ASSERT( SUCCEEDED( hres ) || hres == E_NOINTERFACE ); \
//    if (SUCCEEDED( hres ))                                      \
//        bpParameters[#x] = Value64;
//
//#define _ADD_BP_STR(x)                                          \
//    Value = 0;                                                  \
//    Bp->Get##x(NULL, 0, &Value);                                \
//    if (Value)                                                  \
//    {                                                           \
//        str.resize(Value + 1);                                  \
//        BOOST_VERIFY( SUCCEEDED(                                \
//            Bp->Get##x(&str[0], (ULONG)str.size(), NULL)        \
//        ) );                                                    \
//        if (!str.empty()) bpParameters[#x] = str.c_str();       \
//    }
//
//    _ADD_BP_ULONG(Id);
//    _ADD_BP_ULONG2(Type, "BreakType", "ProcType");
//    _ADD_BP_ULONG(Flags);
//    _ADD_BP_ULONG64(Offset);
//    _ADD_BP_ULONG2(DataParameters, "Size", "AccessType");
//    _ADD_BP_ULONG(PassCount);
//    _ADD_BP_ULONG(CurrentPassCount);
//    _ADD_BP_ULONG(MatchThreadId);
//    _ADD_BP_STR(Command);
//    _ADD_BP_STR(OffsetExpression);
//
//#undef  _ADD_BP_ULONG
//#undef  _ADD_BP_ULONG2
//#undef  _ADD_BP_ULONG64
//#undef  _ADD_BP_STR
//
    //PyThread_StateSave pyThreadSave;

    boost::python::dict         bpParameters;

    return onBreakpoint( bpParameters );

    //onException(bpParameters);
    //return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT EventHandler::Exception(
    __in PEXCEPTION_RECORD64 Exception,
    __in ULONG FirstChance
)
{
    python::list exceptParams;
    python::dict exceptData;

   // build list of parameters
    for (ULONG i = 0; i < Exception->NumberParameters; ++i)
        exceptParams.append(Exception->ExceptionInformation[i]);

    exceptData["Code"] = Exception->ExceptionCode;
    exceptData["Flags"] = Exception->ExceptionFlags;
    exceptData["Record"] = Exception->ExceptionRecord;
    exceptData["Address"] = Exception->ExceptionAddress;
    exceptData["Parameters"] = exceptParams;
    exceptData["FirstChance"] = (0 != FirstChance);

    PyThread_StateSave pyThreadSave( m_parentClient->getThreadState() );

    return onException(exceptData);
}

///////////////////////////////////////////////////////////////////////////////////

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
    //PyThread_StateSave pyThreadSave( m_parentClient->getThreadState() );

    //return onLoadModule( module );

    //std::auto_ptr<OutputReader> silentMode( new OutputReader(dbgExt->client) );

    //ULONG64         moduleBase;
    //ULONG           moduleSize;
    //std::string     moduleName;       
    //
    //queryModuleParams(BaseOffset, moduleName, moduleBase, moduleSize);
    //dbgModuleClass module(moduleName, moduleBase, moduleSize);
    //silentMode.reset(); 

    //PyThread_StateSave pyThreadSave;
    //return onLoadModule( module );

    return S_OK;

}

///////////////////////////////////////////////////////////////////////////////////

HRESULT EventHandler::UnloadModule(
    __in PCSTR ImageBaseName,
    __in ULONG64 BaseOffset
)
{
    //std::auto_ptr<OutputReader> silentMode( new OutputReader(dbgExt->client) );

    //ULONG64         moduleBase;
    //ULONG           moduleSize;
    //std::string     moduleName;       
    //
    //queryModuleParams(BaseOffset, moduleName, moduleBase, moduleSize);
    //dbgModuleClass module(moduleName, moduleBase, moduleSize);
    //silentMode.reset(); 

    //PyThread_StateSave pyThreadSave;
    //return onUnloadModule( module );
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT EventHandler::SessionStatus(
        __in ULONG  Status
)
{
    //PyThread_StateSave pyThreadSave;
    //return onChangeSessionStatus( Status );

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT EventHandler::ChangeDebuggeeState(
        __in ULONG  Flags,
        __in ULONG64 Argument
)
{
    //PyThread_StateSave pyThreadSave;
    //return onChangeDebugeeState();

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////


}; // end pykd namespace










//#include "dbgevent.h"
//#include "dbgio.h"
//#include "dbgexcept.h"
//#include "pyaux.h"
//
/////////////////////////////////////////////////////////////////////////////////////
//
//debugEvent::debugEvent()
//{
//     HRESULT    hres;
//    
//     hres = dbgExt->client->CreateClient( &m_debugClient );
//     if ( FAILED( hres ) )
//        throw DbgException( "IDebugClient::CreateClient" );    
//     
//     hres = m_debugClient->SetEventCallbacks(this);
//     if (FAILED(hres))
//        throw DbgException( "IDebugClient::SetEventCallbacks" );      
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//debugEvent::~debugEvent()
//{
//    m_debugClient->SetEventCallbacks( NULL );
//    
//    m_debugClient->Release();
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//HRESULT debugEvent::GetInterestMask(
//    __out PULONG Mask
//)    
//{
//    *Mask = 0;
//
//    *Mask |= DEBUG_EVENT_LOAD_MODULE;
//    *Mask |= DEBUG_EVENT_UNLOAD_MODULE;
//    *Mask |= DEBUG_EVENT_SESSION_STATUS;
//    *Mask |= DEBUG_EVENT_EXCEPTION;
//    *Mask |= DEBUG_EVENT_BREAKPOINT;
//
//    return S_OK;
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//HRESULT debugEvent::Breakpoint(
//    __in PDEBUG_BREAKPOINT Bp
//)
//{
//    boost::python::dict bpParameters;
//
//    HRESULT hres;
//    ULONG Value = 0;
//    ULONG Value2 = 0;
//    ULONG64 Value64 = 0;
//    std::string str;
//
//#define _ADD_BP_ULONG(x)                                        \
//    hres = Bp->Get##x(&Value);                                  \
//    BOOST_ASSERT( SUCCEEDED( hres ) || hres == E_NOINTERFACE ); \
//    if (SUCCEEDED( hres ))                                      \
//        bpParameters[#x] = Value;
//
//#define _ADD_BP_ULONG2(x, n1, n2)                               \
//    hres = Bp->Get##x(&Value, &Value2);                         \
//    BOOST_ASSERT( SUCCEEDED( hres ) || hres == E_NOINTERFACE ); \
//    if (SUCCEEDED( hres ))                                      \
//    {                                                           \
//        bpParameters[n1] = Value; bpParameters[n2] = Value2;    \
//    }
//
//#define _ADD_BP_ULONG64(x)                                      \
//    hres = Bp->Get##x(&Value64);                                \
//    BOOST_ASSERT( SUCCEEDED( hres ) || hres == E_NOINTERFACE ); \
//    if (SUCCEEDED( hres ))                                      \
//        bpParameters[#x] = Value64;
//
//#define _ADD_BP_STR(x)                                          \
//    Value = 0;                                                  \
//    Bp->Get##x(NULL, 0, &Value);                                \
//    if (Value)                                                  \
//    {                                                           \
//        str.resize(Value + 1);                                  \
//        BOOST_VERIFY( SUCCEEDED(                                \
//            Bp->Get##x(&str[0], (ULONG)str.size(), NULL)        \
//        ) );                                                    \
//        if (!str.empty()) bpParameters[#x] = str.c_str();       \
//    }
//
//    _ADD_BP_ULONG(Id);
//    _ADD_BP_ULONG2(Type, "BreakType", "ProcType");
//    _ADD_BP_ULONG(Flags);
//    _ADD_BP_ULONG64(Offset);
//    _ADD_BP_ULONG2(DataParameters, "Size", "AccessType");
//    _ADD_BP_ULONG(PassCount);
//    _ADD_BP_ULONG(CurrentPassCount);
//    _ADD_BP_ULONG(MatchThreadId);
//    _ADD_BP_STR(Command);
//    _ADD_BP_STR(OffsetExpression);
//
//#undef  _ADD_BP_ULONG
//#undef  _ADD_BP_ULONG2
//#undef  _ADD_BP_ULONG64
//#undef  _ADD_BP_STR
//
//    PyThread_StateSave pyThreadSave;
//    return onException(bpParameters);
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//HRESULT debugEvent::Exception(
//    __in PEXCEPTION_RECORD64 Exception,
//    __in ULONG FirstChance
//)
//{
//    boost::python::list exceptParams;
//    boost::python::dict exceptData;
//
//    // build list of parameters
//    for (ULONG i = 0; i < Exception->NumberParameters; ++i)
//        exceptParams.append(Exception->ExceptionInformation[i]);
//
//    // build dict of exception data
//#define _ADD_EXCEPTION_ENTRY(x) exceptData[#x] = Exception->Exception##x
//    _ADD_EXCEPTION_ENTRY(Code);
//    _ADD_EXCEPTION_ENTRY(Flags);
//    _ADD_EXCEPTION_ENTRY(Record);
//    _ADD_EXCEPTION_ENTRY(Address);
//#undef  _ADD_EXCEPTION_ENTRY
//
//    exceptData["Parameters"] = exceptParams;
//
//    exceptData["FirstChance"] = (0 != FirstChance);
//
//    PyThread_StateSave pyThreadSave;
//    return onException(exceptData);
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//HRESULT debugEvent::LoadModule(
//    __in ULONG64 ImageFileHandle,
//    __in ULONG64 BaseOffset,
//    __in ULONG ModuleSize,
//    __in PCSTR ModuleName,
//    __in PCSTR ImageName,
//    __in ULONG CheckSum,
//    __in ULONG TimeDateStamp
//)
//{
//    std::auto_ptr<OutputReader> silentMode( new OutputReader(dbgExt->client) );
//
//    ULONG64         moduleBase;
//    ULONG           moduleSize;
//    std::string     moduleName;       
//    
//    queryModuleParams(BaseOffset, moduleName, moduleBase, moduleSize);
//    dbgModuleClass module(moduleName, moduleBase, moduleSize);
//    silentMode.reset(); 
//
//    PyThread_StateSave pyThreadSave;
//    return onLoadModule( module );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//HRESULT debugEvent::UnloadModule(
//    __in PCSTR ImageBaseName,
//    __in ULONG64 BaseOffset
//)
//{
//    std::auto_ptr<OutputReader> silentMode( new OutputReader(dbgExt->client) );
//
//    ULONG64         moduleBase;
//    ULONG           moduleSize;
//    std::string     moduleName;       
//    
//    queryModuleParams(BaseOffset, moduleName, moduleBase, moduleSize);
//    dbgModuleClass module(moduleName, moduleBase, moduleSize);
//    silentMode.reset(); 
//
//    PyThread_StateSave pyThreadSave;
//    return onUnloadModule( module );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//HRESULT debugEvent::SessionStatus(
//        __in ULONG  Status
//)
//{
//    PyThread_StateSave pyThreadSave;
//    return onChangeSessionStatus( Status );
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
//HRESULT debugEvent::ChangeDebuggeeState(
//        __in ULONG  Flags,
//        __in ULONG64 Argument
//)
//{
//    PyThread_StateSave pyThreadSave;
//    return onChangeDebugeeState();
//}
//
/////////////////////////////////////////////////////////////////////////////////////
//
