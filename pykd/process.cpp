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

std::string processorToStr(ULONG processorMode)
{
    switch( processorMode )
    {
    case IMAGE_FILE_MACHINE_I386:
        return "X86";
        
    case IMAGE_FILE_MACHINE_ARM:
        return "ARM";
        
    case IMAGE_FILE_MACHINE_IA64:
        return "IA64";
        
    case IMAGE_FILE_MACHINE_AMD64:
        return "X64";
    }

    throw DbgException( "Unknown CPU type" );
}

///////////////////////////////////////////////////////////////////////////////////

std::string DebugClient::getProcessorMode()
{
    HRESULT         hres;
    ULONG           processorMode;

    hres = m_control->GetEffectiveProcessorType( &processorMode );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetEffectiveProcessorType  failed" );

    return processorToStr(processorMode);        
}

std::string getProcessorMode()
{
    return g_dbgClient->getProcessorMode();
}

///////////////////////////////////////////////////////////////////////////////////

std::string DebugClient::getProcessorType()
{
    HRESULT     hres;
    ULONG       processorMode;

    hres = m_control->GetActualProcessorType( &processorMode );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetActualProcessorType  failed" );

    return processorToStr(processorMode);              
}

std::string getProcessorType()
{
    return g_dbgClient->getProcessorType();
}

///////////////////////////////////////////////////////////////////////////////////

python::list DebugClient::getThreadList()
{
    HRESULT         hres;  
    ULONG           i;
    ULONG           oldThreadId = 0;
    ULONG           threadCount;

    hres = m_system->GetNumberThreads( &threadCount );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects::GetNumberThreads failed" );
        
    std::vector<ULONG> threadIds(threadCount);
    hres = m_system->GetThreadIdsByIndex( 0, threadCount, &threadIds[0], NULL );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects::GetThreadIdsByIndex failed" );
    
    hres = m_system->GetCurrentThreadId( &oldThreadId );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects::GetCurrentThreadId failed" );   
    
    boost::python::list     threadList;
    
    for ( i = 0; i < threadCount; ++i )
    {
        m_system->SetCurrentThreadId( threadIds[i] );

        ULONG64   threadOffset;                
        hres = m_system->GetCurrentThreadDataOffset( &threadOffset );
        
        if ( FAILED( hres ) )
        {
            m_system->SetCurrentThreadId( oldThreadId );
            throw DbgException( "IDebugSystemObjects::GetCurrentThreadDataOffset failed" );
        }                
            
        threadList.append( threadOffset );            
    }

    m_system->SetCurrentThreadId( oldThreadId );
    
    return threadList; 
}

python::list getThreadList()
{
    return g_dbgClient->getThreadList();     
}

///////////////////////////////////////////////////////////////////////////////////

void DebugClient::setCurrentProcess( ULONG64 processAddr )
{
    HRESULT     hres;

    processAddr = addr64(processAddr);
    hres = m_system->SetImplicitProcessDataOffset( processAddr );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects2::SetImplicitProcessDataOffset  failed" );         
}

void setCurrentProcess( ULONG64 processAddr )
{
    g_dbgClient->setCurrentProcess( processAddr );
}

///////////////////////////////////////////////////////////////////////////////////

void DebugClient::setImplicitThread( ULONG64 threadAddr )
{
    HRESULT                 hres;  

    threadAddr = addr64(threadAddr);
    hres = m_system->SetImplicitThreadDataOffset( threadAddr );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects2::SetImplicitThreadDataOffset  failed" );         
}

void setImplicitThread( ULONG64 threadAddr )
{
    g_dbgClient->setImplicitThread( threadAddr );
}

///////////////////////////////////////////////////////////////////////////////////

void DebugClient::setProcessorMode( const std::wstring &mode )
{
    HRESULT         hres;  
    ULONG           processorMode;

    if ( mode == L"X86" )
        processorMode = IMAGE_FILE_MACHINE_I386;
    else if ( mode == L"ARM" )
        processorMode = IMAGE_FILE_MACHINE_ARM;
    else if ( mode == L"IA64" )
        processorMode = IMAGE_FILE_MACHINE_IA64;
    else if ( mode == L"X64" )
        processorMode = IMAGE_FILE_MACHINE_AMD64;
    else
        throw DbgException( "Unknown processor type" );

    hres = m_control->SetEffectiveProcessorType( processorMode );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::SetEffectiveProcessorType  failed" );
}

void setProcessorMode( const std::wstring &mode )
{
    g_dbgClient->setProcessorMode( mode );
}

///////////////////////////////////////////////////////////////////////////////////

}