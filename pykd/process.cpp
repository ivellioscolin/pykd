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

python::list 
DebugClient::getCurrentStack()
{
    HRESULT     hres;

    ULONG   filledFrames;
    std::vector<DEBUG_STACK_FRAME>      frames(1000); 

    hres = m_control->GetStackTrace( 0, 0, 0, &frames[0], 1000, &filledFrames );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetStackTrace  failed" );         
    
    python::list    frameList;
    
    for ( ULONG i = 0; i < filledFrames; ++i )
    {
        python::object       frameObj( frames[i] ); 
    
        frameList.append( frameObj );
    }         

    return frameList; 
}

python::list 
getCurrentStack()
{
    return g_dbgClient->getCurrentStack();
}

///////////////////////////////////////////////////////////////////////////////////

}