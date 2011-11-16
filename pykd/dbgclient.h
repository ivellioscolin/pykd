#pragma once

#include <string>
#include <dbgeng.h>
#include <dbghelp.h>

#include <boost\smart_ptr\scoped_ptr.hpp>

#include "dbgobj.h"
#include "dbgexcept.h"
#include "module.h"
#include "dbgio.h"
#include "dbgcmd.h"
#include "pyaux.h"
#include "disasm.h"

/////////////////////////////////////////////////////////////////////////////////

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////

class DebugClient;
typedef boost::shared_ptr<DebugClient>  DebugClientPtr;

/////////////////////////////////////////////////////////////////////////////////

class DebugClient : private DbgObject {

public:

    virtual ~DebugClient() {}

    static
    DebugClientPtr createDbgClient() ;

    static
    DebugClientPtr createDbgClient( IDebugClient4 *client );

    static
    DebugClientPtr  setDbgClientCurrent( DebugClientPtr  newDbgClient );

public:

    ULONG64  addr64( ULONG64 addr );

    DbgOut  dout() {
        return DbgOut( m_client );
    }

    DbgIn din() {
        return DbgIn( m_client );
    }

    std::string dbgCommand( const std::wstring  &command );

    void startProcess( const std::wstring  &processName );

    void attachProcess( ULONG  processId );

    void attachKernel( const std::wstring  &param );

    Disasm disasm( ULONG offset = 0 ) {
        return Disasm( m_client, offset );
    }

    void dprint( const std::wstring &str, bool dml = false );

    void dprintln( const std::wstring &str, bool dml = false );

    void eprint( const std::wstring &str );

    void eprintln( const std::wstring &str );

    ULONG64 evaluate( const std::wstring  &expression );

    python::tuple getDebuggeeType();

    ULONG getExecutionStatus();

    template<ULONG status>
    void changeDebuggerStatus();

    bool isKernelDebugging();

    bool isDumpAnalyzing();

    void loadDump( const std::wstring &fileName );

    Module loadModule( const std::string  &moduleName ) {
        return Module( m_client, moduleName );
    }

    Module findModule( ULONG64  offset ) {
        return Module( m_client, offset ); 
    }

    DbgExtensionPtr loadExtension( const std::wstring &extPath ) {
        return DbgExtensionPtr( new DbgExtension( m_client, extPath ) );
    }

    python::list loadBytes( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

    python::list loadWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

    python::list loadDWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

    python::list loadQWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

    python::list loadSignBytes( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

    python::list loadSignWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

    python::list loadSignDWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

    python::list loadSignQWords( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

    std::string loadChars( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

    std::wstring loadWChars( ULONG64 offset, ULONG count, bool phyAddr = FALSE );

    ULONG ptrSize();

    LONG64 ptrByte();

    LONG64 ptrWord();

    LONG64 ptrDWord();

    LONG64 ptrQWord();

    LONG64 ptrMWord();

    ULONG64 ptrSignByte();

    ULONG64 ptrSignWord();

    ULONG64 ptrSignDWord();

    ULONG64 ptrSignQWord();

    ULONG64 ptrSignMWord();

    void readMemory( ULONG64 address, PVOID buffer, ULONG length, bool phyAddr = FALSE );

    void setExecutionStatus( ULONG status );
    
    void waitForEvent();

    void writeMemory( ULONG64 address, PVOID buffer, ULONG length, bool phyAddr = FALSE );

public:

    CComPtr<IDebugClient4>&
    client() {
        return m_client;    
    }

    CComPtr<IDebugClient5>&
    client5() {
        return m_client5;    
    }

    CComPtr<IDebugControl4>&
    control() {
        return m_control;    
    }


    PyThreadStateSaver&
    getThreadState() {
        return m_pyThreadState;
    }

private:

    template<typename T>
    python::list
    loadArray( ULONG64 offset, ULONG count, bool phyAddr );

    //python::list
    //loadArray( ULONG64 offset, ULONG count, bool phyAddr );

    DebugClient( IDebugClient4 *client ) : DbgObject( client ) {}

    PyThreadStateSaver      m_pyThreadState;
};

/////////////////////////////////////////////////////////////////////////////////

extern DebugClientPtr     g_dbgClient;

void loadDump( const std::wstring &fileName );

void startProcess( const std::wstring  &processName );

void attachProcess( ULONG  processId );

void attachKernel( const std::wstring  &param );

python::tuple getDebuggeeType();

ULONG getExecutionStatus();

bool isKernelDebugging();

bool isDumpAnalyzing();

ULONG ptrSize();

void setExecutionStatus( ULONG status );

void waitForEvent();

/////////////////////////////////////////////////////////////////////////////////

template<ULONG status>
void DebugClient::changeDebuggerStatus()
{
    HRESULT     hres;

    hres = m_control->SetExecutionStatus( status );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::SetExecutionStatus failed" );

    ULONG    currentStatus;

    do {

        waitForEvent();

        hres = m_control->GetExecutionStatus( &currentStatus );

        if ( FAILED( hres ) )
            throw  DbgException( "IDebugControl::GetExecutionStatus  failed" ); 

    } while( currentStatus != DEBUG_STATUS_BREAK && currentStatus != DEBUG_STATUS_NO_DEBUGGEE );
}

template<ULONG status>
void changeDebuggerStatus()
{
    g_dbgClient->changeDebuggerStatus<status>();
}

/////////////////////////////////////////////////////////////////////////////////


};  // namespace pykd










//#include "dbgext.h"
//#include "dbgeventcb.h"
//
/////////////////////////////////////////////////////////////////////////////////
//
//class dbgClient {
//
//public:
//
//    dbgClient() 
//    {
//        m_callbacks = NULL;
//        
//        IDebugClient4     *client = NULL;
//        DebugCreate( __uuidof(IDebugClient4), (void **)&client );
//    
//        m_ext = new DbgExt( client );
//        
//        client->Release();
//    }
//    
//    ~dbgClient() 
//    {
//        removeEventsMgr();
//            
//        delete m_ext;
//    }
//    
//    void startEventsMgr() {
// 
//         m_callbacks = new DbgEventCallbacksManager( m_ext->client );    
//    }
//    
//    void removeEventsMgr() {
//    
//        if ( m_callbacks )
//        {
//            delete m_callbacks;
//            m_callbacks = NULL;
//        }
//    }
//    
//private:
//
//    DbgExt                          *m_ext;
//    DbgEventCallbacksManager        *m_callbacks;  
//};
//
//extern dbgClient    g_dbgClient;
//
/////////////////////////////////////////////////////////////////////////////////