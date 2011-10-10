#include "stdafx.h"

#include "dbgcmd.h"
#include "dbgclient.h"

namespace pykd {

/////////////////////////////////////////////////////////////////////////////////

std::string  DebugClient::dbgCommand( const std::wstring &command )
{
    HRESULT     hres;

    OutputReader        outReader( m_client );

    PyThreadState   *pystate = PyEval_SaveThread();

    hres = m_control->ExecuteWide( DEBUG_OUTCTL_THIS_CLIENT, command.c_str(), 0 );        

    PyEval_RestoreThread( pystate );

    if ( FAILED( hres ) )
        throw  DbgException( "IDebugControl::Execute  failed" ); 

    return std::string( outReader.Line() ); 
}

std::string dbgCommand( const std::wstring  &command )
{
    return g_dbgClient->dbgCommand( command );  
}

/////////////////////////////////////////////////////////////////////////////////

DbgExtension::DbgExtension( IDebugClient4 *client, const std::wstring &extPath ) :
    DbgObject( client )
{
    HRESULT     hres;

    hres = m_control->AddExtensionWide( extPath.c_str(), 0, &m_handle );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugControl::AddExtension failed" );    
}

/////////////////////////////////////////////////////////////////////////////////
    
DbgExtension::~DbgExtension()
{
    m_control->RemoveExtension( m_handle );
}

/////////////////////////////////////////////////////////////////////////////////

std::string DbgExtension::call( const std::wstring &command, const std::wstring  &params )
{
    HRESULT     hres;

    OutputReader        outReader(  m_client );

    PyThreadState   *pystate = PyEval_SaveThread();

    hres = m_control->CallExtensionWide( m_handle, command.c_str(), NULL );

    PyEval_RestoreThread( pystate );

    if ( FAILED( hres ) )
        throw  DbgException( "IDebugControl::CallExtension  failed" ); 
        
    return std::string( outReader.Line() );    
}

/////////////////////////////////////////////////////////////////////////////////

DbgExtensionPtr
loadExtension( const std::wstring &extPath )
{
    return g_dbgClient->loadExtension( extPath );            
}

/////////////////////////////////////////////////////////////////////////////////

ULONG64
DebugClient::evaluate( const std::wstring  &expression )
{
    HRESULT             hres;
    ULONG64             value = 0;

    DEBUG_VALUE  debugValue = {};
    ULONG        remainderIndex = 0;

    hres = m_control->IsPointer64Bit();
    if ( FAILED( hres ) )
        throw  DbgException( "IDebugControl::IsPointer64Bit  failed" );
    
    if ( hres == S_OK )
    {
        hres = m_control->EvaluateWide( 
            expression.c_str(), 
            DEBUG_VALUE_INT64,
            &debugValue,
            &remainderIndex );
            
        if ( FAILED( hres ) )
            throw  DbgException( "IDebugControl::Evaluate  failed" );
            
        if ( remainderIndex == expression.length() )
            value = debugValue.I64;
    }
    else
    {
        hres = m_control->EvaluateWide( 
            expression.c_str(), 
            DEBUG_VALUE_INT32,
            &debugValue,
            &remainderIndex );
            
        if (  FAILED( hres ) )
            throw  DbgException( "IDebugControl::Evaluate  failed" );
            
        if ( remainderIndex == expression.length() )
            value = debugValue.I32;
    }      

    return value;
}

ULONG64
evaluate( const std::wstring  &expression )
{
    return g_dbgClient->evaluate( expression );
}

/////////////////////////////////////////////////////////////////////////////////

} // end namespace pykd











//#include <boost/format.hpp>
//
//#include "dbgext.h"
//#include "dbgcmd.h"
//#include "dbgexcept.h"
//#include "dbgio.h"
//#include "dbgsystem.h"





/////////////////////////////////////////////////////////////////////////////////
//
//std::string
//dbgCommand( const std::wstring &command )
//{
//    HRESULT     hres;
//
//    OutputReader        outReader(  dbgExt->client );
//    {
//        PyThread_StateRestore pyThreadRestore;
//
//        hres = dbgExt->control4->ExecuteWide( DEBUG_OUTCTL_THIS_CLIENT, command.c_str(), 0 );
//    }
//    if ( FAILED( hres ) )
//        throw  DbgException( "IDebugControl::Execute  failed" ); 
//
//    return std::string( outReader.Line() );
//}
//
/////////////////////////////////////////////////////////////////////////////////
//
//dbgExtensionClass::dbgExtensionClass( const char* path ) : m_path(path)
//{
//    HRESULT     hres;
//
//    hres = dbgExt->control->AddExtension( path, 0, &m_handle );
//    if ( FAILED( hres ) )
//        throw DbgException( "IDebugControl::AddExtension failed" );
//}
//
/////////////////////////////////////////////////////////////////////////////////
//    
//dbgExtensionClass::~dbgExtensionClass()
//{
//    if ( m_handle )
//        dbgExt->control->RemoveExtension( m_handle );
//}
//
/////////////////////////////////////////////////////////////////////////////////
//
//std::string
//dbgExtensionClass::call( const std::string &command, const std::string params )
//{
//    HRESULT     hres;
//
//    OutputReader        outReader(  dbgExt->client );
//
//    hres = dbgExt->control->CallExtension( m_handle, command.c_str(), params.c_str() );
//    if ( FAILED( hres ) )
//        throw  DbgException( "IDebugControl::CallExtension  failed" ); 
//        
//    return std::string( outReader.Line() );
//}
//
/////////////////////////////////////////////////////////////////////////////////
//
//std::string
//dbgExtensionClass::print() const
//{
//    return m_handle ? m_path : "";
//}
//
///////////////////////////////////////////////////////////////////////////////// 
//
//ULONG64
//evaluate( const std::string  &expression )
//{
//    HRESULT             hres;
//    ULONG64             value = 0;
//
//    DEBUG_VALUE  debugValue = {};
//    ULONG        remainderIndex = 0;
//    
//    if ( is64bitSystem() )
//    {
//        hres = dbgExt->control->Evaluate( 
//            expression.c_str(), 
//            DEBUG_VALUE_INT64,
//            &debugValue,
//            &remainderIndex );
//            
//        if (  FAILED( hres ) )
//            throw  DbgException( "IDebugControl::Evaluate  failed" );
//            
//        if ( remainderIndex == expression.length() )
//            value = debugValue.I64;
//    }
//    else
//    {
//        hres = dbgExt->control->Evaluate( 
//            expression.c_str(), 
//            DEBUG_VALUE_INT32,
//            &debugValue,
//            &remainderIndex );
//            
//        if (  FAILED( hres ) )
//            throw  DbgException( "IDebugControl::Evaluate  failed" );
//            
//        if ( remainderIndex == expression.length() )
//            value = debugValue.I32;
//    }      
//
//    return value;
//}
//
///////////////////////////////////////////////////////////////////////////////// 
//
//void
//breakin()
//{
//    HRESULT     hres;
//
//    {
//        PyThread_StateRestore pyThreadRestore;
//        hres = dbgExt->control->SetInterrupt( DEBUG_INTERRUPT_ACTIVE );
//    }
//
//    if ( FAILED( hres ) )
//        throw DbgException( "IDebugControl::SetInterrupt" );
//}
//
///////////////////////////////////////////////////////////////////////////////// 