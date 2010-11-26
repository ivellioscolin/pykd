#include "stdafx.h"

#include "dbgprocess.h"
#include "dbgext.h"
#include "dbgexcept.h"

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
getThreadList()
{
    HRESULT         hres;  
    PULONG          ids = NULL;
    ULONG           i;
    ULONG           oldThreadId = 0;

    try {

        ULONG   threadCount;
        hres = dbgExt->system->GetNumberThreads( &threadCount );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSystemObjects::GetNumberThreads failed" );
            
        ids = new ULONG[threadCount];            
        hres = dbgExt->system->GetThreadIdsByIndex( 0, threadCount, ids, NULL );
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
                throw DbgException( "IDebugSystemObjects::GetCurrentThreadDataOffset failed" );
                
            threadList.append( threadOffset );            
        }
        
        if ( ids )
            delete[] ids;
        
        return threadList;  
    }
  	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}
	
	if ( oldThreadId )
        dbgExt->system->SetCurrentThreadId( oldThreadId );
        
    if ( ids )
        delete[] ids;        
	
	return  boost::python::list();
}

/////////////////////////////////////////////////////////////////////////////////

bool
setImplicitThread( 
    ULONG64  newThreadAddr )
{
    HRESULT                 hres;  

    try {
  
        hres = dbgExt->system2->SetImplicitThreadDataOffset( newThreadAddr );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSystemObjects2::SetImplicitThreadDataOffset  failed" ); 
            
        return true;            
        
    }
  	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}
	
	return false;	
}  


/////////////////////////////////////////////////////////////////////////////////  
    
ULONG64
getImplicitThread()
{
    HRESULT                 hres;  

    try {
        
        ULONG64   threadOffset = -1;    
  
        hres = dbgExt->system2->GetImplicitThreadDataOffset( &threadOffset );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugSystemObjects2::GetImplicitThreadDataOffset  failed" ); 
            
        return threadOffset;            
        
    }
  	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}
	
	return -1;	
}     

/////////////////////////////////////////////////////////////////////////////////  

boost::python::object
getCurrentStack()
{
    HRESULT                 hres;  
    PDEBUG_STACK_FRAME      frames = NULL;

    try {
    
        frames = new DEBUG_STACK_FRAME [ 1000 ];
    
        ULONG   filledFrames;
        hres = dbgExt->control->GetStackTrace( 0, 0, 0, frames, 1000, &filledFrames );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugControl::GetStackTrace  failed" );         
        
        boost::python::list    frameList;
        
        for ( ULONG i = 0; i < filledFrames; ++i )
        {
            frameList.append( boost::python::object( dbgStackFrameClass( frames[i] ) ) );
        }         
        
    	if ( frames )
	        delete[] frames;

        return frameList;       
    
    }
  	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}
	
	if ( frames )
	    delete[] frames;

    return boost::python::object(); 
}

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
getProcessorMode()
{
    HRESULT                 hres;  

    try {

        ULONG   processorMode;
        hres = dbgExt->control->GetEffectiveProcessorType( &processorMode );
        if ( FAILED( hres ) )
            throw DbgException( "IDebugControl::GetEffectiveProcessorType  failed" );       
        
        switch( processorMode )
        {
        case IMAGE_FILE_MACHINE_I386:
            return boost::python::str("X86");
            
        case IMAGE_FILE_MACHINE_ARM:
            return boost::python::str("ARM");
            
        case IMAGE_FILE_MACHINE_IA64:
            return boost::python::str("IA64");
            
        case IMAGE_FILE_MACHINE_AMD64:
            return boost::python::str("X64");

        }
    }
  	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}
	
    return boost::python::object(); 
}

/////////////////////////////////////////////////////////////////////////////////

void
setProcessorMode(
    const std::string &mode )
{
    HRESULT                 hres;  

    try {
    
        ULONG       processorMode = ~0;
    
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
  	catch( std::exception  &e )
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd error: %s\n", e.what() );
	}
	catch(...)
	{
		dbgExt->control->Output( DEBUG_OUTPUT_ERROR, "pykd unexpected error\n" );
	}
}    

/////////////////////////////////////////////////////////////////////////////////