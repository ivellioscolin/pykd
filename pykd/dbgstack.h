#pragma once

#include <boost/python.hpp>
#include <boost/python/object.hpp>

/////////////////////////////////////////////////////////////////////////////////

bool
setImplicitThread( 
    ULONG64  newThreadAddr,
    PULONG64  oldThreadAddr );
    
ULONG64
getImplicitThread();        


boost::python::object
getCurrentStack();

class dbgStackFrameClass : public DEBUG_STACK_FRAME {

public:

    dbgStackFrameClass()
    {
        memset( static_cast<DEBUG_STACK_FRAME*>( this ), 0, sizeof(DEBUG_STACK_FRAME) );
    }       
    
    dbgStackFrameClass( const DEBUG_STACK_FRAME &stackFrame )
    {
        memcpy( static_cast<DEBUG_STACK_FRAME*>( this ), &stackFrame, sizeof(DEBUG_STACK_FRAME) );   
    }
};

/////////////////////////////////////////////////////////////////////////////////