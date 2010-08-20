#include "stdafx.h"

#include "dbgext.h"
#include "dbgstack.h"
#include "dbgexcept.h"

/////////////////////////////////////////////////////////////////////////////////

bool
setImplicitThread( 
    ULONG64  newThreadAddr,
    PULONG64  oldThreadAddr )
{
    HRESULT                 hres;  

    try {
    
        *oldThreadAddr = getImplicitThread();
        if ( *oldThreadAddr == -1 )
            return false;
    
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