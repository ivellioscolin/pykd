#include "stdafx.h"

#include "dbgext.h"
#include "dbgstack.h"
#include "dbgexcept.h"

/////////////////////////////////////////////////////////////////////////////////

boost::python::object
getStack( ULONG64 stackPtr )
{
    HRESULT                 hres;  
    PDEBUG_STACK_FRAME      frames = NULL;

    try {
    
        frames = new DEBUG_STACK_FRAME [ 1000 ];
    
        ULONG   filledFrames;
        hres = dbgExt->control->GetStackTrace( stackPtr, stackPtr, stackPtr, frames, 1000, &filledFrames );
        
        dbgExt->control->OutputStackTrace( DEBUG_OUTCTL_THIS_CLIENT, frames, filledFrames, DEBUG_STACK_FUNCTION_INFO | DEBUG_STACK_FRAME_ADDRESSES );
        
    	if ( frames )
	        delete[] frames;

        return boost::python::object();        
    
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