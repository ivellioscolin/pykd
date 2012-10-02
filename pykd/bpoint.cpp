// 
// Breakpoints management
// 

#include "stdafx.h"
#include "bpoint.h"
#include "dbgengine.h"
#include "eventhandler.h"

//////////////////////////////////////////////////////////////////////////////

namespace pykd {

//////////////////////////////////////////////////////////////////////////////

ULONG setSoftwareBp(ULONG64 offset, python::object &callback/* = python::object() */)
{
    offset = addr64( offset );

    ULONG id = breakPointSet( offset );

    python::object main = boost::python::import("__main__");
    python::dict   main_namespace( main.attr("__dict__") );
    EventHandlerPtr  eventHandler = python::extract<EventHandlerPtr>( main_namespace["globalEventHandler"] );
    EventHandlerImpl* handler = dynamic_cast<EventHandlerImpl*>( boost::get_pointer( eventHandler ) );
    handler->addBpCallback( id, callback );

    return id;
}

///////////////////////////////////////////////////////////////////////////////

ULONG setHardwareBp(ULONG64 offset, ULONG size, ULONG accessType,  python::object &callback/* = python::object() */ )
{
    offset = addr64( offset );

    ULONG id = breakPointSet( offset, true, size, accessType );

    python::object main = boost::python::import("__main__");
    python::dict   main_namespace( main.attr("__dict__") );
    EventHandlerPtr  eventHandler = python::extract<EventHandlerPtr>( main_namespace["globalEventHandler"] );
    EventHandlerImpl* handler = dynamic_cast<EventHandlerImpl*>( boost::get_pointer( eventHandler ) );
    handler->addBpCallback( id, callback );

    return id;
}

///////////////////////////////////////////////////////////////////////////////

void removeBp( ULONG id )
{
    breakPointRemove( id );

    python::object main = boost::python::import("__main__");
    python::dict   main_namespace( main.attr("__dict__") );
    EventHandlerPtr  eventHandler = python::extract<EventHandlerPtr>( main_namespace["globalEventHandler"] );
    EventHandlerImpl* handler = dynamic_cast<EventHandlerImpl*>( boost::get_pointer( eventHandler ) );
    handler->removeBpCallback( id );
}

///////////////////////////////////////////////////////////////////////////////

void removeAllBp()
{
   // breakPointRemoveAll();
}

///////////////////////////////////////////////////////////////////////////////

}   // end pykd namespace
