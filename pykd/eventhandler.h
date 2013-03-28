// 
// Internal debug event handler
// 

#pragma once

#include "dbgengine.h"
#include "bpoint.h"
#include "dbgexcept.h"

namespace pykd {

//////////////////////////////////////////////////////////////////////////////////

class EventHandler : public DEBUG_EVENT_CALLBACK {

public:

    EventHandler();

    virtual ~EventHandler();

private:

    virtual DEBUG_CALLBACK_RESULT OnBreakpoint( ULONG bpId ) = 0;
    virtual DEBUG_CALLBACK_RESULT OnModuleLoad( ULONG64 offset, const std::string &name ) = 0;
    virtual DEBUG_CALLBACK_RESULT OnModuleUnload( ULONG64 offset, const std::string &name ) = 0;
    virtual DEBUG_CALLBACK_RESULT OnException( ExceptionInfoPtr exceptInfo ) = 0;
    virtual void onExecutionStatusChange( ULONG executionStatus ) = 0;
    virtual void onSymbolsLoaded(ULONG64 modBase) = 0;
    virtual void onSymbolsUnloaded(ULONG64 modBase OPTIONAL) = 0;
};

//////////////////////////////////////////////////////////////////////////////////


class EventHandlerWrap : public python::wrapper<EventHandler>, public EventHandler
{

public:

    virtual DEBUG_CALLBACK_RESULT OnBreakpoint( ULONG Id ) {
        return handler("onBreakpoint", Id);
    }

    virtual DEBUG_CALLBACK_RESULT OnModuleLoad( ULONG64 offset, const std::string &name ) {
        return handler("onModuleLoad", offset, name );
    }

    virtual DEBUG_CALLBACK_RESULT OnModuleUnload( ULONG64 offset, const std::string &name ) {
        return handler("onModuleUnload", offset, name );
    }

    virtual DEBUG_CALLBACK_RESULT OnException( ExceptionInfoPtr exceptInfo ) {
        return handler("onException", exceptInfo );
    }

    virtual void onExecutionStatusChange( ULONG executionStatus ) {
        void_handler("onExecutionStatusChange", executionStatus );
    }

    virtual void onSymbolsLoaded(ULONG64 modBase) {
        void_handler("onSymbolsLoaded", modBase);
    }

    virtual void onSymbolsUnloaded(ULONG64 modBase OPTIONAL) {
        void_handler("onSymbolsUnloaded", modBase);
    }

private:

    template<typename Arg1Type>
    void void_handler( const char* handlerName, Arg1Type arg1 )
    {
        if (python::override pythonHandler = get_override( handlerName ))
        {
            try {
               pythonHandler(arg1);
            }
            catch (const python::error_already_set &)  {
                printException();
            }
        }
    }


    template<typename Arg1Type>
    DEBUG_CALLBACK_RESULT handler( const char* handlerName, Arg1Type arg1 )
    {
        if (python::override pythonHandler = get_override( handlerName ))
        {
            try {
                pythonHandler(arg1);
            }
            catch (const python::error_already_set &)  {
                printException();
            }
        }
        return DebugCallbackNoChange;
    }

    template<typename Arg1Type, typename Arg2Type>
    DEBUG_CALLBACK_RESULT handler( const char* handlerName, Arg1Type arg1, Arg2Type arg2 )
    {
        if (python::override pythonHandler = get_override( handlerName ))
        {
            try {
                return pythonHandler(arg1,arg2);
            }
            catch (const python::error_already_set &)  {
                printException();
            }
        }
        return DebugCallbackNoChange;
    }

    DEBUG_CALLBACK_RESULT handler( const char* handlerName )
    {
        if (python::override pythonHandler = get_override( handlerName ))
        {
            try {
                return pythonHandler();
            }
            catch (const python::error_already_set &) {
                printException();
            }
        }

        return DebugCallbackNoChange;
    }

}; 

typedef boost::shared_ptr<EventHandlerWrap> EventHandlerPtr;

//////////////////////////////////////////////////////////////////////////////////

class EventHandlerImpl : public EventHandlerWrap
{
public:
    void addBpCallback( ULONG id, python::object &callback )
    {
        m_bpMap.insert( std::make_pair( id, callback ) );
    }

    void removeBpCallback( ULONG id )
    {
        BpMap::iterator  foundIt = m_bpMap.find( id );
        if ( foundIt != m_bpMap.end() )
            m_bpMap.erase( foundIt );
    }

    virtual ~EventHandlerImpl()
    {
        while( !m_bpMap.empty() )
        {
            BpMap::iterator  it = m_bpMap.begin();
            breakPointRemove( it->first );
            m_bpMap.erase( it );
        }
    }

private:

    virtual DEBUG_CALLBACK_RESULT OnBreakpoint( ULONG bpId );

private:

    typedef std::map<ULONG, python::object>  BpMap;

    BpMap  m_bpMap;
};

//////////////////////////////////////////////////////////////////////////////////

};  // namespace pykd
