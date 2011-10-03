#pragma once

#include <string>
#include <dbgeng.h>
#include <dbghelp.h>

#include <boost\smart_ptr\scoped_ptr.hpp>

#include "dbgexcept.h"
#include "module.h"

/////////////////////////////////////////////////////////////////////////////////

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////

class DebugClient;
typedef boost::shared_ptr<DebugClient>  DebugClientPtr;

/////////////////////////////////////////////////////////////////////////////////

class DebugClient {

public:

    virtual ~DebugClient() {}

    static
    DebugClientPtr createDbgClient() {
        return DebugClientPtr( new DebugClient() );
    }

    static
    DebugClientPtr createDbgClient( IDebugClient4 *client ) {
        return DebugClientPtr( new DebugClient(client) );
    }

    static
    DebugClientPtr  setDbgClientCurrent( DebugClientPtr  newDbgClient );
 
    void loadDump( const std::wstring &fileName );

    void startProcess( const std::wstring  &processName );

    void attachProcess( ULONG  processId );

    void attachKernel( const std::wstring  &param );

    Module loadModule( const std::string  &moduleName ) {
        return Module( m_client, moduleName );
    }

    Module findModule( ULONG64  offset ) {
        return Module( m_client, offset ); 
    }

    ULONG64  addr64( ULONG64 addr );

    void dprint( const std::string &str, bool dml = false );

    void dprintln( const std::string &str, bool dml = false );

    void eprint( const std::string &str );

    void eprintln( const std::string &str );

private:

    DebugClient();

    DebugClient( IDebugClient4 *client );

    CComPtr<IDebugClient5>      m_client;     
    CComPtr<IDebugControl4>     m_control;
    CComPtr<IDebugSymbols3>     m_symbols;
};

/////////////////////////////////////////////////////////////////////////////////

extern DebugClientPtr     g_dbgClient;

void loadDump( const std::wstring &fileName );

void startProcess( const std::wstring  &processName );

void attachProcess( ULONG  processId );

void attachKernel( const std::wstring  &param );

Module loadModule( const std::string  &moduleName );

Module findModule( ULONG64  offset );

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