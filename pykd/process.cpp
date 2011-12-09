#include "stdafx.h"
#include "dbgclient.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

ULONG64
DebugClient::getCurrentProcess()
{
    HRESULT         hres;  
    ULONG64         processAddr = 0;
    
    hres = m_system->GetImplicitProcessDataOffset( &processAddr );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects2::GetImplicitProcessDataOffset  failed" ); 
        
     return processAddr;    
}

ULONG64
getCurrentProcess()
{
    return g_dbgClient->getCurrentProcess();    
}

///////////////////////////////////////////////////////////////////////////////////

ULONG64
DebugClient::getImplicitThread()
{
    HRESULT     hres; 
    ULONG64     threadOffset = -1;    

    hres = m_system->GetImplicitThreadDataOffset( &threadOffset );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects2::GetImplicitThreadDataOffset  failed" ); 
        
    return threadOffset;            
}

ULONG64
getImplicitThread() {
    return g_dbgClient->getImplicitThread();
}

///////////////////////////////////////////////////////////////////////////////////

}