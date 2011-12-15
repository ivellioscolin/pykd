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
#include "cpureg.h"
#include "inteventhandler.h"
#include "synsymbol.h"

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

    void breakin();

    bool compareMemory( ULONG64 addr1, ULONG64 addr2, ULONG length, bool phyAddr = FALSE );

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

    std::string findSymbol( ULONG64 offset );

    ULONG64 getCurrentProcess();

    python::list getCurrentStack();

    python::tuple getDebuggeeType();

    ULONG64 getImplicitThread();

    ULONG getExecutionStatus();

    ULONG64 getOffset( const std::wstring  symbolname );

    std::string getPdbFile( ULONG64 moduleBase );

    std::string getProcessorMode();

    std::string getProcessorType();

    python::list getThreadList();

    template<ULONG status>
    void changeDebuggerStatus();

    bool is64bitSystem();

    bool isKernelDebugging();

    bool isDumpAnalyzing();

    bool isVaValid( ULONG64 addr );

    void loadDump( const std::wstring &fileName );

    Module loadModuleByName( const std::string  &moduleName ) {
        return Module( m_client, m_symSymbols, moduleName );
    }

    Module loadModuleByOffset( ULONG64  offset ) {
        return Module( m_client, m_symSymbols, offset ); 
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

    std::string loadCStr( ULONG64 offset );

    std::wstring loadWStr( ULONG64 offset );

    ULONG ptrSize();

    ULONG64 ptrByte( ULONG64 offset );

    ULONG64 ptrWord( ULONG64 offset );

    ULONG64 ptrDWord( ULONG64 offset );

    ULONG64 ptrQWord( ULONG64 offset );

    ULONG64 ptrMWord( ULONG64 offset );

    LONG64 ptrSignByte( ULONG64 offset );

    LONG64 ptrSignWord( ULONG64 offset );

    LONG64 ptrSignDWord( ULONG64 offset );

    LONG64 ptrSignQWord( ULONG64 offset );

    LONG64 ptrSignMWord( ULONG64 offset );

    ULONG64 ptrPtr( ULONG64 offset );
    
    python::object getRegByName( const std::wstring &regName );

    python::object getRegByIndex( ULONG index );

    void setExecutionStatus( ULONG status );
    
    void waitForEvent();


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

    void addSyntheticSymbol(
        ULONG64 addr,
        ULONG size,
        const std::string &symName
    )
    {
        return m_symSymbols->add(addr, size, symName);
    }

    void delAllSyntheticSymbols()
    {
        return m_symSymbols->clear();
    }

    ULONG delSyntheticSymbol(
        ULONG64 addr
    )
    {
        return m_symSymbols->remove(addr);
    }

    ULONG delSyntheticSymbolsMask(
        const std::string &moduleName,
        const std::string &symName
    )
    {
        return m_symSymbols->removeByMask(moduleName, symName);
    }

    SynSymbolsPtr getSynSymbols() {
        return m_symSymbols;
    }

private:

    template<typename T>
    python::list
    loadArray( ULONG64 offset, ULONG count, bool phyAddr );

    //python::list
    //loadArray( ULONG64 offset, ULONG count, bool phyAddr );

    SynSymbolsPtr m_symSymbols; // DebugClient is creator
    InternalDbgEventHandler m_internalDbgEventHandler;

    DebugClient( IDebugClient4 *client );

    PyThreadStateSaver      m_pyThreadState;
};

/////////////////////////////////////////////////////////////////////////////////

extern DebugClientPtr     g_dbgClient;

void loadDump( const std::wstring &fileName );

void startProcess( const std::wstring  &processName );

void attachProcess( ULONG  processId );

void attachKernel( const std::wstring  &param );

std::string findSymbol( ULONG64 offset );

python::tuple getDebuggeeType();

ULONG getExecutionStatus();

ULONG64 getOffset( const std::wstring  symbolname );

std::string getPdbFile( ULONG64 moduleBase );

bool is64bitSystem();

bool isKernelDebugging();

bool isDumpAnalyzing();

ULONG ptrSize();

void setExecutionStatus( ULONG status );

void waitForEvent();

/////////////////////////////////////////////////////////////////////////////////
// Synthetic symbols global finctions:

inline void addSyntheticSymbol(
    ULONG64 addr,
    ULONG size,
    const std::string &symName
)
{
    return g_dbgClient->addSyntheticSymbol(addr, size, symName);
}

inline void delAllSyntheticSymbols()
{
    return g_dbgClient->delAllSyntheticSymbols();
}

inline ULONG delSyntheticSymbol(
    ULONG64 addr
)
{
    return g_dbgClient->delSyntheticSymbol(addr);
}

inline ULONG delSyntheticSymbolsMask(
    const std::string &moduleName,
    const std::string &symName
)
{
    return g_dbgClient->delSyntheticSymbolsMask(moduleName, symName);
}

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
