#include "stdafx.h"

#include <boost/format.hpp>
#include <boost/scoped_array.hpp>

#include "dbgprocess.h"
#include "dbgext.h"
#include "dbgexcept.h"
#include "dbgtype.h"

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
getThreadList()
{
    HRESULT         hres;  
    ULONG           i;
    ULONG           oldThreadId = 0;

    ULONG   threadCount;
    hres = dbgExt->system->GetNumberThreads( &threadCount );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects::GetNumberThreads failed" );
        
    boost::scoped_array<ULONG> ids(new ULONG[threadCount]);
    hres = dbgExt->system->GetThreadIdsByIndex( 0, threadCount, ids.get(), NULL );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects::GetThreadIdsByIndex failed" );
    
    hres = dbgExt->system->GetCurrentThreadId( &oldThreadId );
    
    boost::python::list     threadList;
    
    for ( i = 0; i < threadCount; ++i )
    {
        dbgExt->system->SetCurrentThreadId( ids[i] );

        ULONG64   threadOffset;                
        hres = dbgExt->system->GetCurrentThreadDataOffset( &threadOffset );
        
        if ( FAILED( hres ) )
        {
            dbgExt->system->SetCurrentThreadId( oldThreadId );
            throw DbgException( "IDebugSystemObjects::GetCurrentThreadDataOffset failed" );
        }                
            
        threadList.append( threadOffset );            
    }
    
    return threadList;  
}

/////////////////////////////////////////////////////////////////////////////////

void
setImplicitThread( 
    ULONG64  newThreadAddr )
{
    HRESULT                 hres;  

    newThreadAddr = addr64(newThreadAddr);
    hres = dbgExt->system2->SetImplicitThreadDataOffset( newThreadAddr );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects2::SetImplicitThreadDataOffset  failed" ); 
}

/////////////////////////////////////////////////////////////////////////////////  
    
ULONG64
getImplicitThread()
{
    HRESULT     hres; 
    ULONG64     threadOffset = -1;    

    hres = dbgExt->system2->GetImplicitThreadDataOffset( &threadOffset );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects2::GetImplicitThreadDataOffset  failed" ); 
        
    return threadOffset;            
}     

/////////////////////////////////////////////////////////////////////////////////  

boost::python::object
getCurrentStack()
{
    HRESULT                 hres;  
    ULONG                   currentScope = 0;       
    
    hres = dbgExt->symbols3->GetCurrentScopeFrameIndex( &currentScope );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol3::GetCurrentScopeFrameIndex  failed" );      

    boost::scoped_array<DEBUG_STACK_FRAME> frames(new DEBUG_STACK_FRAME [ 1000 ]);

    ULONG   filledFrames;
    hres = dbgExt->control->GetStackTrace( 0, 0, 0, frames.get(), 1000, &filledFrames );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetStackTrace  failed" );         
    
    boost::python::list    frameList;
    
    for ( ULONG i = 0; i < filledFrames; ++i )
    {
        dbgStackFrameClass          frame( frames[i] );
        
        boost::python::object       frameObj( frame ); 

        hres = dbgExt->symbols->SetScope( NULL, &frames[i], NULL, sizeof(DEBUG_STACK_FRAME) );
        if ( SUCCEEDED( hres ) )
            frameObj.attr( "locals" ) = getLocals();    
    
        frameList.append( frameObj );
    }         
    
    dbgExt->symbols3->SetScopeFrameByIndex( currentScope );	    	        

    return frameList;       
}

/////////////////////////////////////////////////////////////////////////////////
// Processor type to string
/////////////////////////////////////////////////////////////////////////////////
static std::string processorToStr(ULONG processorMode)
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

/////////////////////////////////////////////////////////////////////////////////

std::string
getProcessorMode()
{
    HRESULT                 hres;

    ULONG   processorMode;
    hres = dbgExt->control->GetEffectiveProcessorType( &processorMode );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetEffectiveProcessorType  failed" );

    return processorToStr(processorMode);
}

/////////////////////////////////////////////////////////////////////////////////

std::string 
getProcessorType()
{
    HRESULT hres;

    ULONG processorMode;
    hres = dbgExt->control->GetActualProcessorType( &processorMode );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::GetEffectiveProcessorType  failed" );

    return processorToStr(processorMode);
}

/////////////////////////////////////////////////////////////////////////////////

void
setProcessorMode(
    const std::string &mode )
{
    HRESULT         hres;  
    ULONG           processorMode = ~0;

    if ( mode == "X86" )
        processorMode = IMAGE_FILE_MACHINE_I386;
    else if ( mode == "ARM" )
        processorMode = IMAGE_FILE_MACHINE_ARM;
    else if ( mode == "IA64" )
        processorMode = IMAGE_FILE_MACHINE_IA64;
    else if ( mode == "X64" )
        processorMode = IMAGE_FILE_MACHINE_AMD64;
    else
        throw DbgException( "Unknown processor type" );

    hres = dbgExt->control->SetEffectiveProcessorType( processorMode );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::SetEffectiveProcessorType  failed" );

}

/////////////////////////////////////////////////////////////////////////////////

ULONG64
getCurrentProcess()
{
    HRESULT         hres;  
    ULONG64         processAddr = 0;
    
    hres = dbgExt->system2->GetImplicitProcessDataOffset( &processAddr );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects2::GetImplicitProcessDataOffset  failed" ); 
        
     return processAddr;                  
}

/////////////////////////////////////////////////////////////////////////////////

VOID
setCurrentProcess(
    ULONG64  processAddr )
{
    HRESULT     hres;

    processAddr = addr64(processAddr);
    hres = dbgExt->system2->SetImplicitProcessDataOffset( processAddr );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSystemObjects2::SetImplicitProcessDataOffset  failed" ); 
}

/////////////////////////////////////////////////////////////////////////////////

dbgStackFrameClass::dbgStackFrameClass( const DEBUG_STACK_FRAME &stackFrame )
{
    memcpy( static_cast<DEBUG_STACK_FRAME*>( this ), &stackFrame, sizeof(DEBUG_STACK_FRAME) );           
}

/////////////////////////////////////////////////////////////////////////////////

std::string
dbgStackFrameClass::print() const
{
	boost::format fmt(dbgExt->control->IsPointer64Bit() == S_OK ? "%1$4d %2$16x" : "%1$4d %2$08x");
	fmt % FrameNumber % ReturnOffset;
	return fmt.str();
}

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
getLocals()
{
    HRESULT                         hres;
    CComPtr<IDebugSymbolGroup>      localSymbols;
    CComPtr<IDebugSymbolGroup2>     localSymbols2;
    
    hres = dbgExt->symbols->GetScopeSymbolGroup( DEBUG_SCOPE_GROUP_ARGUMENTS|DEBUG_SCOPE_GROUP_LOCALS, NULL, &localSymbols  );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbols::GetScopeSymbolGroup  failed" );     
            
    hres = localSymbols->QueryInterface( __uuidof(IDebugSymbolGroup2), (void**) &localSymbols2 );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbols::QueryInterface  failed to get IDebugSymbolGroup2" );  
            
    ULONG   localNumber;            
    hres = localSymbols->GetNumberSymbols( &localNumber );       
    
    boost::python::dict     arr; 
    
    for ( ULONG i = 0; i < localNumber; ++i )
    {
        char        varName[0x100];
    
        hres = localSymbols->GetSymbolName( i, varName, sizeof(varName), NULL );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSymbolGroup::GetSymbolName  failed" ); 
            
        DEBUG_SYMBOL_PARAMETERS     symbolParam = {};                
        hres = localSymbols->GetSymbolParameters( i, 1, &symbolParam );                    
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSymbolGroup::GetSymbolParameters  failed" );              
      
        char        typeName[0x100];
        hres = dbgExt->symbols->GetTypeName( symbolParam.Module, symbolParam.TypeId, typeName, sizeof(typeName), NULL );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSymbols::GetTypeName  failed" );    
            
        char        moduleName[0x100];
        hres = dbgExt->symbols2->GetModuleNameString( 
            DEBUG_MODNAME_MODULE,
            DEBUG_ANY_ID,
            symbolParam.Module,
            moduleName,
            sizeof(moduleName),
            NULL );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSymbols2::GetModuleNameString  failed" );               
            
            
        ULONG64     varOffset;                
        hres = localSymbols2->GetSymbolOffset( i, &varOffset );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSymbolGroup2::GetSymbolOffset  failed" );               
                        
        arr[ varName ] = TypedVar( moduleName, typeName, varOffset );
    }    
    
    return arr;
}

/////////////////////////////////////////////////////////////////////////////////


