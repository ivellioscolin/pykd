
#include "stdafx.h"
#include "eventhandler.h"

namespace pykd {


EventHandler     *g_eventHandler = NULL;

///////////////////////////////////////////////////////////////////////////////

EventHandler::EventHandler()
{
    eventRegisterCallbacks( this );
}

///////////////////////////////////////////////////////////////////////////////

EventHandler::~EventHandler()
{
    eventRemoveCallbacks( this );
}

///////////////////////////////////////////////////////////////////////////////

DEBUG_CALLBACK_RESULT EventHandlerImpl::OnBreakpoint( ULONG bpId )
{
    BpMap::iterator  foundIt = m_bpMap.find( bpId );
    if ( foundIt == m_bpMap.end() )
        return DebugCallbackNoChange;

    python::object  bpcallback = foundIt->second;

    if ( bpcallback.is_none() )
        return DebugCallbackBreak;

    try {

        python::object resObj = bpcallback( bpId );

        if ( resObj.is_none() )
            return DebugCallbackProceed;
    
        ULONG retVal = python::extract<ULONG>( resObj );

        if ( retVal >= DebugCallbackMax )
            return DebugCallbackBreak;

        return DEBUG_CALLBACK_RESULT(retVal);
    }
    catch (const python::error_already_set &) {
    }

    return DebugCallbackBreak;
}

///////////////////////////////////////////////////////////////////////////////

};  // namespace pykd






















